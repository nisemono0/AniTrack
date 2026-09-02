#pragma once

#include "ui_login_token_dialog.h"

#include <QDialog>
#include <QWidget>


namespace Ui {
class LoginTokenDialog;
}

class LoginTokenDialog final : public QDialog {
    Q_OBJECT;
public:
    explicit LoginTokenDialog(QWidget *parent = nullptr);
    ~LoginTokenDialog();

public slots:
    void openOrFocus();

signals:
    void authTokenAccepted(const QString &auth_token);

private:
    Ui::LoginTokenDialog *ui_;

};


