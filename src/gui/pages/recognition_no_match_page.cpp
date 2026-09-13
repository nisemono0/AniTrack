#include "gui/pages/recognition_no_match_page.hpp"


RecognitionNoMatchPage::RecognitionNoMatchPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionNoMatchWidget) {

    this->setupPage();
}

RecognitionNoMatchPage::~RecognitionNoMatchPage() {
    delete this->ui_;
}

void RecognitionNoMatchPage::setupPage() {
    this->ui_->setupUi(this);
}
