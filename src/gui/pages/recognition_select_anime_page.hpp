#pragma once

#include "ui_recognition_select_anime_widget.h"

#include "base/anilist/anilist_account.hpp"

#include "base/recognition/recognized_anime.hpp"

#include <QWidget>
#include <QLabel>


namespace Ui {
class RecognitionSelectAnimeWidget;
}

class RecognitionSelectAnimePage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionSelectAnimePage(QWidget *parent = nullptr);
    ~RecognitionSelectAnimePage();

    void updateUserPreferences(const AnilistAccount::User &user);

    void setAnimeSelection(const QList<RecognizedAnime> &recognized_anime);
    void setPlayingAnime(const QString &title, int episode);

signals:
    void recognizedAnimeSelected(const RecognizedAnime &recognized_anime);
    void requestQuietAnimeSearch(const QString &title);

private:
    Ui::RecognitionSelectAnimeWidget *ui_;

    AnilistAccount::TitleLanguage title_language_;
    QString playing_title_;

    QList<RecognizedAnime> recognized_anime_;

    void clearSelection();
    void updateSelection();

    void initPage();
    void setupPage();

private slots:
    void searchAnime();

};
