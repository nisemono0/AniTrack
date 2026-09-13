#pragma once

#include "ui_recognition_no_match_widget.h"

#include <QWidget>


namespace Ui {
class RecognitionNoMatchWidget;
}

class RecognitionNoMatchPage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionNoMatchPage(QWidget *parent = nullptr);
    ~RecognitionNoMatchPage();

public slots:

signals:

private:
    Ui::RecognitionNoMatchWidget *ui_;

    void setupPage();
};

