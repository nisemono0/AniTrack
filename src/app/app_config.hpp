#pragma once

#include <QString>

namespace AppConfig {
inline const QString Name            = QStringLiteral("AniTrack");
inline const QString OrgName         = QStringLiteral("AniTrack");
inline const QString Description     = QStringLiteral("Application to track watched anime.");
inline const QString Version         = QStringLiteral(ANITRACK_VERSION);

inline const QString InstanceName    = QStringLiteral("AniTrack.Single.Instance");
inline constexpr int InstanceTimeout = 200;

inline constexpr int CursorFlashTime = 0;
}; // namespace AppConfig

