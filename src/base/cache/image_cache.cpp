#include "base/cache/image_cache.hpp"

#include "utils/log.hpp"

#include <expected>

#include <QStandardPaths>
#include <QCryptographicHash>
#include <QTimer>
#include <QRestReply>
#include <QBuffer>
#include <QImageReader>

namespace {
struct PixmapResponse final {
    QPixmap pixmap;
    QByteArray format;
};

std::expected<PixmapResponse, QString> readRequestResponse(QNetworkReply *network_reply) {
    QRestReply rest_reply(network_reply);

    if (!rest_reply.isSuccess()) {
        return std::unexpected(rest_reply.errorString());
    }

    QByteArray raw_image_data = rest_reply.readBody();

    PixmapResponse response;

    if (response.pixmap.loadFromData(raw_image_data)) {
        QBuffer read_buffer;
        read_buffer.setData(raw_image_data);

        if (!read_buffer.open(QIODevice::ReadOnly)) {
            return std::unexpected(QStringLiteral("Failed to open image data for reading"));
        }

        QImageReader image_reader(&read_buffer);
        response.format = image_reader.format();
    }
    return response;
}
} // namespace


ImageCache::ImageCache(
    Network *network,
    QObject *parent
) : QObject(parent),
    network_(network) {

    this->cache_dir_.setFilter(QDir::NoDotAndDotDot | QDir::Files);
    this->cache_dir_.setPath(
        QStandardPaths::writableLocation(QStandardPaths::CacheLocation)
    );

}

ImageCache::~ImageCache() {
    for (auto &pending : this->pending_downloads_) {
        if (pending.network_reply) {
            // Disconnect before calling abort, abort instantly emits finished
            disconnect(pending.network_reply, &QNetworkReply::finished, nullptr, nullptr);
            pending.network_reply->abort();
            pending.network_reply->deleteLater();
        }
        qDeleteAll(pending.requests);
    }
}

bool ImageCache::init() {
    if (this->cache_dir_.exists()) {
        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Using cache dir: %1").arg(
                this->cachePath()
            )
        );
        return true;
    }

    Log::warning(
        CONTEXT_CLASS,
        QStringLiteral("No cache dir, trying to create it at: %1").arg(
            this->cachePath()
        )
    );

    if (!this->cache_dir_.mkpath(this->cachePath())) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to create cache dir: %1").arg(
                this->cachePath()
            )
        );
        return false;
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Created and using cache dir: %1").arg(
            this->cachePath()
        )
    );
    return true;
}

QString ImageCache::cachePath() const {
    return this->cache_dir_.absolutePath();
}

qsizetype ImageCache::cacheCount() const {
    this->cache_dir_.refresh();
    return this->cache_dir_.count();
}

ImageRequest* ImageCache::requestPixmap(const QString &url) {
    QString key = this->urlToKey(url);

    // Try to get the cached pixmap from memory first
    {
        QMutexLocker locker(&this->cache_mutex_);
        QPixmap *cached_obj = this->memory_cache_.object(key);
        if (cached_obj) {
            QPixmap cached_pixmap = *cached_obj;
            Log::info(
                CONTEXT_CLASS,
                QStringLiteral("Memory cache hit: %1 (%2)").arg(url, key)
            );

            ImageRequest *image_request = new ImageRequest(this);
            QTimer::singleShot(0, this, [image_request, cached_pixmap] {
                image_request->sendFinish(cached_pixmap);
            });
            return image_request;
        }
    }

    // Fallback to disk if memory cache failed
    QFile disk_file(this->keyToFilePath(key));
    if (disk_file.exists()) {
        QPixmap disk_pixmap;
        // Try to load cached pixmap from disk
        if (disk_pixmap.load(disk_file.fileName())) {
            // Cache to memory if loaded
            {
                QMutexLocker locker(&this->cache_mutex_);
                this->memory_cache_.insert(key, new QPixmap(disk_pixmap));
            }
            Log::info(
                CONTEXT_CLASS,
                QStringLiteral("Disk cache hit: %1 (%2)").arg(url, key)
            );
            ImageRequest *image_request = new ImageRequest(this);
            QTimer::singleShot(0, this, [image_request, disk_pixmap] {
                image_request->sendFinish(disk_pixmap);
            });
            return image_request;
        } else {
            // Cached file on disk failed to load, possibly corrupted so remove it
            Log::warning(
                CONTEXT_CLASS,
                QStringLiteral("Removing corrupted disk cache: %1 (%2)").arg(url, key)
            );
            disk_file.remove();
        }
    }

    // Not in memory and not on disk. Download it from url
    {
        QMutexLocker locker(&this->cache_mutex_);

        // Already downloading, just add the new request
        if (this->pending_downloads_.contains(key)) {
            Log::info(
                CONTEXT_CLASS,
                QStringLiteral("Alredy downloading: %1 (%2)").arg(url, key)
            );
            ImageRequest *image_request = new ImageRequest(this);
            this->pending_downloads_[key].requests.append(image_request);
            return image_request;
        }

        // Start a download for url
        PendingDownload new_download;
        new_download.url = url;

        QNetworkReply *network_reply = this->network_->getImage(url);
        new_download.network_reply = network_reply;

        ImageRequest *image_request = new ImageRequest(this);
        new_download.requests.append(image_request);

        this->pending_downloads_.insert(key, new_download);

        connect(network_reply, &QNetworkReply::finished, this, [this, key] {
            this->handleDownloadFinished(key);
        });

        return image_request;
    }
}

QString ImageCache::urlToKey(const QString &url) {
    return QString(
        QCryptographicHash::hash(url.toUtf8(), QCryptographicHash::Md5).toHex()
    );
}

QString ImageCache::keyToFilePath(const QString &key) {
    return this->cache_dir_.absoluteFilePath(key);
}

void ImageCache::cachePixmap(const QString &url, const QPixmap &pixmap, const QByteArray &format) {
    QString key = this->urlToKey(url);

    QFileInfo disk_file(this->keyToFilePath(key));
    if (!disk_file.exists()) {
        if (!pixmap.save(disk_file.absoluteFilePath(), format)) {
            Log::warning(
                CONTEXT_CLASS,
                QStringLiteral("Failed to cache pixmap to disk: %1 (%2)").arg(url, key)
            );
        } else {
            Log::info(
                CONTEXT_CLASS,
                QStringLiteral("Cached pixmap to disk: %1 (%2)").arg(url, key)
            );
        }
    }

    // Lock the memory cache
    {
        QMutexLocker locker(&this->cache_mutex_);
        if (this->memory_cache_.insert(key, new QPixmap(pixmap))) {
            Log::info(
                CONTEXT_CLASS,
                QStringLiteral("Cached pixmap to memory: %1 (%2)").arg(url, key)
            );
        } else {
            Log::warning(
                CONTEXT_CLASS,
                QStringLiteral("Failed to cache pixmap to memory: %1 (%2)").arg(url, key)
            );
        }
    }
}

void ImageCache::handleDownloadFinished(const QString &key) {
    PendingDownload download;
    {
        QMutexLocker locker(&this->cache_mutex_);
        download = this->pending_downloads_.take(key);
    }

    // Read the network reply image
    QNetworkReply *network_reply = download.network_reply;
    auto response = readRequestResponse(network_reply);
    network_reply->deleteLater();

    // All requests for the same image
    QList<QPointer<ImageRequest>> download_request = download.requests;

    if (!response) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Download failed for %1 (%2): %3").arg(download.url, key, response.error())
        );
        // Notify all requests that the request failed
        for (const auto &request : download_request) {
            if (!request.isNull()) {
                request->sendFail();
            }
        }
    } else {
        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Downloaded image %1 (%2)").arg(download.url, key)
        );
        // Cache the downloaded image
        this->cachePixmap(
            download.url,
            response.value().pixmap,
            response.value().format
        );
        // Notify all requests with the pixmap
        for (const auto &request : download_request) {
            if (!request.isNull()) {
                request->sendFinish(response.value().pixmap);
            }
        }
    }
}

