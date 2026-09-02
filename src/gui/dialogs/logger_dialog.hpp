#pragma once

#include "ui_logger_dialog.h"

#include <QDialog>
#include <QWidget>
#include <QEvent>


namespace Ui {
class LoggerDialog;
};

class LoggerDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit LoggerDialog(QWidget *parent = nullptr);
    ~LoggerDialog();

public slots:
    void showOrFocus();
    void appendLogMessage(const QString &message);

protected:
    bool eventFilter(QObject *o, QEvent *e) override;

private:
    // Ui
    Ui::LoggerDialog *ui_;

    void showDefaultStatusTip();

private slots:
    void saveLogs();

    void onBlockCountChanged(int newBlockCount);

};

