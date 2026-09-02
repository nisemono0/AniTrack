#pragma once

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_media.hpp"

#include "gui/views/anime_search_header.hpp"

#include "gui/models/anime_search_model.hpp"
#include "gui/models/anime_search_proxy.hpp"

#include "gui/delegates/anime_search_adult_delegate.hpp"
#include "gui/delegates/anime_search_in_list_delegate.hpp"

#include <QTreeView>
#include <QWidget>
#include <QWheelEvent>


class AnimeSearchView final : public QTreeView {
    Q_OBJECT;
public:
    explicit AnimeSearchView(QWidget *parent = nullptr);
    ~AnimeSearchView() = default;

    void updateUserPreferences(const AnilistAccount::User &user);

    void setExistingMediaIds(const QList<AnilistAnime> &anime_list);
    void setMedia(const QList<AnilistMedia> &media_list);
    void updateAddedMedia(const QList<AnilistMedia> &media_list);

    void setFilterText(const QString &text);

signals:
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);
    void requestShowAnimeInfoSearchDialog(const AnilistMedia &media);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    AnilistAccount::ScoreFormat score_format_;
    AnilistAccount::TitleLanguage title_language_;

    AnimeSearchModel *anime_search_model_;
    AnimeSearchProxy *anime_search_proxy_;
    AnimeSearchHeader *anime_search_header_;

    AnimeSearchAdultDelegate *adult_delegate_;
    AnimeSearchInListDelegate *in_list_delegate_;

    void initView();

    void setupModel();
    void setupDelegates();
    void setupHeader();

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);

};

