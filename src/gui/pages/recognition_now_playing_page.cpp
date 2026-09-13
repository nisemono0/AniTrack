#include "gui/pages/recognition_now_playing_page.hpp"


RecognitionNowPlayingPage::RecognitionNowPlayingPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionNowPlayingWidget) {

    this->setupPage();
}

RecognitionNowPlayingPage::~RecognitionNowPlayingPage() {
    delete this->ui_;
}

void RecognitionNowPlayingPage::setupPage() {
    this->ui_->setupUi(this);
}
