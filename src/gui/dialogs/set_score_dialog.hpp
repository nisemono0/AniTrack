#pragma once

#include "ui_set_score_dialog.h"

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
class SetScoreDialog;
}

class SetScoreDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit SetScoreDialog(
        const QList<AnilistAnime> &anime_list,
        AnilistAccount::TitleLanguage title_language,
        AnilistAccount::ScoreFormat score_format,
        QWidget *parent = nullptr
    );
    ~SetScoreDialog();

signals:
    void scoreAccepted(int score);

private:
    Ui::SetScoreDialog *ui_;

    void populateDialog(const QList<AnilistAnime> &anime_list, AnilistAccount::TitleLanguage title_language);
};
