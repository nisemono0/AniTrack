#pragma once

#include <QString>


namespace DatabaseColumns {
namespace Entry {
inline const QString LocalId          = QStringLiteral("entry_local_id");
inline const QString Id               = QStringLiteral("entry_id");
inline const QString MediaId          = QStringLiteral("entry_media_id");
inline const QString IsPrivate        = QStringLiteral("entry_is_private");
inline const QString Status           = QStringLiteral("entry_status");
inline const QString Progress         = QStringLiteral("entry_progress");
inline const QString Repeat           = QStringLiteral("entry_repeat");
inline const QString Notes            = QStringLiteral("entry_notes");
inline const QString Score            = QStringLiteral("entry_score");
inline const QString StartedAt        = QStringLiteral("entry_started_at");
inline const QString CompletedAt      = QStringLiteral("entry_completed_at");
inline const QString UpdatedAt        = QStringLiteral("entry_updated_at");
inline const QString PendingOperation = QStringLiteral("entry_pending_operation");
} // namespace Entry

namespace Media {
inline const QString Id              = QStringLiteral("media_id");
inline const QString IsAdult         = QStringLiteral("media_is_adult");
inline const QString Status          = QStringLiteral("media_status");
inline const QString Season          = QStringLiteral("media_season");
inline const QString SeasonYear      = QStringLiteral("media_season_year");
inline const QString AverageScore    = QStringLiteral("media_average_score");
inline const QString SiteUrl         = QStringLiteral("media_site_url");
inline const QString TitleEnglish    = QStringLiteral("media_title_english");
inline const QString TitleNative     = QStringLiteral("media_title_native");
inline const QString TitleRomaji     = QStringLiteral("media_title_romaji");
inline const QString Synonyms        = QStringLiteral("media_synonyms");
inline const QString Format          = QStringLiteral("media_format");
inline const QString Description     = QStringLiteral("media_description");
inline const QString StartDate       = QStringLiteral("media_start_date");
inline const QString EndDate         = QStringLiteral("media_end_date");
inline const QString UpdatedAt       = QStringLiteral("media_updated_at");
inline const QString Episodes        = QStringLiteral("media_episodes");
inline const QString CountryOfOrigin = QStringLiteral("media_country_of_origin");
inline const QString CoverUrl        = QStringLiteral("media_cover_url");
inline const QString Genres          = QStringLiteral("media_genres");
inline const QString Studios         = QStringLiteral("media_studios");
inline const QString Producers       = QStringLiteral("media_producers");
} // namespace Media
} // namespace DatabaseColumns

