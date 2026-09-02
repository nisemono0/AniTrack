#pragma once

#include "ui_set_episode_dialog.h"

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
class SetEpisodeDialog;
}

class SetEpisodeDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit SetEpisodeDialog(
        const QList<AnilistAnime> &anime_list,
        AnilistAccount::TitleLanguage title_language,
        QWidget *parent = nullptr
    );
    ~SetEpisodeDialog();

signals:
    void episodeAccepted(int episode);

private:
    Ui::SetEpisodeDialog *ui_;

    void populateDialog(const QList<AnilistAnime> &anime_list, AnilistAccount::TitleLanguage title_language);
};

