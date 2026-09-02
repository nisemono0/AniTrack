#include "utils/file.hpp"

#include <QFile>
#include <QDir>
#include <QFileInfo>


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

