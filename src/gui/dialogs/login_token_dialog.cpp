#include "gui/dialogs/login_token_dialog.hpp"


LoginTokenDialog::LoginTokenDialog(QWidget *parent) :
    QDialog(parent),
    ui_(new Ui::LoginTokenDialog) {
    this->ui_->setupUi(this);

    for (auto &button : this->ui_->buttonBox->buttons()) {
        button->setFocusPolicy(Qt::NoFocus);
    }

    connect(this, &QDialog::accepted, this, [this] {
        QString auth_token = this->ui_->lineEditAccessToken->text();
        emit authTokenAccepted(auth_token);
        this->ui_->lineEditAccessToken->clear();
    });

    connect(this, &QDialog::rejected, this, [this] {
        this->ui_->lineEditAccessToken->clear();
    });
}

LoginTokenDialog::~LoginTokenDialog() {
    delete this->ui_;
}

void LoginTokenDialog::openOrFocus() {
    if (this->isVisible()) {
        this->raise();
        this->activateWindow();
        return;
    }

    this->open();
}

