#pragma once

#include "base/anilist/anilist_api.hpp"
#include "base/database/database.hpp"

#include <QObject>


class SyncManager final : public QObject {
    Q_OBJECT;
public:
    explicit SyncManager(
        AnilistApi *anilist_api,
        Database *database,
        QObject *parent = nullptr
    );
    ~SyncManager() = default;

public slots:
    void requestSync();

signals:
    void syncFinished();

    void syncProgressStarted(const QString &title, const QString &message, int minimum, int maximum);
    void syncProgressUpdated(int progress);
    void syncProgressFinished();

    void listFetchStarted(const QString &title, const QString &message);
    void listFetchFinished();

    void syncFailed(const QString &title, const QString &message);

private:
    AnilistApi *anilist_api_;
    Database *database_;

    QList<AnilistAnime> pending_anime_;
    int current_anime_ = 0;

    bool sync_in_progress_ = false;

    void startSync();
    void resetSync();

    void finishSync();
    void failSync(const QString &message);

    void startPendingSync();

    void processNextEntry();

    void resolveLocalToRemote(const QList<AnilistAnime> &local_anime,
                              const QList<AnilistAnime> &remote_anime,
                              QList<AnilistAnime> &upserts,
                              QList<int> &deletions);
    void resolveRemoteToLocal(const QList<AnilistAnime> &local_anime,
                              const QList<AnilistAnime> &remote_anime,
                              QList<AnilistAnime> &upserts);

    void resolveMissingAnime(const AnilistAnime &local, QList<AnilistAnime> &upserts, QList<int> &deletions);
    void resolveExistingAnime(const AnilistAnime &local, const AnilistAnime &remote, QList<AnilistAnime> &upserts);

private slots:
    void handleApiFailure(const QString &message);

    void onFetchListFinished(const QList<AnilistAnime> &anime_list);

    void onAddAnimeFinished(const AnilistAnime &anime);
    void onUpdateAnimeFinished(const AnilistAnime &anime);
    void onDeleteAnimeFinished(int local_id);

};

