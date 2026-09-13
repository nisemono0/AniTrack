#pragma once

#include "base/anilist/anilist_entry.hpp"
#include "base/anilist/anilist_media.hpp"

#include "base/recognition/anime_file_parser.hpp"

#include <optional>

#include <QString>


struct RecognizedAnime {
    int episode = AnimeFileParser::InvalidEpisode;
    AnilistMedia media;
    std::optional<AnilistEntry> entry = std::nullopt;
};
Q_DECLARE_METATYPE(RecognizedAnime);
