#include "gui/pages/recognition_search_page.hpp"


RecognitionSearchPage::RecognitionSearchPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionSearchWidget) {

    this->initPage();
    this->setupPage();
}

RecognitionSearchPage::~RecognitionSearchPage() {
    delete this->ui_;
}

void RecognitionSearchPage::setPlayingTitle(const QString &title) {
    this->playing_title_= title;

    this->ui_->labelPlayingTitle->setText(title);
    this->ui_->lineEditSearchText->setPlaceholderText(title);
}

void RecognitionSearchPage::initPage() {
    this->ui_->setupUi(this);
}

void RecognitionSearchPage::setupPage() {
    connect(this->ui_->lineEditSearchText, &QLineEdit::returnPressed, this, &RecognitionSearchPage::requestQuietSearch);
    connect(this->ui_->pushButtonSearch, &QPushButton::clicked, this, &RecognitionSearchPage::requestQuietSearch);
}

void RecognitionSearchPage::requestQuietSearch() {
    const QString search_text = this->ui_->lineEditSearchText->text();

    if (search_text.isEmpty()) {
        emit requestQuietAnimeSearch(this->playing_title_);
        return;
    }

    emit requestQuietAnimeSearch(search_text);
}

