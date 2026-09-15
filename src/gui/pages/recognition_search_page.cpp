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

void RecognitionSearchPage::setPlayingAnime(const QString &title, int episode) {
    this->playing_title_= title;

    this->ui_->lineEditSearchText->clear();
    this->ui_->lineEditSearchText->setPlaceholderText(title);

    this->ui_->labelPlayingTitle->setText(
        QStringLiteral("%1 [Episode %2]").arg(title).arg(episode)
    );
}

void RecognitionSearchPage::initPage() {
    this->ui_->setupUi(this);
}

void RecognitionSearchPage::setupPage() {
    connect(this->ui_->lineEditSearchText, &QLineEdit::returnPressed, this, &RecognitionSearchPage::searchAnime);
    connect(this->ui_->pushButtonSearch, &QPushButton::clicked, this, &RecognitionSearchPage::searchAnime);
}

void RecognitionSearchPage::searchAnime() {
    const QString search_text = this->ui_->lineEditSearchText->text();

    if (search_text.isEmpty()) {
        emit requestQuietAnimeSearch(this->playing_title_);
        return;
    }

    emit requestQuietAnimeSearch(search_text);
    this->ui_->lineEditSearchText->clear();
}

