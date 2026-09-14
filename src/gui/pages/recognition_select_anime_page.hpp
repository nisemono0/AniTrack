#pragma once

#include "ui_recognition_select_anime_widget.h"

#include <QWidget>


namespace Ui {
class RecognitionSelectAnimeWidget;
}

class RecognitionSelectAnimePage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionSelectAnimePage(QWidget *parent = nullptr);
    ~RecognitionSelectAnimePage();

public slots:

signals:

private:
    Ui::RecognitionSelectAnimeWidget *ui_;

    void initPage();
};
