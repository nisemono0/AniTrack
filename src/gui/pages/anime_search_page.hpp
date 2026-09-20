#pragma once

#include "ui_anime_search_widget.h"

#include "base/anilist/anilist_media.hpp"
#include "base/anilist/anilist_account.hpp"

#include <QWidget>


namespace Ui {
class AnimeSearchWidget;
}

class AnimeSearchPage final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeSearchPage(QWidget *parent = nullptr);
    ~AnimeSearchPage();

    void focusSearch();

public slots:
    void onSearchFinished(const QList<AnilistMedia> &media_list);
    void onUserUpdated(const AnilistAccount::User &user);

    void onFilterTextChanged(const QString &text);

    void onAnimeLoadFinished(const QList<AnilistAnime> &anime_list);
    void onAnimeAddFinished(const QList<AnilistAnime> &anime_list);

signals:
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);
    void requestShowAnimeInfoEditDialog(const AnilistMedia &media);

private:
    Ui::AnimeSearchWidget *ui_;

    void initPage();
    void setupPage();

};

