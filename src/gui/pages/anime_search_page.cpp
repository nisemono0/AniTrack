#include "gui/pages/anime_search_page.hpp"


AnimeSearchPage::AnimeSearchPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::AnimeSearchWidget) {

    this->initPage();
    this->setupPage();
    this->setupInfoDialog();
}

AnimeSearchPage::~AnimeSearchPage() {
    delete this->ui_;
}

void AnimeSearchPage::onSearchFinished(const QList<AnilistMedia> &media_list) {
    this->ui_->treeViewAnimeSearch->setMedia(media_list);
}

void AnimeSearchPage::onUserUpdated(const AnilistAccount::User &user) {
    this->ui_->treeViewAnimeSearch->updateUserPreferences(user);
    this->info_search_dialog_->updateUserPreferences(user);
}

void AnimeSearchPage::onFilterTextChanged(const QString &text) {
    this->ui_->treeViewAnimeSearch->setFilterText(text);
}

void AnimeSearchPage::onAnimeLoadFinished(const QList<AnilistAnime> &anime_list) {
    this->ui_->treeViewAnimeSearch->setExistingMediaIds(anime_list);
}

void AnimeSearchPage::onMediaAddFinished(const QList<AnilistMedia> &media_list) {
    this->ui_->treeViewAnimeSearch->updateAddedMedia(media_list);
}

void AnimeSearchPage::onRequestShowAnimeInfoSearchDialog(const AnilistMedia &media) {
    this->info_search_dialog_->setMedia(media);
    this->info_search_dialog_->showOrFocus();
}

void AnimeSearchPage::initPage() {
    this->ui_->setupUi(this);
    this->info_search_dialog_ = new AnimeInfoSearchDialog(this);
}

void AnimeSearchPage::setupPage() {
    connect(this->ui_->treeViewAnimeSearch, &AnimeSearchView::requestAddMedia, this, &AnimeSearchPage::requestAddMedia);
}

void AnimeSearchPage::setupInfoDialog() {
    connect(this->ui_->treeViewAnimeSearch, &AnimeSearchView::requestShowAnimeInfoSearchDialog, this, &AnimeSearchPage::onRequestShowAnimeInfoSearchDialog);
    connect(this->info_search_dialog_, &AnimeInfoSearchDialog::requestAddMedia, this, &AnimeSearchPage::requestAddMedia);
}

