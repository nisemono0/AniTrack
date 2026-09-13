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
    void requestQuietAnimeSearch(const QString &title);
    void requestAnimeSearchById(const QList<int> &media_ids);

signals:
    void searchProgressStarted(const QString &title, const QString &message);
    void searchProgressFinished();

    void searchFinished(const QList<AnilistMedia> &media_list);
    void quietSearchFinished(const QList<AnilistMedia> &media_list);
    void idSearchFinished(const QList<AnilistMedia> &media_list);

    void searchFailed(const QString &title, const QString &message);
    void quietSearchFailed(const QString &message);
    void idSearchFailed(const QString &message);

private:
    enum class SearchType {
        None,
        Normal,
        Quiet,
        Id
    };
    AnilistApi *anilist_api_;

    bool search_in_progress_ = false;
    SearchType search_type_ = SearchType::None;

    void resetSearch();

private slots:
    void onSearchAnimeFailed(const QString &message);

    void onSearchAnimeFinished(const QList<AnilistMedia> &media_list);
};

