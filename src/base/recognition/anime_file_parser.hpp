#pragma once

#include <QString>


namespace AnimeFileParser {
static constexpr int InvalidEpisode = -1;
struct AnimeFileInfo {
    QString title;
    int episode = InvalidEpisode;
};

AnimeFileInfo parse(const QString &filename);
} // namespace AnimeFileParser
