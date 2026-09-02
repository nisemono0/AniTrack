#pragma once

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include "base/network/network.hpp"

#include <QObject>


class AnilistApi final : public QObject {
    Q_OBJECT;
public:
    explicit AnilistApi(
        Network *network,
        AnilistAccount *anilist_account,
        QObject *parent = nullptr
    );
    ~AnilistApi() = default;

    void fetchUser();
    void fetchList();

    void addAnime(const AnilistAnime &anime);
    void updateAnime(const AnilistAnime &anime);

    void deleteAnime(const AnilistAnime &anime);

    void searchAnime(const QString &title);

signals:
    void userFetchFailed(const QString &message);
    void userFetchFinished(const AnilistAccount::User &user);

    void fetchListFailed(const QString &message);
    void fetchListFinished(const QList<AnilistAnime> &anime_list);

    void addAnimeFailed(const QString &message);
    void addAnimeFinished(const AnilistAnime &anime_list);

    void updateAnimeFailed(const QString &message);
    void updateAnimeFinished(const AnilistAnime &anime_list);

    void deleteAnimeFailed(const QString &message);
    void deleteAnimeFinished(int local_id);

    void searchAnimeFailed(const QString &message);
    void searchAnimeFinished(const QList<AnilistMedia> &media);

private:
    Network *network_;
    AnilistAccount *anilist_account_;
};


