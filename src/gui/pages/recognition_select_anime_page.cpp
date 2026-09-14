#include "gui/pages/recognition_select_anime_page.hpp"


RecognitionSelectAnimePage::RecognitionSelectAnimePage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionSelectAnimeWidget) {

    this->initPage();
}

RecognitionSelectAnimePage::~RecognitionSelectAnimePage() {
    delete this->ui_;
}

void RecognitionSelectAnimePage::initPage() {
    this->ui_->setupUi(this);
}
