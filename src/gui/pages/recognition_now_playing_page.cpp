#include "gui/pages/recognition_now_playing_page.hpp"

#include "utils/anilist.hpp"
#include "utils/links.hpp"
#include "utils/cache.hpp"
#include "utils/log.hpp"
#include "utils/settings.hpp"


RecognitionNowPlayingPage::RecognitionNowPlayingPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::RecognitionNowPlayingWidget) {

    this->initPage();
    this->setupPage();

    this->applySettings();
    connect(App::instance()->settings(), &AppSettings::settingsChanged, this, &RecognitionNowPlayingPage::applySettings);
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
    this->playing_episode_ = recognized_anime.episode;
    this->release_group_ = recognized_anime.release_group;

    this->media_ = recognized_anime.media;
    // entry_ is nullopt if anime not in our list
    if (!recognized_anime.entry) {
        this->entry_.reset();
        this->showAdd();
    } else {
        this->entry_ = recognized_anime.entry;
        this->showEdit();
    }

    this->updateCoverImage();
    this->updateNowPlayingInfo();
}

void RecognitionNowPlayingPage::startPopupTimer() {
    if (this->is_popup_enabled_) {
        this->popup_timer_->start();
        this->display_timer_->start();
    }
}

void RecognitionNowPlayingPage::stopPopupTimer() {
    this->popup_timer_->stop();
    this->display_timer_->stop();
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

    this->popup_timer_ = new QTimer(this);
    this->popup_timer_->setSingleShot(true);

    this->display_timer_ = new QTimer(this);
    this->display_timer_->setInterval(std::chrono::seconds(1));
    this->display_timer_->setSingleShot(false);
}

void RecognitionNowPlayingPage::setupPage() {
    connect(this->ui_->comboBoxAnimeStatus, &AnimeStatusComboBox::animeStatusActivated, this, [this] (AnilistEntry::Status status) {
        emit requestAddMedia({this->media_}, status);
    });

    connect(this->ui_->pushButtonEdit, &QPushButton::clicked, this, [this] {
        AnilistAnime anime{
            this->entry_.value(),
            this->media_
        };

        emit requestShowAnimeInfoEditDialog(anime, AnimeInfoEditDialog::Page::Edit);
    });

    connect(this->popup_timer_, &QTimer::timeout, this, &RecognitionNowPlayingPage::onPopupTimerTimeout);

    connect(this->display_timer_, &QTimer::timeout, this, [this] {
        emit requestShowPopupRemainingTime(
            QStringLiteral("Time until popup: %1s").arg(
                std::chrono::duration_cast<std::chrono::seconds>(
                    this->popup_timer_->remainingTimeAsDuration()
                ).count()
            ),
            1000
        );
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

    // Show the entry progress if entry in our list
    // otherwise hide it
    const auto &entry = this->entry_;
    if (entry) {
        this->ui_->labelProgress->setVisible(true);
        this->ui_->labelProgressText->setVisible(true);
        if (media.episodes <= 0) {
            this->ui_->labelProgressText->setText(
                QStringLiteral("%1/?").arg(entry->state().progress)
            );
        } else {
            this->ui_->labelProgressText->setText(
                QStringLiteral("%1/%2").arg(entry->state().progress).arg(media.episodes)
            );
        }
    } else {
        this->ui_->labelProgress->setVisible(false);
        this->ui_->labelProgressText->setVisible(false);
    }

    // Now playing episode
    this->ui_->labelPlayingTitle->setText(
        QStringLiteral("Episode %1 by %2").arg(this->playing_episode_)
                                          .arg(this->release_group_)
    );

    // Top title
    this->playing_title_ = AnilistUtils::animeTitleToPrettyString(media.title, this->title_language_);
    this->ui_->labelTitleHeader->setText(
        LinksUtils::createTextLink(media.site_url, this->playing_title_)
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

void RecognitionNowPlayingPage::openPopupDialog(const QString &header_text,
                                                const QString &body_text,
                                                NowPlayingPopupDialog::PopupType popup_type) {
    // create a new dialog if it doesnt exist and
    // connect to its accepted signal once
    if (!this->popup_dialog_) {
        this->popup_dialog_ = new NowPlayingPopupDialog(header_text, body_text, popup_type, this);

        connect(this->popup_dialog_, &NowPlayingPopupDialog::popupAccepted, this, &RecognitionNowPlayingPage::onPopupAccepted);

        this->popup_dialog_->open();
        return;
    }

    // set text and display existing one
    this->popup_dialog_->setHeadertext(header_text);
    this->popup_dialog_->setBodyText(body_text);
    this->popup_dialog_->setPopupType(popup_type);
    this->popup_dialog_->open();
}

void RecognitionNowPlayingPage::onPopupTimerTimeout() {
    this->display_timer_->stop();

    // anime not in list, ask to add
    if (!this->entry_) {
        this->openPopupDialog(
            this->playing_title_,
            QStringLiteral("Add to watching list"),
            NowPlayingPopupDialog::PopupType::Add
        );
    } else {
        // anime in list, update it
        // ask to set it as completed if current episode >= total episodes
        // and anime has a known number of episodes media.episodes > 0
        // don't show the popup if current episode == entry progress
        if (this->playing_episode_ >= this->media_.episodes &&
            this->media_.episodes > 0) {
            this->openPopupDialog(
                this->playing_title_,
                QStringLiteral("Set as completed"),
                NowPlayingPopupDialog::PopupType::Complete
            );
        } else if (this->playing_episode_ > this->entry_->state().progress) {
            // ask to set progress to currently playing episode
            this->openPopupDialog(
                this->playing_title_,
                QStringLiteral("Set as watching and progress to: %1").arg(this->playing_episode_),
                NowPlayingPopupDialog::PopupType::Update
            );
        }
    }
}

void RecognitionNowPlayingPage::onPopupAccepted(NowPlayingPopupDialog::PopupType popup_type) {
    switch (popup_type) {
        case NowPlayingPopupDialog::PopupType::Add: {
            emit requestAddMedia({this->media_}, AnilistEntry::Status::CURRENT);
            break;
        }
        case NowPlayingPopupDialog::PopupType::Update: {
            AnilistAnime anime{
                this->entry_.value(),
                this->media_
            };
            emit requestSetNowPlayingAnimeProgress(anime, this->playing_episode_);
            break;
        }
        case NowPlayingPopupDialog::PopupType::Complete: {
            if (this->media_.episodes <= 0) {
                return;
            }

            AnilistAnime anime{
                this->entry_.value(),
                this->media_
            };
            emit requestSetNowPlayingAnimeProgress(anime, anime.media.episodes);
            break;
        }
    }
}

void RecognitionNowPlayingPage::applySettings() {
    this->is_popup_enabled_ = Settings::get(Settings::Recognition::EnableRecognitionPopup, true);

    if (!this->is_popup_enabled_) {
        this->popup_timer_->stop();
        this->display_timer_->stop();
    }

    this->popup_timer_->setInterval(
        std::chrono::seconds(
            Settings::get(Settings::Recognition::RecognitionPopupDelay, 120)
        )
    );
}

