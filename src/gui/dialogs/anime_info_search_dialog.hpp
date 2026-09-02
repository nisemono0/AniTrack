#pragma once

#include "ui_anime_info_search_dialog.h"

#include "app/app_resources.hpp"

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_media.hpp"
#include "base/anilist/anilist_entry.hpp"

#include <QDialog>
#include <QWidget>


namespace Ui {
class AnimeInfoSearchDialog;
}

class AnimeInfoSearchDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit AnimeInfoSearchDialog(QWidget *parent = nullptr);
    ~AnimeInfoSearchDialog();

    void updateUserPreferences(const AnilistAccount::User &user);
    void setMedia(const AnilistMedia &media);

public slots:
    void showOrFocus();

signals:
    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

private:
    Ui::AnimeInfoSearchDialog *ui_;

    QPixmap  no_image_ = QPixmap(AppResources::Images::NoImage);

    AnilistAccount::TitleLanguage title_language_;

    AnilistMedia media_;

    void updateCoverImage();
    void updateInfo();

};

