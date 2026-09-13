#pragma once

#include "ui_anime_playing_widget.h"

#include <QWidget>


namespace Ui {
class AnimePlayingWidget;
}

class AnimePlayingPage final : public QWidget {
    Q_OBJECT;
public:
    explicit AnimePlayingPage(QWidget *parent = nullptr);
    ~AnimePlayingPage();

public slots:

signals:

private:
    Ui::AnimePlayingWidget *ui_;

    void setupPage();
};

