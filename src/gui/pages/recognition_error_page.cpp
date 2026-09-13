#include "gui/pages/recognition_error_page.hpp"


RecognitionErrorPage::RecognitionErrorPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionErrorWidget) {

    this->setupPage();
}

RecognitionErrorPage::~RecognitionErrorPage() {
    delete this->ui_;
}

void RecognitionErrorPage::setupPage() {
    this->ui_->setupUi(this);
}
