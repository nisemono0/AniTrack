#pragma once

#include "ui_anime_list_widget.h"

#include "base/anilist/anilist_account.hpp"

#include <QWidget>


namespace Ui {
class AnimeListWidget;
}

class AnimeListPage final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeListPage(QWidget *parent = nullptr);
    ~AnimeListPage();

    void selectNextTab();
    void selectPreviousTab();

public slots:
    void onUserUpdated(const AnilistAccount::User &user);

    void onFilterTextChanged(const QString &text);

    void onAnimeLoadFinished(const QList<AnilistAnime> &anime_list);

    void onAnimeUpdateFinished(const QList<AnilistAnime> &anime_list);
    void onAnimeAddFinished(const QList<AnilistAnime> &anime_list);

signals:
    void requestIncreaseAnimeProgress(const QList<AnilistAnime> &anime_list);
    void requestDecreaseAnimeProgress(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeProgress(const QList<AnilistAnime> &anime_list, int progress);
 
    void requestUndoAnimeState(const QList<AnilistAnime> &anime_list);
    void requestRedoAnimeState(const QList<AnilistAnime> &anime_list);
 
    void requestMarkAnimeForRemoval(const QList<AnilistAnime> &anime_list);

    void requestClearAnimeStartedDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeStartedDateFromMediaStartDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeStartedDate(const QList<AnilistAnime> &anime_list, const QDate &date);

    void requestClearAnimeCompletedDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeCompletedDateFromLastUpdated(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeCompletedDateFromMediaEndDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeCompletedDate(const QList<AnilistAnime> &anime_list, const QDate &date);

    void requestSetAnimeStatus(const QList<AnilistAnime> &anime_list, AnilistEntry::Status status);

    void requestSetAnimeEpisode(const QList<AnilistAnime> &anime_list, int progress);

    void requestSetAnimeScore(const QList<AnilistAnime> &anime_list, int score);

    void requestRestoreAnimeState(const AnilistAnime &anime, int index);

    void requestUpdateAnime(const AnilistAnime &anime);

    void requestShowAnimeInfoEditDialog(const AnilistAnime &anime, AnimeInfoEditDialog::Page page);

private:
    Ui::AnimeListWidget *ui_;

    void initPage();

    void setupTabBar();
    void setupPage();

private slots:
    void onCurrentTabChanged(AnimeListTabBar::Tab tab);
};

