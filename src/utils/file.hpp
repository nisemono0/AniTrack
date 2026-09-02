#pragma once

#include <QWidget>
#include <QString>
#include <QStringList>

namespace FileUtils {
// True if file_path exists
bool fileExists(const QString &file_path);

// True if file_path's extension is in ext_list
bool containsExtension(const QString &file_path, const QStringList &ext_list);

// Return absolute path of a file or home path if file doesn't exist
QString absolutePath(const QString &file_path);

// Return a sanitized QString where all dots but the extension dot
// from file_path are replaced with underscores. If extension is set
// it is used as the resulting sanitized file path's extension instead
// of the one already in file_path
QString sanitizeFilePath(const QString &file_path, const QString &extension = nullptr);

// Loads and reads the file contents as QString
QString readFile(const QString &file_path);

}; // namespace FileUtils

