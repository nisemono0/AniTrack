#include "gui/dialogs/set_score_dialog.hpp"

#include "utils/anilist.hpp"


SetScoreDialog::SetScoreDialog(
    const QList<AnilistAnime> &anime_list,
    AnilistAccount::TitleLanguage title_language,
    AnilistAccount::ScoreFormat score_format,
    QWidget *parent
) : QDialog(parent),
    ui_(new Ui::SetScoreDialog) {

    this->ui_->setupUi(this);

    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(this->ui_->spinBoxScore, &QSpinBox::valueChanged, this, [this, score_format] (int value) {
        this->ui_->labelPrettyScore->setText(
            AnilistUtils::entryScoreToPrettyString(value, score_format)
        );
    });

    this->populateDialog(anime_list, title_language);

    connect(this, &QDialog::accepted, this, [this] {
        emit scoreAccepted(this->ui_->spinBoxScore->value());
    });
}

SetScoreDialog::~SetScoreDialog() {
    delete this->ui_;
}

void SetScoreDialog::populateDialog(const QList<AnilistAnime> &anime_list, AnilistAccount::TitleLanguage title_language) {
    if (anime_list.size() == 1) {
        this->ui_->spinBoxScore->setValue(anime_list.constFirst().entry.state().score);
    }

    for (const auto &anime : anime_list) {
        this->ui_->plainTextEditAnime->appendPlainText(
            QStringLiteral("- %1").arg(
                AnilistUtils::animeTitleToPrettyString(anime.media.title, title_language)
            )
        );
    }
}

