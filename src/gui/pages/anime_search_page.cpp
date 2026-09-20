#include "gui/pages/anime_search_page.hpp"


AnimeSearchPage::AnimeSearchPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::AnimeSearchWidget) {

    this->initPage();
    this->setupPage();
}

AnimeSearchPage::~AnimeSearchPage() {
    delete this->ui_;
}

void AnimeSearchPage::focusSearch() {
    this->ui_->treeViewAnimeSearch->setFocus();
}

void AnimeSearchPage::onSearchFinished(const QList<AnilistMedia> &media_list) {
    this->ui_->treeViewAnimeSearch->setMedia(media_list);
}

void AnimeSearchPage::onUserUpdated(const AnilistAccount::User &user) {
    this->ui_->treeViewAnimeSearch->updateUserPreferences(user);
}

void AnimeSearchPage::onFilterTextChanged(const QString &text) {
    this->ui_->treeViewAnimeSearch->setFilterText(text);
}

void AnimeSearchPage::onAnimeLoadFinished(const QList<AnilistAnime> &anime_list) {
    this->ui_->treeViewAnimeSearch->setExistingMediaIds(anime_list);
}

void AnimeSearchPage::onAnimeAddFinished(const QList<AnilistAnime> &anime_list) {
    this->ui_->treeViewAnimeSearch->updateAddedAnime(anime_list);
}

void AnimeSearchPage::initPage() {
    this->ui_->setupUi(this);
}

void AnimeSearchPage::setupPage() {
    connect(this->ui_->treeViewAnimeSearch, &AnimeSearchView::requestAddMedia, this, &AnimeSearchPage::requestAddMedia);
    connect(this->ui_->treeViewAnimeSearch, &AnimeSearchView::requestShowAnimeInfoEditDialog, this, &AnimeSearchPage::requestShowAnimeInfoEditDialog);
}

