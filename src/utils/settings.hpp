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

// Signal that settings have changed
void signalChanges();

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

// Sync settings keys
namespace Settings::Sync {
inline const QString UpdateUseInfoOnStart   = QStringLiteral("Sync/UpdateUseInfoOnStart");
inline const QString UpdateAnimeInfoOnStart = QStringLiteral("Sync/UpdateAnimeInfoOnStart");
inline const QString SyncAnimeOnStart       = QStringLiteral("Sync/SyncAnimeOnStart");
} // namespace Settings::Sync

// Recognition settings keys
namespace Settings::Recognition {
inline const QString EnableAnimeRecognition           = QStringLiteral("Recognition/EnableAnimeRecognition");
inline const QString EnableRecognitionPopup           = QStringLiteral("Recognition/EnableRecognitionPopup");
inline const QString MaxCacheMatches                  = QStringLiteral("Recognition/MaxCacheMatches");
inline const QString MinMatchScore                    = QStringLiteral("Recognition/MinMatchScore");
inline const QString RecognitionPopupDelay            = QStringLiteral("Recognition/RecognitionPopupDelay");
inline const QString RecognitionSuccessGotoNowPlaying = QStringLiteral("Recognition/RecognitionSuccessGotoNowPlaying");
inline const QString RecognitionSuccessMessageDialog  = QStringLiteral("Recognition/RecognitionSuccessMessageDialog");
inline const QString RecognitionFailGotoNowPlaying    = QStringLiteral("Recognition/RecognitionFailGotoNowPlaying");
inline const QString RecognitionFailMessageDialog     = QStringLiteral("Recognition/RecognitionFailMessageDialog");
} // namespace Settings::Recognition

// Ui settings keys
namespace Settings::Ui {
namespace Window {
inline const QString StartMinimized            = QStringLiteral("Window/StartMinimized");
inline const QString SaveLoadWindowState       = QStringLiteral("Window/SaveLoadWindowState");
inline const QString SaveLoadAnimeHeaderState  = QStringLiteral("Window/SaveLoadAnimeHeaderState");
inline const QString SaveLoadSearchHeaderState = QStringLiteral("Window/SaveLoadSearchHeaderState");
inline const QString State                     = QStringLiteral("Window/WindowState");
namespace AnimeList {
inline const QString HeaderState = QStringLiteral("Window/AnimeList/HeaderState");
}
namespace AnimeSearch {
inline const QString HeaderState = QStringLiteral("Window/AnimeSearch/HeaderState");
}
} // namespace Window
} // namespace Settings::Ui

