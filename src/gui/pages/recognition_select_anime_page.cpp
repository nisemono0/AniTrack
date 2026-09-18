#include "gui/pages/recognition_select_anime_page.hpp"

#include "gui/widgets/label.hpp"

#include "utils/anilist.hpp"


RecognitionSelectAnimePage::RecognitionSelectAnimePage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionSelectAnimeWidget) {

    this->initPage();
    this->setupPage();
}

RecognitionSelectAnimePage::~RecognitionSelectAnimePage() {
    delete this->ui_;
}

void RecognitionSelectAnimePage::updateUserPreferences(const AnilistAccount::User &user) {
    this->title_language_ = user.title_language;
    this->updateSelection();
}

void RecognitionSelectAnimePage::setAnimeSelection(const QList<RecognizedAnime> &recognized_anime) {
    this->recognized_anime_ = recognized_anime;
    this->updateSelection();
}

void RecognitionSelectAnimePage::setPlayingAnime(const QString &title, int episode, const QString &release_group) {
    this->playing_title_ = title;

    this->ui_->lineEditSearchText->clear();
    this->ui_->lineEditSearchText->setPlaceholderText(title);

    this->ui_->labelPlayingTitle->setText(
        QStringLiteral("%1 [Episode %2 by %3]").arg(title)
                                               .arg(episode)
                                               .arg(release_group)
    );
}

void RecognitionSelectAnimePage::clearSelection() {
    QLayoutItem *item;

    while ((item = this->ui_->verticalLayoutInList->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    while ((item = this->ui_->verticalLayoutNotInList->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }
}

void RecognitionSelectAnimePage::updateSelection() {
    this->clearSelection();

    for (int i = 0; i < this->recognized_anime_.size(); i++) {
        const auto &recognized = this->recognized_anime_.at(i);
        Label *label = new Label(
            // hack to make the label look like a hyperlink
            QStringLiteral("<a href=#>• %1</a>").arg(
                AnilistUtils::animeTitleToPrettyString(recognized.media.title, this->title_language_)
            )
        );

        connect(label, &Label::clicked, this, [this, i] {
            emit recognizedAnimeSelected(
                this->recognized_anime_.at(i)
            );
        });

        if (recognized.entry) {
            this->ui_->verticalLayoutInList->addWidget(label);
        } else {
            this->ui_->verticalLayoutNotInList->addWidget(label);
        }
    }

    const bool has_in_list_items = this->ui_->verticalLayoutInList->count() > 0;
    this->ui_->labelInList->setVisible(has_in_list_items);
    this->ui_->lineInList->setVisible(has_in_list_items);
    this->ui_->scrollAreaInList->setVisible(has_in_list_items);

    const bool has_not_in_list_items = this->ui_->verticalLayoutNotInList->count() > 0;
    this->ui_->labelNotInList->setVisible(has_not_in_list_items);
    this->ui_->lineNotInList->setVisible(has_not_in_list_items);
    this->ui_->scrollAreaNotInList->setVisible(has_not_in_list_items);


}

void RecognitionSelectAnimePage::initPage() {
    this->ui_->setupUi(this);
    this->ui_->verticalLayoutInList->setAlignment(Qt::AlignTop);
    this->ui_->verticalLayoutNotInList->setAlignment(Qt::AlignTop);
}

void RecognitionSelectAnimePage::setupPage() {
    connect(this->ui_->lineEditSearchText, &QLineEdit::returnPressed, this, &RecognitionSelectAnimePage::searchAnime);
    connect(this->ui_->pushButtonSearch, &QPushButton::clicked, this, &RecognitionSelectAnimePage::searchAnime);
}

void RecognitionSelectAnimePage::searchAnime() {
    const QString search_text = this->ui_->lineEditSearchText->text();

    if (search_text.isEmpty()) {
        emit requestQuietAnimeSearch(this->playing_title_);
        return;
    }

    emit requestQuietAnimeSearch(search_text);
    this->ui_->lineEditSearchText->clear();
}

