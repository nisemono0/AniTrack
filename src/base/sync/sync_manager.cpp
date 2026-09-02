#include "base/sync/sync_manager.hpp"


SyncManager::SyncManager(
    AnilistApi *anilist_api,
    Database *database,
    QObject *parent
) : QObject(parent),
    database_(database),
    anilist_api_(anilist_api) {

    connect(this->anilist_api_, &AnilistApi::fetchListFinished, this, &SyncManager::onFetchListFinished);

    connect(this->anilist_api_, &AnilistApi::addAnimeFinished, this, &SyncManager::onAddAnimeFinished);
    connect(this->anilist_api_, &AnilistApi::updateAnimeFinished, this, &SyncManager::onUpdateAnimeFinished);
    connect(this->anilist_api_, &AnilistApi::deleteAnimeFinished, this, &SyncManager::onDeleteAnimeFinished);

    connect(this->anilist_api_, &AnilistApi::fetchListFailed, this, &SyncManager::handleApiFailure);
    connect(this->anilist_api_, &AnilistApi::addAnimeFailed, this, &SyncManager::handleApiFailure);
    connect(this->anilist_api_, &AnilistApi::updateAnimeFailed, this, &SyncManager::handleApiFailure);
    connect(this->anilist_api_, &AnilistApi::deleteAnimeFailed, this, &SyncManager::handleApiFailure);
}

void SyncManager::requestSync() {
    this->startSync();
}

void SyncManager::startSync() {
    if (this->sync_in_progress_) {
        return;
    }

    this->sync_in_progress_ = true;
    this->is_final_fetch_ = false;

    this->current_anime_ = 0;
    this->pending_anime_.clear();

    emit listFetchStarted(
        QStringLiteral("Sync"),
        QStringLiteral("Fetching anime list...")
    );

    this->anilist_api_->fetchList();
}

void SyncManager::resetSync() {
    this->current_anime_ = 0;
    this->pending_anime_.clear();

    this->is_final_fetch_ = false;
    this->sync_in_progress_ = false;
}

void SyncManager::finishSync() {
    this->resetSync();
    emit listFetchFinished();
    emit syncProgressFinished();
    emit syncFinished();
}

void SyncManager::failSync(const QString &message) {
    this->resetSync();
    emit listFetchFinished();
    emit syncProgressFinished();
    emit syncFailed(
        QStringLiteral("Sync"),
        message
    );
}

void SyncManager::startPendingSync() {
    if (!this->sync_in_progress_) {
        return;
    }
    this->is_final_fetch_ = false;

    auto pending_anime = this->database_->selectAllPendingEntries();
    if (!pending_anime) {
        this->failSync(QStringLiteral("Failed to load local pending anime"));
        return;
    }

    if (pending_anime->isEmpty()) {
        this->finishSync();
        return;
    }

    this->current_anime_ = 0;
    this->pending_anime_ = std::move(pending_anime.value());

    emit syncProgressStarted(
        QStringLiteral("Sync"),
        QStringLiteral("Syncing..."),
        this->current_anime_,
        this->pending_anime_.size()
    );

    this->processNextEntry();
}

void SyncManager::processNextEntry() {
    if (this->current_anime_ >= this->pending_anime_.size()) {
        this->is_final_fetch_ = true;
        emit listFetchStarted(
            QStringLiteral("Sync"),
            QStringLiteral("Fetching anime list...")
        );
        this->anilist_api_->fetchList();
        return;
    }

    AnilistAnime anime = this->pending_anime_.at(this->current_anime_);

    emit syncProgressUpdated(this->current_anime_);
    this->current_anime_ += 1;

    switch (anime.entry.state().pending_operation) {
        // Nothing to do
        case AnilistEntry::PendingOperation::NONE: {
            this->processNextEntry();
            break;
        }
        // Add on anilist
        case AnilistEntry::PendingOperation::ADD: {
            this->anilist_api_->addAnime(anime);
            break;
        }
        // Update on anilist
        case AnilistEntry::PendingOperation::UPDATE: {
            this->anilist_api_->updateAnime(anime);
            break;
        }
        // Remove from anilist
        case AnilistEntry::PendingOperation::REMOVE: {
            this->anilist_api_->deleteAnime(anime);
            break;
        }
        default:
            break;
    }
}

void SyncManager::handleInitialFetch(const QList<AnilistAnime> &anime_list) {
    auto local_anime = this->database_->selectAllEntries();
    if (!local_anime) {
        this->failSync(QStringLiteral("Failed to load local anime"));
        return;
    }

    // No local anime, just upsert the ones from anilist
    if (local_anime->isEmpty()) {
        if (!this->database_->upsertAnime(anime_list)) {
            this->failSync(QStringLiteral("Failed to import Anilist anime"));
            return;
        }

        this->finishSync();
        return;
    }

    // Always upsert medias, since they could've been updated
    // NOTE: If i keep the final fetch this might not be needed at all
    //       since the final fetch does upsertAnime (both media and entry)
    if (!this->database_->upsertMedias(anime_list)) {
        this->failSync(QStringLiteral("Failed to update Anilist medias"));
        return;
    }

    // anilist media_id -> index in anime
    QHash<int, int> anime_idx_by_media_id;
    anime_idx_by_media_id.reserve(anime_list.size());
    for (int i = 0; i < anime_list.size(); i++) {
        anime_idx_by_media_id.insert(
            anime_list.at(i).media.id,
            i
        );
    }

    QList<AnilistAnime> upserts;
    QList<int> deletions;

    for (const auto &local : local_anime.value()) {
        const auto entry_idx = anime_idx_by_media_id.constFind(
            local.media.id
        );

        // Local anime has no matching anilist entry
        if (entry_idx == anime_idx_by_media_id.constEnd()) {
            this->resolveMissingAnime(local, upserts, deletions);
            continue;
        }

        // Local anime has an existing anilist entry
        const auto &remote = anime_list.at(entry_idx.value());
        this->resolveExistingAnime(local, remote, upserts);
    }

    // Apply the above resolves in the database
    if (!this->database_->upsertEntries(upserts)) {
        this->failSync(QStringLiteral("Failed to resolve Anilist<->Local updates"));
        return;
    }

    if (!this->database_->deleteEntries(deletions)) {
        this->failSync(QStringLiteral("Failed to resolve Anilist<->Local deletions"));
        return;
    }

    emit listFetchFinished();

    this->startPendingSync();
}

void SyncManager::handleFinalFetch(const QList<AnilistAnime> &anime_list) {
    if (!this->database_->upsertAnime(anime_list)) {
        this->failSync(QStringLiteral("Failed to update local anime"));
        return;
    }
    this->finishSync();
}

void SyncManager::resolveMissingAnime(const AnilistAnime &local, QList<AnilistAnime> &upserts, QList<int> &deletions) {
    switch (local.entry.state().pending_operation) {
        case AnilistEntry::PendingOperation::NONE: {
            // Local has no pending changes and remote is deleted
            deletions.append(local.entry.localId());
            break;
        }
        case AnilistEntry::PendingOperation::ADD: {
            // Anime is local only, do nothing
            break;
        }
        case AnilistEntry::PendingOperation::UPDATE: {
            // Anilist anime was deleted while local has pending changes
            AnilistAnime updated_anime = local;
            updated_anime.entry.setId(AnilistEntry::InvalidId);
            auto updated_state = updated_anime.entry.state();
            updated_state.pending_operation = AnilistEntry::PendingOperation::ADD;
            updated_anime.entry.addState(updated_state);
            upserts.append(std::move(updated_anime));
            break;
        }
        case AnilistEntry::PendingOperation::REMOVE: {
            // Anilist anime already gone
            deletions.append(local.entry.localId());
            break;
        }
        default:
            break;
    }
}

void SyncManager::resolveExistingAnime(const AnilistAnime &local, const AnilistAnime &remote, QList<AnilistAnime> &upserts) {
    switch (local.entry.state().pending_operation) {
        case AnilistEntry::PendingOperation::NONE: {
            // Update local if anilist anime is newer
            if (local.entry.state().updated_at < remote.entry.state().updated_at) {
                AnilistAnime updated_anime = remote;
                updated_anime.entry.setLocalId(local.entry.localId());
                upserts.append(std::move(updated_anime));
            }
            break;
        }
        // New local anime but anilist anime already exists
        case AnilistEntry::PendingOperation::ADD: {
            AnilistAnime added_anime;

            // Local newer than anilist, local becomes update
            if (local.entry.state().updated_at > remote.entry.state().updated_at) {
                added_anime = local;
                added_anime.entry.setId(remote.entry.id());
                auto state = added_anime.entry.state();
                state.pending_operation = AnilistEntry::PendingOperation::UPDATE;
                added_anime.entry.addState(state);
            } else {
                // Anilist newer or the same, remote wins
                added_anime = remote;
                added_anime.entry.setLocalId(local.entry.localId());
                auto state = added_anime.entry.state();
                state.pending_operation = AnilistEntry::PendingOperation::NONE;
                added_anime.entry.addState(state);
            }

            upserts.append(std::move(added_anime));
            break;
        }
        // Anilist entry exists and local has pending changes
        case AnilistEntry::PendingOperation::UPDATE: {
            AnilistAnime updated_anime;

            // Similar to ADD, either update local from remote or remote from local
            if (local.entry.state().updated_at > remote.entry.state().updated_at) {
                updated_anime = local;
                updated_anime.entry.setId(remote.entry.id());
            } else {
                // Remote defaults to PendingOperation::NONE
                updated_anime = remote;
                updated_anime.entry.setLocalId(local.entry.localId());
                auto state = updated_anime.entry.state();
                state.pending_operation = AnilistEntry::PendingOperation::NONE;
                updated_anime.entry.addState(state);
            }

            upserts.append(std::move(updated_anime));
            break;
        }
        // Anilist entry exists, do nothing
        case AnilistEntry::PendingOperation::REMOVE: {
            break;
        }
        default:
            break;
    }
}

void SyncManager::handleApiFailure(const QString &message) {
    this->failSync(message);
}

void SyncManager::onFetchListFinished(const QList<AnilistAnime> &anime_list) {
    if (this->is_final_fetch_) {
        // NOTE: The final fetch is probably not needed
        //       It's here just to be safe
        this->handleFinalFetch(anime_list);
    } else {
        this->handleInitialFetch(anime_list);
    }
}

void SyncManager::onAddAnimeFinished(const AnilistAnime &anime) {
    if (!this->database_->upsertEntry(anime)) {
        this->failSync(QStringLiteral("Failed to add anime"));
        return;
    }
    this->processNextEntry();
}

void SyncManager::onUpdateAnimeFinished(const AnilistAnime &anime) {
    if (!this->database_->upsertEntry(anime)) {
        this->failSync(QStringLiteral("Failed to update anime"));
        return;
    }
    this->processNextEntry();
}

void SyncManager::onDeleteAnimeFinished(int local_id) {
    if (!this->database_->deleteEntry(local_id)) {
        this->failSync(QStringLiteral("Failed to remove anime"));
        return;
    }
    this->processNextEntry();
}

