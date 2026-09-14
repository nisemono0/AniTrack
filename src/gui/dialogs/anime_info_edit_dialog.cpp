#include "gui/dialogs/anime_info_edit_dialog.hpp"

#include "utils/anilist.hpp"
#include "utils/date.hpp"
#include "utils/sync.hpp"
#include "utils/cache.hpp"
#include "utils/log.hpp"
#include "utils/links.hpp"


AnimeInfoEditDialog::AnimeInfoEditDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::AnimeInfoEditDialog) {

    this->ui_->setupUi(this);

    connect(this->ui_->spinBoxEpisodesWatched, &QSpinBox::valueChanged, this, [this] (int i) {
        this->new_state_.progress = i;
    });
    connect(this->ui_->spinBoxRewatches, &QSpinBox::valueChanged, this, [this] (int i) {
        this->new_state_.repeat = i;
    });
    connect(this->ui_->spinBoxScore, &QSpinBox::valueChanged, this, [this] (int i) {
        this->new_state_.score = i;
        this->ui_->labelFormattedScore->setText(
            AnilistUtils::entryScoreToPrettyString(i, this->score_format_)
        );
    });
    connect(this->ui_->comboBoxEditStatus, &AnimeStatusComboBox::animeStatusActivated, this, [this] (AnilistEntry::Status status) {
        this->new_state_.status = status;
    });
    connect(this->ui_->widgetStartedDatePicker, &DatePicker::dateChanged, this, [this] (QDate date) {
        this->new_state_.started_at = date;
    });
    connect(this->ui_->widgetCompletedDatePicker, &DatePicker::dateChanged, this, [this] (QDate date) {
        this->new_state_.completed_at = date;
    });
    connect(this->ui_->plainTextEditNotes, &QPlainTextEdit::textChanged, this, [this] {
        this->new_state_.notes = this->ui_->plainTextEditNotes->toPlainText();
    });

    connect(this->ui_->pushButtonClose, &QPushButton::clicked, this, &AnimeInfoEditDialog::rejectDialog);
    connect(this->ui_->pushButtonSave, &QPushButton::clicked, this, &AnimeInfoEditDialog::updateAnime);
    connect(this->ui_->comboBoxAddStatus, &AnimeStatusComboBox::animeStatusActivated, this, &AnimeInfoEditDialog::addAnime);
}

AnimeInfoEditDialog::~AnimeInfoEditDialog() {
    delete this->ui_;
}

void AnimeInfoEditDialog::onUserUpdated(const AnilistAccount::User &user) {
    this->score_format_ = user.score_format;
    this->title_language_ = user.title_language;

    this->updateInfoTab();
    this->updateEditTab();
}

void AnimeInfoEditDialog::showOrFocus() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->show();
    }
}

void AnimeInfoEditDialog::showOrFocusInfoEdit(const AnilistAnime &anime, AnimeInfoEditDialog::Page page) {
    this->showEditTab();
    this->ui_->pushButtonSave->show();
    this->ui_->comboBoxAddStatus->hide();

    this->setAnime(anime);

    switch (page) {
        case Page::Info: {
            this->ui_->tabWidgetInfoEdit->setCurrentWidget(
                this->ui_->infoPage
            );
            break;
        }
        case Page::Edit: {
            this->ui_->tabWidgetInfoEdit->setCurrentWidget(
                this->ui_->editPage
            );
            break;
        }
        default:
            break;
    }

    this->showOrFocus();
}

void AnimeInfoEditDialog::showOrFocusAdd(const AnilistMedia &media) {
    this->hideEditTab();
    this->ui_->pushButtonSave->hide();
    this->ui_->comboBoxAddStatus->show();

    this->setMedia(media);

    this->ui_->tabWidgetInfoEdit->setCurrentWidget(
        this->ui_->infoPage
    );

    this->ui_->comboBoxAddStatus->setCurrentIndex(-1);

    // Disable adding to list if already in list
    if (media.in_list) {
        this->ui_->comboBoxAddStatus->setEnabled(false);
    } else {
        this->ui_->comboBoxAddStatus->setEnabled(true);
    }

    this->showOrFocus();
}

void AnimeInfoEditDialog::setAnime(const AnilistAnime &anime) {
    this->entry_ = anime.entry;
    this->media_ = anime.media;
    this->new_state_ = anime.entry.state();
    this->original_state_ = anime.entry.state();

    this->updateCoverImage();
    this->updateInfoTab();
    this->updateEditTab();
}

void AnimeInfoEditDialog::setMedia(const AnilistMedia &media) {
    this->entry_ = {};
    this->media_ = media;

    this->updateCoverImage();
    this->updateInfoTab();
    this->updateEditTab();
}

void AnimeInfoEditDialog::hideEditTab() {
    const int edit_tab = this->ui_->tabWidgetInfoEdit->indexOf(
        this->ui_->editPage
    );

    this->ui_->tabWidgetInfoEdit->setTabVisible(edit_tab, false);
}

void AnimeInfoEditDialog::showEditTab() {
    const int edit_tab = this->ui_->tabWidgetInfoEdit->indexOf(
        this->ui_->editPage
    );

    this->ui_->tabWidgetInfoEdit->setTabVisible(edit_tab, true);
}

void AnimeInfoEditDialog::updateCoverImage() {
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

void AnimeInfoEditDialog::updateInfoTab() {
    const auto &media = this->media_;

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

void AnimeInfoEditDialog::updateEditTab() {
    // Block signals from firing while setting up the edit page
    const QSignalBlocker episodes_watched_blocker(this->ui_->spinBoxEpisodesWatched);
    const QSignalBlocker rewatched_blocker(this->ui_->spinBoxRewatches);
    const QSignalBlocker score_blocker(this->ui_->spinBoxScore);
    const QSignalBlocker status_blocker(this->ui_->comboBoxEditStatus);
    const QSignalBlocker started_date_blocker(this->ui_->widgetStartedDatePicker);
    const QSignalBlocker completed_date_blocker(this->ui_->widgetCompletedDatePicker);
    const QSignalBlocker notes_blocker(this->ui_->plainTextEditNotes);

    const auto &media = this->media_;
    const auto &entry_state = this->entry_.state();

    if (media.episodes > 0) {
        this->ui_->spinBoxEpisodesWatched->setMaximum(media.episodes);
    } else {
        this->ui_->spinBoxEpisodesWatched->setMaximum(INT_MAX);
    }

    this->ui_->spinBoxEpisodesWatched->setValue(entry_state.progress);
    this->ui_->spinBoxRewatches->setValue(entry_state.repeat);
    this->ui_->spinBoxScore->setValue(entry_state.score);
    this->ui_->labelFormattedScore->setText(
        AnilistUtils::entryScoreToPrettyString(entry_state.score, this->score_format_)
    );

    this->ui_->comboBoxEditStatus->setCurrentStatus(
        entry_state.status
    );

    this->ui_->widgetStartedDatePicker->setDate(entry_state.started_at);

    this->ui_->widgetCompletedDatePicker->setDate(entry_state.completed_at);

    this->ui_->plainTextEditNotes->setPlainText(entry_state.notes);
}

void AnimeInfoEditDialog::rejectDialog() {
    this->ui_->labelCoverImage->clear();
    this->reject();
}

void AnimeInfoEditDialog::updateAnime() {
    if (this->new_state_ == this->original_state_) {
        this->reject();
        return;
    }

    this->new_state_.updated_at = DateUtils::currentEpochTime();
    this->new_state_.pending_operation = SyncUtils::mergePendingOperation(
        this->new_state_.pending_operation,
        AnilistEntry::PendingOperation::UPDATE
    );

    this->new_state_.name = QStringLiteral("Dialog edit");
    this->entry_.addState(this->new_state_);

    AnilistAnime update_anime{
        this->entry_,
        this->media_
    };

    emit requestUpdateAnime(update_anime);

    this->ui_->labelCoverImage->clear();
    this->accept();
}

void AnimeInfoEditDialog::addAnime(AnilistEntry::Status status) {
    emit requestAddMedia({this->media_}, status);

    this->ui_->labelCoverImage->clear();
    this->accept();
}

