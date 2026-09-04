#pragma once

#include <QString>
#include <QSet>


namespace MprisConfig {
inline const QString Service                 = QStringLiteral("org.mpris.MediaPlayer2*");
inline const QString ObjectPath              = QStringLiteral("/org/mpris/MediaPlayer2");
inline const QString PropertiesInterface     = QStringLiteral("org.freedesktop.DBus.Properties");
inline const QString PropertiesChangedSignal = QStringLiteral("PropertiesChanged");
inline const QString PropertiesGet           = QStringLiteral("Get");
inline const QString PlayerInterface         = QStringLiteral("org.mpris.MediaPlayer2.Player");

inline const QSet<QString> AllowedServices{
    QStringLiteral("org.mpris.MediaPlayer2.mpv"),
    QStringLiteral("org.mpris.MediaPlayer2.vlc"),
    QStringLiteral("org.mpris.MediaPlayer2.haruna")
};
} // namespace MprisConfig

