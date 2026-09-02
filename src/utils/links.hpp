#pragma once

#include <QString>


namespace LinksUtils {
namespace ExternalLinks {
inline const QString AniList = QStringLiteral("https://anilist.co");
inline const QString AniChart = QStringLiteral("https://anichart.net");
inline const QString LiveChart = QStringLiteral("https://livechart.me");
} // namespace ExternalLinks
QString createTextLink(const QString &url, const QString &text);
}; // namespace LinksUtils
