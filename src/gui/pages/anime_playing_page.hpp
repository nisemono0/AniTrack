#pragma once

#include "ui_anime_playing_widget.h"

#include "base/anilist/anilist_account.hpp"

#include "base/recognition/recognized_anime.hpp"

#include <QWidget>


namespace Ui {
class AnimePlayingWidget;
}

class AnimePlayingPage final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimePlayingPage(QWidget *parent = nullptr);
    ~AnimePlayingPage();

public slots:
    void onUserUpdated(const AnilistAccount::User &user);
    void handleAnimeAddUpdateFinished(const QList<AnilistAnime> &anime_list);

    void showNoMatchPage(const QString &message);
    void showIdlePage();
    void showSelectAnimePage(const QList<RecognizedAnime> &recognized_anime, const QString &title);
    void showNowPlayingPage(const RecognizedAnime &recognized_anime, const QString &title);

    void showSearchPage(const QString &title);

signals:
    void requestQuietAnimeSearch(const QString &title);

    void requestShowAnimeInfoEditDialog(const AnilistAnime &anime, AnimeInfoEditDialog::Page page);
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

private:
    Ui::AnimePlayingWidget *ui_;

    void setupPage();

    void setupSelectAnimePage();
    void setupNowPlayingPage();
    void setupSearchPage();
};

