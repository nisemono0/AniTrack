#include "gui/pages/recognition_error_page.hpp"


RecognitionErrorPage::RecognitionErrorPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionErrorWidget) {

    this->initPage();
}

RecognitionErrorPage::~RecognitionErrorPage() {
    delete this->ui_;
}

void RecognitionErrorPage::setDisplayedMessage(const QString &message) {
    this->ui_->labelErrorMessage->setText(message);
}

void RecognitionErrorPage::initPage() {
    this->ui_->setupUi(this);
}

