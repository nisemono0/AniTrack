#include "utils/sync.hpp"


AnilistEntry::PendingOperation SyncUtils::mergePendingOperation(
    AnilistEntry::PendingOperation current,
    AnilistEntry::PendingOperation requested
) {
    switch (current) {
        case AnilistEntry::PendingOperation::NONE:
            return requested;
        case AnilistEntry::PendingOperation::ADD: {
            switch (requested) {
                case AnilistEntry::PendingOperation::NONE:
                case AnilistEntry::PendingOperation::ADD:
                case AnilistEntry::PendingOperation::UPDATE:
                    return AnilistEntry::PendingOperation::ADD;
                case AnilistEntry::PendingOperation::REMOVE:
                    return AnilistEntry::PendingOperation::REMOVE;
            }
            break;
        }
        case AnilistEntry::PendingOperation::UPDATE: {
            switch (requested) {
                case AnilistEntry::PendingOperation::NONE:
                case AnilistEntry::PendingOperation::ADD:
                case AnilistEntry::PendingOperation::UPDATE:
                    return AnilistEntry::PendingOperation::UPDATE;
                case AnilistEntry::PendingOperation::REMOVE:
                    return AnilistEntry::PendingOperation::REMOVE;
            }
            break;
        }
        case AnilistEntry::PendingOperation::REMOVE:
            return AnilistEntry::PendingOperation::REMOVE;
    }

    return AnilistEntry::PendingOperation::NONE;
}

