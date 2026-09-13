#include "gui/pages/recognition_select_anime_page.hpp"


RecognitionSelectAnimePage::RecognitionSelectAnimePage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionSelectAnimeWidget) {

    this->setupPage();
}

RecognitionSelectAnimePage::~RecognitionSelectAnimePage() {
    delete this->ui_;
}

void RecognitionSelectAnimePage::setupPage() {
    this->ui_->setupUi(this);
}
