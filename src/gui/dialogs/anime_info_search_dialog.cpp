#include "gui/dialogs/anime_info_search_dialog.hpp"

#include "utils/cache.hpp"
#include "utils/anilist.hpp"
#include "utils/links.hpp"

#include <QPushButton>


AnimeInfoSearchDialog::AnimeInfoSearchDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::AnimeInfoSearchDialog) {

    this->ui_->setupUi(this);

    for (auto &button : this->ui_->buttonBox->buttons()) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    connect(this->ui_->comboBoxAnimeStatus, &AnimeStatusComboBox::animeStatusActivated, this, [this] (AnilistEntry::Status status) {
        emit requestAddMedia({this->media_}, status);
        this->ui_->labelCoverImage->clear();
        this->accept();
    });

    connect(this, &QDialog::rejected, this, [this] {
        this->ui_->labelCoverImage->clear();
    });
}

AnimeInfoSearchDialog::~AnimeInfoSearchDialog() {
    delete this->ui_;
}

void AnimeInfoSearchDialog::updateUserPreferences(const AnilistAccount::User &user) {
    this->title_language_ = user.title_language;
}

void AnimeInfoSearchDialog::setMedia(const AnilistMedia &media) {
    this->media_ = media;

    this->updateCoverImage();
    this->updateInfo();
}

void AnimeInfoSearchDialog::showOrFocus() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->show();
    }
}

void AnimeInfoSearchDialog::updateCoverImage() {
    auto *request = Cache::requestPixmap(this->media_.cover_url);
    connect(request, &ImageRequest::finished, this->ui_->labelCoverImage, &QLabel::setPixmap);
    connect(request, &ImageRequest::failed, this, [this] {
        this->ui_->labelCoverImage->setPixmap(
            this->no_image_
        );
    });
}

void AnimeInfoSearchDialog::updateInfo() {
    const auto &media = this->media_;

    this->ui_->comboBoxAnimeStatus->setCurrentIndex(-1);

    // Disable adding to list if already in list
    if (media.in_list) {
        this->ui_->comboBoxAnimeStatus->setEnabled(false);
    } else {
        this->ui_->comboBoxAnimeStatus->setEnabled(true);
    }

    // Top title
    this->ui_->labelTitleHeader->setText(
        LinksUtils::createTextLink(
            media.site_url, AnilistUtils::animeTitleToPrettyString(media.title, this->title_language_)
        )
    );

    // Titles
    this->ui_->labelTitleRomaji->setText(media.title.romaji);
    this->ui_->labelTitleEnglish->setText(media.title.english);
    this->ui_->labelTitleNative->setText(media.title.native);

    this->ui_->plainTextEditSynonyms->clear();
    for (const auto &synonym : media.synonyms) {
        this->ui_->plainTextEditSynonyms->appendPlainText(
            QStringLiteral("• %1").arg(synonym)
        );
    }
    this->ui_->plainTextEditSynonyms->moveCursor(QTextCursor::Start);

    this->ui_->labelFormat->setText(
        AnilistUtils::mediaFormatToPrettyString(media.format)
    );
    this->ui_->labelEpisodes->setText(QString::number(media.episodes));
    this->ui_->labelStatus->setText(
        AnilistUtils::mediaStatusToPrettyString(media.status)
    );
    this->ui_->labelSeason->setText(
        AnilistUtils::mediaSeasonToPrettyString(media.season, media.season_year)
    );
    this->ui_->labelGenres->setText(media.genres.join(QStringLiteral(", ")));

    QStringList studios_text;
    for (const auto &studio : media.studios) {
        studios_text.append(
            LinksUtils::createTextLink(studio.site_url, studio.name)
        );
    }
    this->ui_->labelStudios->setText(studios_text.join(QStringLiteral(", ")));

    QStringList producers_text;
    for (const auto &producer : media.producers) {
        producers_text.append(
            LinksUtils::createTextLink(producer.site_url, producer.name)
        );
    }
    this->ui_->labelProducers->setText(producers_text.join(QStringLiteral(", ")));

    this->ui_->labelStarted->setText(AnilistUtils::dateToPrettyString(media.start_date));
    this->ui_->labelEnded->setText(AnilistUtils::dateToPrettyString(media.end_date));
    this->ui_->labelOrigin->setText(media.country_of_origin);
    this->ui_->labelScore->setText(QStringLiteral("%1%").arg(media.average_score));
    // Synopsis
    this->ui_->textEditSynopsis->setHtml(media.description);

}
