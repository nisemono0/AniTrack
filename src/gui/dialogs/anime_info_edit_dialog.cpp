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

    for (auto &button : this->ui_->buttonBox->buttons()) {
        button->setFocusPolicy(Qt::NoFocus);
    }

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
    connect(this->ui_->comboBoxStatus, &AnimeStatusComboBox::animeStatusActivated, this, [this] (AnilistEntry::Status status) {
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

    connect(this, &QDialog::accepted, this, &AnimeInfoEditDialog::onDialogAccepted);
    connect(this, &QDialog::rejected, this, &AnimeInfoEditDialog::onDialogRejected);
}

AnimeInfoEditDialog::~AnimeInfoEditDialog() {
    delete this->ui_;
}

void AnimeInfoEditDialog::updateUserPreferences(const AnilistAccount::User &user) {
    this->score_format_ = user.score_format;
    this->title_language_ = user.title_language;
}

void AnimeInfoEditDialog::setAnime(const AnilistAnime &anime) {
    this->anime_ = anime;
    this->new_state_ = anime.entry.state();
    this->original_state_ = anime.entry.state();
    this->updateCoverImage();
    this->updateInfoTab();
    this->updateEditTab();
}

void AnimeInfoEditDialog::showOrFocus(Page page) {
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

    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
    } else {
        this->show();
    }
}

void AnimeInfoEditDialog::updateCoverImage() {
    auto *request = Cache::requestPixmap(this->anime_.media.cover_url);
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
    const auto &media = this->anime_.media;

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

void AnimeInfoEditDialog::updateEditTab() {
    // Block signals from firing while setting up the edit page
    QSignalBlocker episodes_watched_blocker(this->ui_->spinBoxEpisodesWatched);
    QSignalBlocker rewatched_blocker(this->ui_->spinBoxRewatches);
    QSignalBlocker score_blocker(this->ui_->spinBoxScore);
    QSignalBlocker status_blocker(this->ui_->comboBoxStatus);
    QSignalBlocker started_date_blocker(this->ui_->widgetStartedDatePicker);
    QSignalBlocker completed_date_blocker(this->ui_->widgetCompletedDatePicker);
    QSignalBlocker notes_blocker(this->ui_->plainTextEditNotes);

    const auto &media = this->anime_.media;
    const auto &entry_state = this->anime_.entry.state();

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

    this->ui_->comboBoxStatus->setCurrentStatus(
        entry_state.status
    );

    this->ui_->widgetStartedDatePicker->setDate(entry_state.started_at);

    this->ui_->widgetCompletedDatePicker->setDate(entry_state.completed_at);

    this->ui_->plainTextEditNotes->setPlainText(entry_state.notes);
}

void AnimeInfoEditDialog::onDialogAccepted() {
    if (this->new_state_ == this->original_state_) {
        return;
    }

    this->new_state_.updated_at = DateUtils::currentEpochTime();
    this->new_state_.pending_operation = SyncUtils::mergePendingOperation(
        this->new_state_.pending_operation,
        AnilistEntry::PendingOperation::UPDATE
    );

    this->new_state_.name = QStringLiteral("Dialog edit");
    this->anime_.entry.addState(this->new_state_);

    emit requestUpdateAnime(this->anime_);

    this->ui_->labelCoverImage->clear();
}

void AnimeInfoEditDialog::onDialogRejected() {
    this->ui_->labelCoverImage->clear();
}

