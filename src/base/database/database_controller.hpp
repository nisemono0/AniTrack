#pragma once

#include "base/anilist/anilist_anime.hpp"

#include "base/database/database.hpp"

#include <QObject>


class DatabaseController final : public QObject {
    Q_OBJECT;
public:
    explicit DatabaseController(
        Database *database,
        QObject *parent = nullptr
    );
    ~DatabaseController() = default;

    void initialLoad();

public slots:
    void requestLoadAnime();

    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

    void requestUpdateAnime(const AnilistAnime &anime);

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

    void requestSetAnimeScore(const QList<AnilistAnime> &anime_list, int score);

    void requestSetAnimeStatus(const QList<AnilistAnime> &anime_list, AnilistEntry::Status status);

    void requestRestoreAnimeState(const AnilistAnime &anime, int index);

    void requestCleanupUnusedMedia();
    void requestVacuumDatabase();
    void requestRecreateDatabase();

signals:
    void errorOccurred(const QString &title, const QString &message);
    void infoOccurred(const QString &title, const QString &message);

    void databasePathChanged(const QString &database_path);

    void animeAddFinished(const QList<AnilistAnime> &anime_list);
    void mediaAddFinished(const QList<AnilistMedia> &media_list);

    void animeLoadFinished(const QList<AnilistAnime> &anime_list);

    void animeUpdateFinished(const QList<AnilistAnime> &anime_list);
    void animeDeleteFinished(const QList<int> &local_ids);

    void animeCountChanged(int count);

private:
    Database *database_;

    void upsertEntriesAndNotify(const QList<AnilistAnime> &anime_list, const QString &title, const QString &message);
    void countEntriesAndNotifiy();

};

