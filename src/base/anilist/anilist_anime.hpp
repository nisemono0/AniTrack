#pragma once


#include "base/anilist/anilist_entry.hpp"
#include "base/anilist/anilist_media.hpp"


struct AnilistAnime {
    AnilistEntry entry;
    AnilistMedia media;
};
Q_DECLARE_METATYPE(AnilistAnime);
