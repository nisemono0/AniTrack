#pragma once

#include "ui_recognition_error_widget.h"

#include <QWidget>


namespace Ui {
class RecognitionErrorWidget;
}

class RecognitionErrorPage final : public QWidget {
    Q_OBJECT;
public:
    explicit RecognitionErrorPage(QWidget *parent = nullptr);
    ~RecognitionErrorPage();

    void setDisplayedMessage(const QString &message);

private:
    Ui::RecognitionErrorWidget *ui_;

    void initPage();

};

