#include "utils/file.hpp"

#include <QFile>
#include <QDir>
#include <QFileInfo>
#include <QUrl>


bool FileUtils::fileExists(const QString &file_path) {
    if (QFile::exists(file_path)) {
        return true;
    }
    return false;
}

bool FileUtils::containsExtension(const QString &file_path, const QStringList &ext_list) {
    QFileInfo file_info = QFileInfo(file_path);
    return ext_list.contains(file_info.suffix(), Qt::CaseInsensitive);
}

QString FileUtils::absolutePath(const QString &file_path) {
    QFileInfo file_info = QFileInfo(file_path);
    return file_info.absolutePath();
}

QString FileUtils::sanitizeFilePath(const QString &file_path, const QString &extension) {
    QFileInfo file_info = QFileInfo(file_path);
    QString absolute_path = file_info.absolutePath();

    if (extension.isEmpty()) {
        QString sanitized_filename = file_info.completeBaseName().replace(".", "_") + "." + file_info.suffix();
        return QDir(absolute_path).filePath(sanitized_filename);
    }

    QString sanitized_filename = file_info.fileName().replace(".", "_") + "." + extension;
    return QDir(absolute_path).filePath(sanitized_filename);
}

QString FileUtils::readFile(const QString &file_path) {
    QFile file(file_path);
    if (file.open(QIODevice::ReadOnly)) {
        return file.readAll();
    }
    return QString();
}

std::expected<QString, QString> FileUtils::fileNameFromUrl(const QString &file_url) {
    QUrl url(file_url);

    if (!url.isValid()) {
        return std::unexpected(QStringLiteral("Invalid file url: %1").arg(file_url));
    }

    if (!url.isLocalFile()) {
        return std::unexpected(QStringLiteral("Url is not a local file: %1").arg(file_url));
    }

    QString file_path = url.toLocalFile();
    if (file_path.isEmpty()) {
        return std::unexpected(QStringLiteral("Url has an empty local path: %1").arg(file_path));
    }

    QString file_name = QFileInfo(file_path).fileName();
    if (file_name.isEmpty()) {
        return std::unexpected(QStringLiteral("Failed to extract filename from url: %1").arg(file_url));
    }

    return file_name;
}

