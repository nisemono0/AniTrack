#pragma once

#include "ui_settings_dialog.h"

#include <QDialog>
#include <QWidget>


namespace Ui {
class SettingsDialog;
}

class SettingsDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit SettingsDialog(QWidget *parent = nullptr);
    ~SettingsDialog();

public slots:
    void showOrFocus();

private:
    Ui::SettingsDialog *ui_;

    void initDialog();
    void setupDialog();
    void updateDialog();

    void updateSyncSettings();
    void updateRecognitionSettings();
    void updateUiSettings();

private slots:
    void onDialogAccepted();
    void onResetClicked();
};

