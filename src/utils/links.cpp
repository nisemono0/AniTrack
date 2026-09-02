#include "utils/links.hpp"


QString LinksUtils::createTextLink(const QString &url, const QString &text) {
    return QStringLiteral(
        "<a href=\"%1\" style=\"text-decoration:none;\">%2</a>"
    ).arg(url, text);
}

