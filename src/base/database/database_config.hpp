#pragma once

#include <QString>


namespace DatabaseConfig {
inline const QString ConnectionName   = QStringLiteral("AniTrack_CONNECTION");
inline const QString DriverName       = QStringLiteral("QSQLITE");
inline const QString DatabaseFilename = QStringLiteral("anitrack.db");
} // namespace DatabaseConfig

