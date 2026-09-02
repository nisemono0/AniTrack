#pragma once

#include "ui_anime_info_edit_dialog.h"

#include "app//app_resources.hpp"

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
class AnimeInfoEditDialog;
}

class AnimeInfoEditDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit AnimeInfoEditDialog(QWidget *parent = nullptr);
    ~AnimeInfoEditDialog();

    enum class Page {
        Info = 0,
        Edit
    };

    void updateUserPreferences(const AnilistAccount::User &user);
    // Set the displayed anime
    void setAnime(const AnilistAnime &anime);

public slots:
    // Show or focus the dialog on page. Defaults to showing the Info page
    void showOrFocus(Page page);

signals:
    void requestUpdateAnime(const AnilistAnime &anime);

private:
    Ui::AnimeInfoEditDialog *ui_;

    QPixmap no_image_ = QPixmap(AppResources::Images::NoImage);

    AnilistAccount::ScoreFormat score_format_;
    AnilistAccount::TitleLanguage title_language_;

    AnilistAnime anime_;

    AnilistEntry::State original_state_;
    AnilistEntry::State new_state_;

    void updateCoverImage();
    void updateInfoTab();
    void updateEditTab();

private slots:
    void onDialogAccepted();
    void onDialogRejected();

};

