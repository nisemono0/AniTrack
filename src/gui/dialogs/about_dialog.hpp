#pragma once

#include "ui_about_dialog.h"

#include <QDialog>
#include <QWidget>


namespace Ui {
class AboutDialog;
}

class AboutDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit AboutDialog(QWidget *parent = nullptr);
    ~AboutDialog();

public slots:
    void showOrFocus();

private:
    Ui::AboutDialog *ui_;

    void setupDialog();
};

