#include "gui/pages/anime_list_page.hpp"


AnimeListPage::AnimeListPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::AnimeListWidget) {

    this->initPage();

    this->setupTabBar();
    this->setupPage();
    this->setupInfoEditDialog();
}

AnimeListPage::~AnimeListPage() {
    delete this->ui_;
}

void AnimeListPage::selectNextTab() {
    this->tab_bar_->selectNextTab();
}

void AnimeListPage::selectPreviousTab() {
    this->tab_bar_->selectPreviousTab();
}

void AnimeListPage::onUserUpdated(const AnilistAccount::User &user) {
    this->ui_->treeViewAnimeList->updateUserPreferences(user);
    this->info_edit_dialog_->updateUserPreferences(user);
}

void AnimeListPage::onFilterTextChanged(const QString &text) {
    this->ui_->treeViewAnimeList->setFilterText(text);
}

void AnimeListPage::onAnimeLoadFinished(const QList<AnilistAnime> &anime_list) {
    this->ui_->treeViewAnimeList->setAnime(anime_list);
}

void AnimeListPage::onAnimeUpdateFinished(const QList<AnilistAnime> &anime_list) {
    this->ui_->treeViewAnimeList->updateAnime(anime_list);
}

void AnimeListPage::onAnimeDeleteFinished(const QList<int> &local_ids) {
    this->ui_->treeViewAnimeList->deleteAnime(local_ids);
}

void AnimeListPage::onAnimeAddFinished(const QList<AnilistAnime> &anime_list) {
    this->ui_->treeViewAnimeList->addAnime(anime_list);
}

void AnimeListPage::onRequestShowAnimeInfoEditDialog(const AnilistAnime &anime, AnimeInfoEditDialog::Page page) {
    this->info_edit_dialog_->setAnime(anime);
    this->info_edit_dialog_->showOrFocus(page);
}

void AnimeListPage::initPage() {
    this->ui_->setupUi(this);
    this->tab_bar_ = new AnimeListTabBar(this);
    this->info_edit_dialog_ = new AnimeInfoEditDialog(this);
}

void AnimeListPage::setupTabBar() {
    connect(this->tab_bar_, &AnimeListTabBar::currentTabChanged, this, &AnimeListPage::onCurrentTabChanged);

    this->ui_->verticalLayout->insertWidget(0, this->tab_bar_);
    this->tab_bar_->setCurrentTab(AnimeListTabBar::Tab::Watching);
}

void AnimeListPage::setupPage() {
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestIncreaseAnimeProgress, this, &AnimeListPage::requestIncreaseAnimeProgress);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestDecreaseAnimeProgress, this, &AnimeListPage::requestDecreaseAnimeProgress);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeProgress, this, &AnimeListPage::requestSetAnimeProgress);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestUndoAnimeState, this, &AnimeListPage::requestUndoAnimeState);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestRedoAnimeState, this, &AnimeListPage::requestRedoAnimeState);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestMarkAnimeForRemoval, this, &AnimeListPage::requestMarkAnimeForRemoval);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestClearAnimeStartedDate, this, &AnimeListPage::requestClearAnimeStartedDate);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeStartedDateFromMediaStartDate, this, &AnimeListPage::requestSetAnimeStartedDateFromMediaStartDate);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeStartedDate, this, &AnimeListPage::requestSetAnimeStartedDate);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestClearAnimeCompletedDate, this, &AnimeListPage::requestClearAnimeCompletedDate);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeCompletedDateFromLastUpdated, this, &AnimeListPage::requestSetAnimeCompletedDateFromLastUpdated);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeCompletedDateFromMediaEndDate, this, &AnimeListPage::requestSetAnimeCompletedDateFromMediaEndDate);
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeCompletedDate, this, &AnimeListPage::requestSetAnimeCompletedDate);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeStatus, this, &AnimeListPage::requestSetAnimeStatus);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeProgress, this, &AnimeListPage::requestSetAnimeEpisode);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestSetAnimeScore, this, &AnimeListPage::requestSetAnimeScore);

    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestRestoreAnimeState, this, &AnimeListPage::requestRestoreAnimeState);
}

void AnimeListPage::setupInfoEditDialog() {
    connect(this->ui_->treeViewAnimeList, &AnimeListView::requestShowAnimeInfoEditDialog, this, &AnimeListPage::onRequestShowAnimeInfoEditDialog);
    connect(this->info_edit_dialog_, &AnimeInfoEditDialog::requestUpdateAnime, this, &AnimeListPage::requestUpdateAnime);
}

void AnimeListPage::onCurrentTabChanged(AnimeListTabBar::Tab tab) {
    this->ui_->treeViewAnimeList->setFilterTab(tab);
}
