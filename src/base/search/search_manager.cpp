#include "base/search/search_manager.hpp"


SearchManager::SearchManager(
    AnilistApi *anilist_api,
    QObject *parent
) : QObject(parent),
    anilist_api_(anilist_api) {

    connect(this->anilist_api_, &AnilistApi::searchAnimeFailed, this, &SearchManager::onSearchAnimeFailed);

    connect(this->anilist_api_, &AnilistApi::searchAnimeFinished, this, &SearchManager::onSearchAnimeFinished);
}

void SearchManager::requestAnimeSearch(const QString &title) {
    if (this->search_in_progress_) {
        return;
    }

    this->search_in_progress_ = true;
    this->search_type_ = SearchType::Normal;

    emit searchProgressStarted(
        QStringLiteral("Search"),
        QStringLiteral("Searching anime...")
    );

    this->anilist_api_->searchAnime(title);
}

void SearchManager::requestQuietAnimeSearch(const QString &title) {
    if (this->search_in_progress_) {
        return;
    }

    this->search_in_progress_ = true;
    this->search_type_ = SearchType::Quiet;

    this->anilist_api_->searchAnime(title);
}

void SearchManager::requestAnimeSearchById(const QList<int> &media_ids) {
    if (this->search_in_progress_) {
        return;
    }

    this->search_in_progress_ = true;
    this->search_type_ = SearchType::Id;

    this->anilist_api_->searchAnimeIds(media_ids);
}

void SearchManager::resetSearch() {
    this->search_in_progress_ = false;
    this->search_type_ = SearchType::None;
}

void SearchManager::onSearchAnimeFailed(const QString &message) {
    const SearchType search_type = this->search_type_;
    this->resetSearch();

    switch (search_type) {
        case SearchType::None:
            break;
        case SearchType::Normal: {
            emit searchProgressFinished();
            emit searchFailed(
                QStringLiteral("Search"),
                message
            );
            break;
        }
        case SearchType::Quiet: {
            emit quietSearchFailed(message);
            break;
        }
        case SearchType::Id: {
            emit idSearchFailed(message);
            break;
        }
    }
}

void SearchManager::onSearchAnimeFinished(const QList<AnilistMedia> &media_list) {
    const SearchType search_type = this->search_type_;
    this->resetSearch();

    switch (search_type) {
        case SearchType::None:
            break;
        case SearchType::Normal: {
            emit searchProgressFinished();
            emit searchFinished(media_list);
            break;
        }
        case SearchType::Quiet: {
            emit quietSearchFinished(media_list);
            break;
        }
        case SearchType::Id: {
            emit idSearchFinished(media_list);
            break;
        }
    }
}

