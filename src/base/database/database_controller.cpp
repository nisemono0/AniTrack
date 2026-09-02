#include "base/database/database_controller.hpp"

#include "utils/sync.hpp"
#include "utils/date.hpp"
#include "utils/anilist.hpp"


DatabaseController::DatabaseController(
    Database *database,
    QObject *parent
) : QObject(parent),
    database_(database) {}


void DatabaseController::initialLoad() {
    auto entries = this->database_->selectAllEntries();

    if (!entries) {
        emit errorOccurred(
            QStringLiteral("Anime load"),
            entries.error()
        );
        return;
    }

    emit databasePathChanged(this->database_->databasePath());
    emit animeLoadFinished(entries.value());

    this->countEntriesAndNotifiy();
}

void DatabaseController::requestLoadAnime() {
    auto entries = this->database_->selectAllEntries();

    if (!entries) {
        emit errorOccurred(
            QStringLiteral("Anime load"),
            entries.error()
        );
        return;
    }

    emit animeLoadFinished(entries.value());

    this->countEntriesAndNotifiy();
}

void DatabaseController::requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status) {
    QList<AnilistAnime> added_anime;

    for (const auto &media : media_list) {
        if (media.in_list) {
            continue;
        }

        AnilistAnime new_anime;
        new_anime.media = media;

        auto new_state = new_anime.entry.state();
        new_state.pending_operation = AnilistEntry::PendingOperation::ADD;
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.status = status;
        new_state.name = QStringLiteral("Added anime");

        switch (status) {
            case AnilistEntry::Status::CURRENT: {
                new_state.started_at = DateUtils::currentDate();
                break;
            }
            case AnilistEntry::Status::COMPLETED: {
                new_state.completed_at = DateUtils::currentDate();
                break;
            }
            default:
                break;
        }

        new_anime.entry.addState(new_state);

        added_anime.append(std::move(new_anime));
    }

    if (!this->database_->insertAnime(added_anime)) {
        emit errorOccurred(
            QStringLiteral("Add anime"),
            QStringLiteral("Failed to add anime")
        );
        return;
    }

    emit animeAddFinished(added_anime);
    emit mediaAddFinished(media_list);

    this->countEntriesAndNotifiy();
}

void DatabaseController::requestUpdateAnime(const AnilistAnime &anime) {
    if (!this->database_->upsertEntry(anime)) {
        emit errorOccurred(
            QStringLiteral("Anime update"),
            QStringLiteral("Failed to update anime")
        );
        return;
    }

    emit animeUpdateFinished({anime});
}

void DatabaseController::requestIncreaseAnimeProgress(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        const int new_progress = anime.entry.state().progress + 1;
        if (anime.media.episodes > 0 && new_progress > anime.media.episodes) {
            continue;
        }

        auto new_state = anime.entry.state();
        new_state.progress = new_progress;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        if (new_progress == anime.media.episodes && anime.media.episodes != 0) {
            new_state.completed_at = DateUtils::currentDate();
            new_state.status = AnilistEntry::Status::COMPLETED;
        }
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Increase progress: %1").arg(new_progress);

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Increase episode"),
        QStringLiteral("Failed to increase anime progress")
    );
}

void DatabaseController::requestDecreaseAnimeProgress(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        const int new_progress = anime.entry.state().progress - 1;
        if (new_progress < 0) {
            continue;
        }

        auto new_state = anime.entry.state();
        new_state.progress = new_progress;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        if (new_progress < anime.media.episodes && anime.media.episodes != 0) {
            new_state.completed_at = QDate();
        }
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Decrease progress: %1").arg(new_progress);

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Decrease episode"),
        QStringLiteral("Failed to decrease anime progress")
    );
}

void DatabaseController::requestSetAnimeProgress(const QList<AnilistAnime> &anime_list, int progress) {
    if (progress < 0) {
        return;
    }

    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        auto new_state = anime.entry.state();

        int new_progress;

        if (anime.media.episodes > 0) {
            new_progress = qMin(progress, anime.media.episodes);
        } else {
            new_progress = progress;
        }

        if (new_progress == new_state.progress) {
            continue;
        }

        new_state.progress = new_progress;

        if (new_progress == anime.media.episodes && anime.media.episodes != 0 ) {
            new_state.status = AnilistEntry::Status::COMPLETED;
            new_state.completed_at = DateUtils::currentDate();
        } else if (new_progress < anime.media.episodes && anime.media.episodes != 0) {
            new_state.completed_at = QDate();
        }

        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );

        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set progress: %1").arg(new_progress);

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set progress"),
        QStringLiteral("Failed to set anime progress")
    );
}

void DatabaseController::requestUndoAnimeState(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (!anime.entry.canUndoState()) {
            continue;
        }
        anime.entry.undoState();

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Undo"),
        QStringLiteral("Failed to undo anime state")
    );
}

void DatabaseController::requestRedoAnimeState(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (!anime.entry.canRedoState()) {
            continue;
        }
        anime.entry.redoState();

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Redo"),
        QStringLiteral("Failed to redo anime state")
    );
}

void DatabaseController::requestMarkAnimeForRemoval(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        auto new_state = anime.entry.state();
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::REMOVE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Mark for removal");

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Mark for removal"),
        QStringLiteral("Failed to mark anime for removal")
    );
}

void DatabaseController::requestClearAnimeStartedDate(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        auto new_state = anime.entry.state();
        new_state.started_at = QDate();
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set started date: -");

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Clear started date"),
        QStringLiteral("Failed to clear started date")
    );
}

void DatabaseController::requestSetAnimeStartedDateFromMediaStartDate(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (!anime.media.start_date.isValid()) {
            continue;
        }
        auto new_state = anime.entry.state();
        new_state.started_at = anime.media.start_date;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set started date: %1").arg(
            AnilistUtils::dateToPrettyString(new_state.started_at)
        );

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set started date"),
        QStringLiteral("Failed to set started date to anime started airing date")
    );
}

void DatabaseController::requestSetAnimeStartedDate(const QList<AnilistAnime> &anime_list, const QDate &date) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (anime.entry.state().started_at == date) {
            continue;
        }

        auto new_state = anime.entry.state();
        new_state.started_at = date;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set started date: %1").arg(
            AnilistUtils::dateToPrettyString(new_state.started_at)
        );

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set started date"),
        QStringLiteral("Failed to set started date")
    );
}

void DatabaseController::requestClearAnimeCompletedDate(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        auto new_state = anime.entry.state();
        new_state.completed_at = QDate();
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set completed date: -");

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Clear completed date"),
        QStringLiteral("Failed to clear completed date")
    );
}

void DatabaseController::requestSetAnimeCompletedDateFromLastUpdated(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (anime.entry.state().updated_at <= 0) {
            continue;
        }
        auto new_state = anime.entry.state();
        new_state.completed_at = DateUtils::dateFromEpoch(new_state.updated_at);
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set completed date: %1").arg(
            AnilistUtils::dateToPrettyString(new_state.completed_at)
        );

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set completed date"),
        QStringLiteral("Failed to set completed date to last updated")
    );
}

void DatabaseController::requestSetAnimeCompletedDateFromMediaEndDate(const QList<AnilistAnime> &anime_list) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (!anime.media.end_date.isValid()) {
            continue;
        }
        auto new_state = anime.entry.state();
        new_state.completed_at = anime.media.end_date;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set completed date: %1").arg(
            AnilistUtils::dateToPrettyString(new_state.completed_at)
        );

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set completed date"),
        QStringLiteral("Failed to set completed date to anime finished airing date")
    );
}

void DatabaseController::requestSetAnimeCompletedDate(const QList<AnilistAnime> &anime_list, const QDate &date) {
    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (anime.entry.state().completed_at == date) {
            continue;
        }

        auto new_state = anime.entry.state();
        new_state.completed_at = date;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set completed date: %1").arg(
            AnilistUtils::dateToPrettyString(new_state.completed_at)
        );

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set completed date"),
        QStringLiteral("Failed to set completed date")
    );
}

void DatabaseController::requestSetAnimeScore(const QList<AnilistAnime> &anime_list, int score) {
    if (score < 0) {
        return;
    }

    QList<AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        auto new_state = anime.entry.state();

        if (score == new_state.score) {
            continue;
        }

        new_state.score = score;
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set anime score: %1").arg(score);

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set anime score"),
        QStringLiteral("Failed to set score")
    );
}

void DatabaseController::requestSetAnimeStatus(const QList<AnilistAnime> &anime_list, AnilistEntry::Status status) {
    QList <AnilistAnime> updated_anime;

    for (auto anime : anime_list) {
        if (anime.entry.state().status == status) {
            continue;
        }

        auto new_state = anime.entry.state();
        new_state.pending_operation = SyncUtils::mergePendingOperation(
            new_state.pending_operation,
            AnilistEntry::PendingOperation::UPDATE
        );
        new_state.status = status;
        switch (status) {
            case AnilistEntry::Status::CURRENT:
            case AnilistEntry::Status::PLANNING:
            case AnilistEntry::Status::DROPPED:
            case AnilistEntry::Status::PAUSED: {
                new_state.completed_at = QDate();
                break;
            }
            case AnilistEntry::Status::COMPLETED: {
                new_state.completed_at = DateUtils::currentDate();
                break;
            }
            case AnilistEntry::Status::REPEATING:
                break;
            default:
                break;
        }
        new_state.updated_at = DateUtils::currentEpochTime();
        new_state.name = QStringLiteral("Set anime status: %1").arg(
            AnilistUtils::entryStatusToPrettyString(status)
        );

        anime.entry.addState(new_state);

        updated_anime.append(std::move(anime));
    }

    this->upsertEntriesAndNotify(
        updated_anime,
        QStringLiteral("Set anime status"),
        QStringLiteral("Failed to set anime status")
    );
}

void DatabaseController::requestRestoreAnimeState(const AnilistAnime &anime, int index) {
    auto updated_anime = anime;

    if (!updated_anime.entry.setCurrentState(index)) {
        emit errorOccurred(
            QStringLiteral("Anime state"),
            QStringLiteral("Failed to set anime state index")
        );
        return;
    }

    this->upsertEntriesAndNotify(
        {updated_anime},
        QStringLiteral("Anime state"),
        QStringLiteral("Failed to set anime state")
    );
}

void DatabaseController::requestCleanupUnusedMedia() {
    if (!this->database_->cleanupUnusedMedia()) {
        emit errorOccurred(
            QStringLiteral("Media cleanup"),
            QStringLiteral("Failed to cleanup unused media from the database")
        );
        return;
    }

    emit infoOccurred(
        QStringLiteral("Media cleanup"),
        QStringLiteral("Cleaned up unused media")
    );
}

void DatabaseController::requestVacuumDatabase() {
    if (!this->database_->vacuum()) {
        emit errorOccurred(
            QStringLiteral("Database vacuum"),
            QStringLiteral("Failed to vacuum the database")
        );
        return;
    }

    emit infoOccurred(
        QStringLiteral("Database vacuum"),
        QStringLiteral("Database vacuumed")
    );
}

void DatabaseController::requestRecreateDatabase() {
    if (!this->database_->recreateDatabase()) {
        emit errorOccurred(
            QStringLiteral("Database recreate"),
            QStringLiteral("Failed to recreate the database")
        );
        return;
    }

    auto entries = this->database_->selectAllEntries();

    if (!entries) {
        emit errorOccurred(
            QStringLiteral("Database recreate"),
            entries.error()
        );
        return;
    }

    emit databasePathChanged(this->database_->databasePath());
    emit animeLoadFinished(entries.value());

    this->countEntriesAndNotifiy();

    emit infoOccurred(
        QStringLiteral("Database recreate"),
        QStringLiteral("Database recreated")
    );

    this->countEntriesAndNotifiy();
}

void DatabaseController::upsertEntriesAndNotify(const QList<AnilistAnime> &anime_list, const QString &title, const QString &message) {
    if (anime_list.isEmpty()) {
        return;
    }

    if (!this->database_->upsertEntries(anime_list)) {
        emit errorOccurred(title, message);
        return;
    }

    emit animeUpdateFinished(anime_list);
}

void DatabaseController::countEntriesAndNotifiy() {
    auto entries_count = this->database_->entriesCount();

    if (!entries_count) {
        emit errorOccurred(
            QStringLiteral("Entry count"),
            entries_count.error()
        );
        return;
    }

    emit animeCountChanged(entries_count.value());
}

