#pragma once

#include "base/anilist/anilist_api.hpp"

#include <QObject>


class SearchManager final : public QObject {
    Q_OBJECT;
public:
    explicit SearchManager(
        AnilistApi *anilist_api,
        QObject *parent = nullptr
    );
    ~SearchManager() = default;

public slots:
    void requestAnimeSearch(const QString &title);

signals:
    void searchProgressStarted(const QString &title, const QString &message);
    void searchProgressFinished();

    void searchFinished(const QList<AnilistMedia> &media_list);

    void searchFailed(const QString &title, const QString &message);

private:
    AnilistApi *anilist_api_;

    bool search_in_progress_ = false;

    void startSearch(const QString &title);
    void finishSearch();

private slots:
    void onSearchAnimeFailed(const QString &message);
    void onSearchAnimeFinished(const QList<AnilistMedia> &media);

};

