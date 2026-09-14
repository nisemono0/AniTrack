#include "gui/pages/recognition_idle_page.hpp"


RecognitionIdlePage::RecognitionIdlePage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionIdleWidget) {

    this->initPage();
}

RecognitionIdlePage::~RecognitionIdlePage() {
    delete this->ui_;
}

void RecognitionIdlePage::initPage() {
    this->ui_->setupUi(this);
}

