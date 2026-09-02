#pragma once

#include "base/network/network.hpp"

#include "base/cache/image_request.hpp"

#include <QObject>
#include <QMutex>
#include <QCache>
#include <QPointer>
#include <QDir>


class ImageCache final : public QObject {
    Q_OBJECT;
public:
    explicit ImageCache(
        Network *network,
        QObject *parent = nullptr
    );
    ~ImageCache();

    bool init();

    QString cachePath() const;
    qsizetype cacheCount() const;

    ImageRequest* requestPixmap(const QString &url);

private:
    Network *network_;
    QMutex cache_mutex_;

    QDir cache_dir_;
    QCache<QString, QPixmap> memory_cache_;

    struct PendingDownload {
        QString url;
        QNetworkReply *network_reply;
        QList<QPointer<ImageRequest>> requests;
    };
    QMap<QString, PendingDownload> pending_downloads_;

    // Return a cache key from url
    QString urlToKey(const QString &url);
    // Return the cache filepath from key
    QString keyToFilePath(const QString &key);
    // Cache pixmap to memory and disk
    void cachePixmap(const QString &url, const QPixmap &pixmap, const QByteArray &format);
    // Handles download finished for key
    void handleDownloadFinished(const QString &key);
};

