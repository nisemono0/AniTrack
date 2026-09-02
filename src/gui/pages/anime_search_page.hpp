#pragma once

#include "ui_anime_search_widget.h"

#include "base/anilist/anilist_media.hpp"
#include "base/anilist/anilist_account.hpp"

#include "gui/dialogs/anime_info_search_dialog.hpp"

#include <QWidget>


namespace Ui {
class AnimeSearchWidget;
}

class AnimeSearchPage final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimeSearchPage(QWidget *parent = nullptr);
    ~AnimeSearchPage();

public slots:
    void onSearchFinished(const QList<AnilistMedia> &media_list);
    void onUserUpdated(const AnilistAccount::User &user);

    void onFilterTextChanged(const QString &text);

    void onAnimeLoadFinished(const QList<AnilistAnime> &anime_list);
    void onMediaAddFinished(const QList<AnilistMedia> &media_list);

    void onRequestShowAnimeInfoSearchDialog(const AnilistMedia &media);

signals:
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

private:
    Ui::AnimeSearchWidget *ui_;

    AnimeInfoSearchDialog *info_search_dialog_;

    void initPage();
    void setupPage();
    void setupInfoDialog();

};

