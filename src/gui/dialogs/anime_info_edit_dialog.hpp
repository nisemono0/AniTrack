#pragma once

#include "ui_anime_info_edit_dialog.h"

#include "app/app_resources.hpp"

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

public slots:
    void onUserUpdated(const AnilistAccount::User &user);
    void handleAnimeAddUpdateFinished(const QList<AnilistAnime> &anime_list);

    void showOrFocus();
    void showOrFocusInfoEdit(const AnilistAnime &anime, AnimeInfoEditDialog::Page page);
    void showOrFocusAdd(const AnilistMedia &media);

signals:
    void requestUpdateAnime(const AnilistAnime &anime);
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

private:
    enum class CurrentMode {
        InfoEdit = 0,
        Add
    };

    Ui::AnimeInfoEditDialog *ui_;

    const QPixmap no_image_ = QPixmap(AppResources::Images::NoImage);

    AnilistAccount::ScoreFormat score_format_;
    AnilistAccount::TitleLanguage title_language_;

    AnilistEntry entry_;
    AnilistMedia media_;
    CurrentMode current_mode_;

    AnilistEntry::State original_state_;
    AnilistEntry::State new_state_;

    void setAnime(const AnilistAnime &anime);
    void setMedia(const AnilistMedia &media);

    void enableInfoEdit();
    void enableAdd();

    void updateCoverImage();
    void updateInfoTab();
    void updateEditTab();

private slots:
    void rejectDialog();
    void updateAnime();
    void addAnime(AnilistEntry::Status status);

};

