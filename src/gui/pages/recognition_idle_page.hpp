#pragma once

#include "ui_recognition_idle_widget.h"

#include <QWidget>


namespace Ui {
class RecognitionIdleWidget;
}

class RecognitionIdlePage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionIdlePage(QWidget *parent = nullptr);
    ~RecognitionIdlePage();

private:
    Ui::RecognitionIdleWidget *ui_;

    void initPage();
};

