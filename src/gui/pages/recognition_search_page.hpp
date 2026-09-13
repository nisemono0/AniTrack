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

public slots:

signals:

private:
    Ui::RecognitionSearchWidget *ui_;

    void setupPage();
};
