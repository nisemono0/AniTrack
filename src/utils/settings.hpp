#pragma once

#include "app/app.hpp"

#include <QString>


// Wrappers around App::instance()->settings()
namespace Settings {
// Template that returns a type T setting or default_value if key does not exist
// If the type T is an enum, it will static_cast the value to said enum type
template<typename T>
inline T get(const QString &key, const T &default_value) {
    return App::instance()->settings()->get(key, default_value);
}

// Template that sets key setting to T type value
// Value is set using QVariant::fromValue()
template<typename T>
inline void set(const QString &key, const T &value) {
    App::instance()->settings()->set(key, value);
}

// Save stored settings to disk
// Save enum types as their underlying int value
void syncToDisk();

} // namespace Settings


// Anilist settings keys
namespace Settings::Anilist {
namespace User {
inline const QString AuthToken = QStringLiteral("AnilistUser/AuthToken");
inline const QString Name      = QStringLiteral("AnilistUser/Name");
inline const QString Id        = QStringLiteral("AnilistUser/Id");
inline const QString AvatarUrl = QStringLiteral("AnilistUser/AvatarUrl");
} // namespace User

namespace UserSettings {
inline const QString ScoreFormat   = QStringLiteral("AnilistUserSettings/ScoreFormat");
inline const QString TitleLanguage = QStringLiteral("AnilistUserSettings/TitleLanguage");
} // namespace UserSettings

namespace UserStats {
inline const QString AnimeCount        = QStringLiteral("AnilistUserStats/AnimeCount");
inline const QString EpisodesWatched   = QStringLiteral("AnilistUserStats/EpisodesWatched");
inline const QString MeanScore         = QStringLiteral("AnilistUserStats/MeanScore");
inline const QString MinutesWatched    = QStringLiteral("AnilistUserStats/MinutesWatched");
inline const QString StandardDeviation = QStringLiteral("AnilistUserStats/StandardDeviation");
} // namespace UserStats
} // namespace Settings::Anilist


namespace Settings::Ui {
namespace AnimeListView {
inline const QString HeaderState = QStringLiteral("AnimeListView/HeaderState");
}
namespace AnimeSearchView {
inline const QString HeaderState = QStringLiteral("AnimeSearchView/HeaderState");
}
} // namespace Settings::Ui
