#pragma once

#include "ui_recognition_now_playing_widget.h"

#include "app/app_resources.hpp"

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include "base/recognition/recognized_anime.hpp"

#include "gui/dialogs/anime_info_edit_dialog.hpp"

#include <QWidget>


namespace Ui {
class RecognitionNowPlayingWidget;
}

class RecognitionNowPlayingPage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionNowPlayingPage(QWidget *parent = nullptr);
    ~RecognitionNowPlayingPage();

    void updateUserPreferences(const AnilistAccount::User &user);
    void handleAnimeAddUpdateFinished(const QList<AnilistAnime> &anime_list);

    void setNowPlayingAnime(const RecognizedAnime &recognized_anime, const QString &title);

public slots:

signals:
    void requestShowAnimeInfoEditDialog(const AnilistAnime &anime, AnimeInfoEditDialog::Page page);
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

private:
    Ui::RecognitionNowPlayingWidget *ui_;

    const QPixmap no_image_ = QPixmap(AppResources::Images::NoImage);

    AnilistAccount::TitleLanguage title_language_;

    QString playing_title_;
    int episode_;

    AnilistEntry entry_;
    AnilistMedia media_;

    void showEdit();
    void showAdd();

    void initPage();
    void setupPage();

    void updateCoverImage();
    void updateNowPlayingInfo();

private slots:

};
