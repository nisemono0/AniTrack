#include "gui/pages/recognition_search_page.hpp"


RecognitionSearchPage::RecognitionSearchPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionSearchWidget) {

    this->setupPage();
}

RecognitionSearchPage::~RecognitionSearchPage() {
    delete this->ui_;
}

void RecognitionSearchPage::setupPage() {
    this->ui_->setupUi(this);
}
