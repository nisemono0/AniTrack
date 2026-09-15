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
    this->ui_->pageRecognitionSelectAnime->updateUserPreferences(user);
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

    this->stopNowPlayingTimer();
}

void AnimePlayingPage::showIdlePage() {
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionIdle
    );

    this->stopNowPlayingTimer();
}

void AnimePlayingPage::showSelectAnimePage(const QList<RecognizedAnime> &recognized_anime, const QString &title, int episode) {
    this->ui_->pageRecognitionSelectAnime->setPlayingAnime(title, episode);
    this->ui_->pageRecognitionSelectAnime->setAnimeSelection(recognized_anime);

    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionSelectAnime
    );

    this->stopNowPlayingTimer();
}

void AnimePlayingPage::showNowPlayingPage(const RecognizedAnime &recognized_anime, const QString &title) {
    this->ui_->pageRecognitionNowPlaying->setNowPlayingAnime(
        recognized_anime,
        title
    );
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionNowPlaying
    );

    this->startNowPlaingTimer();
}

void AnimePlayingPage::showSearchPage(const QString &title, int episode) {
    this->ui_->pageRecognitionSearch->setPlayingAnime(title, episode);
    this->ui_->stackedWidgetAnimePlaying->setCurrentWidget(
        this->ui_->pageRecognitionSearch
    );

    this->stopNowPlayingTimer();
}

void AnimePlayingPage::setupPage() {
    this->ui_->setupUi(this);
}

void AnimePlayingPage::setupSelectAnimePage() {
    connect(this->ui_->pageRecognitionSelectAnime, &RecognitionSelectAnimePage::recognizedAnimeSelected, this, &AnimePlayingPage::recognizedAnimeSelected);
    connect(this->ui_->pageRecognitionSelectAnime, &RecognitionSelectAnimePage::requestQuietAnimeSearch, this, &AnimePlayingPage::requestQuietAnimeSearch);
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

    connect(
        this->ui_->pageRecognitionNowPlaying, &RecognitionNowPlayingPage::requestSetAnimeProgress,
        this, &AnimePlayingPage::requestSetAnimeProgress
    );
}

void AnimePlayingPage::setupSearchPage() {
    connect(this->ui_->pageRecognitionSearch, &RecognitionSearchPage::requestQuietAnimeSearch, this, &AnimePlayingPage::requestQuietAnimeSearch);
}

void AnimePlayingPage::startNowPlaingTimer() {
    this->ui_->pageRecognitionNowPlaying->startPopupTimer();
}

void AnimePlayingPage::stopNowPlayingTimer() {
    this->ui_->pageRecognitionNowPlaying->stopPopupTimer();
}

