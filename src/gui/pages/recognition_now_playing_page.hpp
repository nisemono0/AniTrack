#pragma once

#include "ui_recognition_now_playing_widget.h"

#include "app/app_resources.hpp"

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include "base/recognition/recognized_anime.hpp"

#include "gui/dialogs/anime_info_edit_dialog.hpp"
#include "gui/dialogs/now_playing_popup_dialog.hpp"

#include <QWidget>
#include <QTimer>
#include <QPointer>


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

    void startPopupTimer();
    void stopPopupTimer();

signals:
    void requestShowAnimeInfoEditDialog(const AnilistAnime &anime, AnimeInfoEditDialog::Page page);
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

    void requestSetNowPlayingAnimeProgress(const AnilistAnime &anime, int progress);

    void requestShowPopupRemainingTime(const QString &message, int timeout);

private:
    Ui::RecognitionNowPlayingWidget *ui_;

    const QPixmap no_image_ = QPixmap(AppResources::Images::NoImage);

    AnilistAccount::TitleLanguage title_language_;

    QTimer *popup_timer_;
    QTimer *display_timer_;

    bool is_popup_enabled_ = true;
    bool is_popup_timer_enabled_ = true;
    QPointer<NowPlayingPopupDialog> popup_dialog_;

    QString playing_title_;
    int playing_episode_;
    QString release_group_;

    std::optional<AnilistEntry> entry_;
    AnilistMedia media_;

    void showEdit();
    void showAdd();

    void initPage();
    void setupPage();

    void updateCoverImage();
    void updateNowPlayingInfo();

    void openPopupDialog(const QString &header_text,
                         const QString &body_text,
                         NowPlayingPopupDialog::PopupType popup_type);

private slots:
    void onPopupTimerTimeout();
    void onPopupAccepted(NowPlayingPopupDialog::PopupType popup_type);

    void applySettings();
};
