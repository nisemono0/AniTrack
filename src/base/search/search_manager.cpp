#include "base/search/search_manager.hpp"


SearchManager::SearchManager(
    AnilistApi *anilist_api,
    QObject *parent
) : QObject(parent),
    anilist_api_(anilist_api) {

    connect(this->anilist_api_, &AnilistApi::searchAnimeFinished, this, &SearchManager::onSearchAnimeFinished);
    connect(this->anilist_api_, &AnilistApi::searchAnimeFailed, this, &SearchManager::onSearchAnimeFailed);
}

void SearchManager::requestAnimeSearch(const QString &title) {
    this->startSearch(title);
}

void SearchManager::startSearch(const QString &title) {
    if (this->search_in_progress_) {
        return;
    }

    this->search_in_progress_ = true;

    emit searchProgressStarted(
        QStringLiteral("Search"),
        QStringLiteral("Searching anime...")
    );

    this->anilist_api_->searchAnime(title);
}

void SearchManager::onSearchAnimeFailed(const QString &message) {
    this->search_in_progress_ = false;
    emit searchProgressFinished();
    emit searchFailed(
        QStringLiteral("Search"),
        message
    );
}

void SearchManager::onSearchAnimeFinished(const QList<AnilistMedia> &media) {
    this->search_in_progress_ = false;
    emit searchProgressFinished();
    emit searchFinished(media);
}

