#include "gui/pages/recognition_now_playing_page.hpp"

#include "utils/anilist.hpp"
#include "utils/links.hpp"
#include "utils/cache.hpp"
#include "utils/log.hpp"

RecognitionNowPlayingPage::RecognitionNowPlayingPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionNowPlayingWidget) {

    this->initPage();
    this->setupPage();
}

RecognitionNowPlayingPage::~RecognitionNowPlayingPage() {
    delete this->ui_;
}

void RecognitionNowPlayingPage::updateUserPreferences(const AnilistAccount::User &user) {
    this->title_language_ = user.title_language;
    this->updateNowPlayingInfo();
}

void RecognitionNowPlayingPage::handleAnimeAddUpdateFinished(const QList<AnilistAnime> &anime_list) {
    for (const auto &anime : anime_list) {
        if (anime.media.id != this->media_.id) {
            continue;
        }

        this->entry_ = anime.entry;
        this->media_ = anime.media;
        this->showEdit();

        this->updateNowPlayingInfo();
    }
}

void RecognitionNowPlayingPage::setNowPlayingAnime(const RecognizedAnime &recognized_anime, const QString &title) {
    this->playing_title_ = title;
    this->episode_ = recognized_anime.episode;

    this->media_ = recognized_anime.media;
    if (recognized_anime.entry) {
        // anime is in our list, show edit widgets
        this->showEdit();
        this->entry_ = recognized_anime.entry.value();
    } else {
        // anime no in our list, show add widgets
        this->showAdd();
        this->entry_ = {};
    }

    this->updateCoverImage();
    this->updateNowPlayingInfo();
}

void RecognitionNowPlayingPage::showEdit() {
    this->ui_->pushButtonEdit->show();
    this->ui_->comboBoxAnimeStatus->hide();
}

void RecognitionNowPlayingPage::showAdd() {
    this->ui_->pushButtonEdit->hide();
    this->ui_->comboBoxAnimeStatus->show();
    this->ui_->comboBoxAnimeStatus->setCurrentIndex(-1);
}

void RecognitionNowPlayingPage::initPage() {
    this->ui_->setupUi(this);
}

void RecognitionNowPlayingPage::setupPage() {
    connect(this->ui_->comboBoxAnimeStatus, &AnimeStatusComboBox::animeStatusActivated, this, [this] (AnilistEntry::Status status) {
        emit requestAddMedia({this->media_}, status);
    });

    connect(this->ui_->pushButtonEdit, &QPushButton::clicked, this, [this] {
        AnilistAnime anime{
            this->entry_,
            this->media_
        };

        emit requestShowAnimeInfoEditDialog(anime, AnimeInfoEditDialog::Page::Edit);
    });
}

void RecognitionNowPlayingPage::updateCoverImage() {
    auto *request = Cache::requestPixmap(this->media_.cover_url);
    connect(request, &ImageRequest::finished, this->ui_->labelCoverImage, &QLabel::setPixmap);
    connect(request, &ImageRequest::failed, this, [this] {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Cache request failed, using default image")
        );
        this->ui_->labelCoverImage->setPixmap(
            this->no_image_
        );
    });
}

void RecognitionNowPlayingPage::updateNowPlayingInfo() {
    const auto &media = this->media_;

    // Now playing title
    this->ui_->labelPlayingTitle->setText(
        QStringLiteral("Episode: %1").arg(this->episode_)
    );

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

    this->ui_->labelFormat->setText(
        AnilistUtils::mediaFormatToPrettyString(media.format)
    );
    this->ui_->labelEpisodes->setText(
        AnilistUtils::mediaEpisodesToPrettyString(media.episodes)
    );
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
