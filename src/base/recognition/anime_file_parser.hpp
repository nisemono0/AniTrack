#pragma once

#include <QString>


namespace AnimeFileParser {
static constexpr int InvalidEpisode = -1;
struct ParsedAnimeFile {
    QString title;
    int episode = InvalidEpisode;
};

ParsedAnimeFile parse(const QString &filename);
} // namespace AnimeFileParser
