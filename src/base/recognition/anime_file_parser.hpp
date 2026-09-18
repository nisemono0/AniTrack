#pragma once

#include <QString>


namespace AnimeFileParser {
static constexpr int InvalidEpisode = -1;
struct AnimeFileInfo {
    QString title;
    QString release_group = QStringLiteral("Unknown");
    int episode = InvalidEpisode;
};

AnimeFileInfo parse(const QString &filename);
} // namespace AnimeFileParser
