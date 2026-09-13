#pragma once

#include "ui_recognition_now_playing_widget.h"

#include <QWidget>


namespace Ui {
class RecognitionNowPlayingWidget;
}

class RecognitionNowPlayingPage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionNowPlayingPage(QWidget *parent = nullptr);
    ~RecognitionNowPlayingPage();

public slots:

signals:

private:
    Ui::RecognitionNowPlayingWidget *ui_;

    void setupPage();
};
