#pragma once

#include "base/anilist/anilist_entry.hpp"


namespace SyncUtils {
AnilistEntry::PendingOperation mergePendingOperation(
    AnilistEntry::PendingOperation current,
    AnilistEntry::PendingOperation requested
);
} // namespace SyncUtils
