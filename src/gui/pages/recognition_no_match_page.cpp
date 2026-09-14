#include "gui/pages/recognition_no_match_page.hpp"


RecognitionNoMatchPage::RecognitionNoMatchPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionNoMatchWidget) {

    this->initPage();
}

RecognitionNoMatchPage::~RecognitionNoMatchPage() {
    delete this->ui_;
}

void RecognitionNoMatchPage::setDisplayedMessage(const QString &message) {
    this->ui_->labelNoMatchMessage->setText(message);
}

void RecognitionNoMatchPage::initPage() {
    this->ui_->setupUi(this);
}

