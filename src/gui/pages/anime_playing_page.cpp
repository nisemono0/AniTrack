#include "gui/pages/anime_playing_page.hpp"


AnimePlayingPage::AnimePlayingPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::AnimePlayingWidget) {

    this->setupPage();

    this->setupSelectAnimePage();
    this->setupNowPlayingPage();
    this->setupSearchPage();
}

AnimePlayingPage::~AnimePlayingPage() {
    delete this->ui_;
}

void AnimePlayingPage::onUserUpdated(const AnilistAccount::User &user) {
    this->ui_->pageRecognitionNowPlaying->updateUserPreferences(user);

    // TODO: set new user preferences where needed
    //       prolly to now playing and select anime
}

void AnimePlayingPage::handleAnimeAddUpdateFinished(const QList<AnilistAnime> &anime_list) {
    auto *current_page = this->ui_->stackedWidgetAnimePlaying->currentWidget();
    if (current_page != this->ui_->pageRecognitionNowPlaying) {
        return;
    }

    this->ui_->pageRecognitionNowPlaying->handleAnimeAddUpdateFinished(anime_list);
}

void AnimePlayingPage::showNoMatchPage(const QString &message) {
    this->ui_->pageRecognitionNoMatch->setDisplayedMessage(message);
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionNoMatch
    );
}

void AnimePlayingPage::showIdlePage() {
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionIdle
    );
}

void AnimePlayingPage::showSelectAnimePage(const QList<RecognizedAnime> &recognized_anime, const QString &title) {
    // TODO: Set select anime info
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionSelectAnime
    );
}

void AnimePlayingPage::showNowPlayingPage(const RecognizedAnime &recognized_anime, const QString &title) {
    this->ui_->pageRecognitionNowPlaying->setNowPlayingAnime(
        recognized_anime,
        title
    );
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionNowPlaying
    );
}

void AnimePlayingPage::showSearchPage(const QString &title) {
    this->ui_->pageRecognitionSearch->setPlayingTitle(title);
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionSearch
    );
}

void AnimePlayingPage::setupPage() {
    this->ui_->setupUi(this);
}

void AnimePlayingPage::setupSelectAnimePage() {
    // TODO:
}

void AnimePlayingPage::setupNowPlayingPage() {
    connect(
        this->ui_->pageRecognitionNowPlaying, &RecognitionNowPlayingPage::requestShowAnimeInfoEditDialog,
        this, &AnimePlayingPage::requestShowAnimeInfoEditDialog
    );

    connect(
        this->ui_->pageRecognitionNowPlaying, &RecognitionNowPlayingPage::requestAddMedia,
        this, &AnimePlayingPage::requestAddMedia
    );
}

void AnimePlayingPage::setupSearchPage() {
    connect(this->ui_->pageRecognitionSearch, &RecognitionSearchPage::requestQuietAnimeSearch, this, &AnimePlayingPage::requestQuietAnimeSearch);
}

