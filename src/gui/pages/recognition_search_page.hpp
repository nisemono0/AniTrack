#pragma once

#include "ui_recognition_search_widget.h"

#include <QWidget>


namespace Ui {
class RecognitionSearchWidget;
}

class RecognitionSearchPage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionSearchPage(QWidget *parent = nullptr);
    ~RecognitionSearchPage();

    void setPlayingTitle(const QString &title);

signals:
    void requestQuietAnimeSearch(const QString &title);

private:
    Ui::RecognitionSearchWidget *ui_;

    QString playing_title_;

    void initPage();
    void setupPage();

private slots:
    void requestQuietSearch();

};
