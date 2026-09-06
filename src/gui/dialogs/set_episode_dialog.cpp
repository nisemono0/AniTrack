#include "gui/dialogs/set_episode_dialog.hpp"

#include "utils/anilist.hpp"


SetEpisodeDialog::SetEpisodeDialog(
    const QList<AnilistAnime> &anime_list,
    AnilistAccount::TitleLanguage title_language,
    QWidget *parent
) : QDialog(parent),
    ui_(new Ui::SetEpisodeDialog) {

    this->ui_->setupUi(this);

    this->populateDialog(anime_list, title_language);

    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(this, &QDialog::accepted, this, [this] {
        emit episodeAccepted(this->ui_->spinBoxEpisodeNumber->value());
    });
}

SetEpisodeDialog::~SetEpisodeDialog() {
    delete this->ui_;
}

void SetEpisodeDialog::populateDialog(const QList<AnilistAnime> &anime_list, AnilistAccount::TitleLanguage title_language) {
    if (anime_list.size() == 1) {
        const int entry_progress = anime_list.constFirst().entry.state().progress;
        const int episodes_number = anime_list.constFirst().media.episodes;
        if (episodes_number > 0) {
            this->ui_->spinBoxEpisodeNumber->setMaximum(episodes_number);
            this->ui_->spinBoxEpisodeNumber->setValue(entry_progress);
        } else {
            this->ui_->spinBoxEpisodeNumber->setMaximum(INT_MAX);
        }
    }

    for (const auto &anime : anime_list) {
        this->ui_->plainTextEditAnime->appendPlainText(
            QStringLiteral("• %1").arg(
                AnilistUtils::animeTitleToPrettyString(anime.media.title, title_language)
            )
        );
    }
    this->ui_->plainTextEditAnime->moveCursor(QTextCursor::Start);
}

