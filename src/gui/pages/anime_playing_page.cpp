#include "gui/pages/anime_playing_page.hpp"


AnimePlayingPage::AnimePlayingPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::AnimePlayingWidget) {

    this->setupPage();
}

AnimePlayingPage::~AnimePlayingPage() {
    delete this->ui_;
}

void AnimePlayingPage::setupPage() {
    this->ui_->setupUi(this);
}

