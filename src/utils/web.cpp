#include "utils/web.hpp"

#include <QDesktopServices>


bool WebUtils::openUrl(const QString &url) {
    return QDesktopServices::openUrl(url);
}

bool WebUtils::openUrl(const QUrl &url) {
    return QDesktopServices::openUrl(url);
}

