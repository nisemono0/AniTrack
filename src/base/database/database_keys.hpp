#pragma once

#include <QString>

namespace DatabaseKeys {
namespace Media {
inline const QString Id              = QStringLiteral(":id");
inline const QString IsAdult         = QStringLiteral(":is_adult");
inline const QString MediaStatus     = QStringLiteral(":media_status");
inline const QString Season          = QStringLiteral(":season");
inline const QString SeasonYear      = QStringLiteral(":season_year");
inline const QString AverageScore    = QStringLiteral(":average_score");
inline const QString SiteUrl         = QStringLiteral(":site_url");
inline const QString TitleEnglish    = QStringLiteral(":title_english");
inline const QString TitleNative     = QStringLiteral(":title_native");
inline const QString TitleRomaji     = QStringLiteral(":title_romaji");
inline const QString Synonyms        = QStringLiteral(":synonyms");
inline const QString Format          = QStringLiteral(":format");
inline const QString Description     = QStringLiteral(":description");
inline const QString StartDate       = QStringLiteral(":start_date");
inline const QString EndDate         = QStringLiteral(":end_date");
inline const QString UpdatedAt       = QStringLiteral(":updated_at");
inline const QString Episodes        = QStringLiteral(":episodes");
inline const QString CountryOfOrigin = QStringLiteral(":country_of_origin");
inline const QString CoverUrl        = QStringLiteral(":cover_url");
inline const QString Genres          = QStringLiteral(":genres");
inline const QString Studios         = QStringLiteral(":studios");
inline const QString Producers       = QStringLiteral(":producers");
} // namespace Media
namespace Entry {
inline const QString LocalId          = QStringLiteral(":local_id");
inline const QString Id               = QStringLiteral(":id");
inline const QString MediaId          = QStringLiteral(":media_id");
inline const QString IsPrivate        = QStringLiteral(":is_private");
inline const QString Status           = QStringLiteral(":status");
inline const QString Progress         = QStringLiteral(":progress");
inline const QString Repeat           = QStringLiteral(":repeat");
inline const QString Notes            = QStringLiteral(":notes");
inline const QString Score            = QStringLiteral(":score");
inline const QString StartedAt        = QStringLiteral(":started_at");
inline const QString CompletedAt      = QStringLiteral(":completed_at");
inline const QString UpdatedAt        = QStringLiteral(":updated_at");
inline const QString PendingOperation = QStringLiteral(":pending_operation");
} // namespace Entry
} // namespace DatabaseKeys
