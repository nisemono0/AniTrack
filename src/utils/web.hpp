#pragma once

#include <QString>
#include <QUrl>


namespace WebUtils {
// Open url in web browser. Returns true on success
bool openUrl(const QString &url);
bool openUrl(const QUrl &url);
} // namespace WebUtils

