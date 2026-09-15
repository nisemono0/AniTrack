#include "gui/dialogs/now_playing_popup_dialog.hpp"


NowPlayingPopupDialog::NowPlayingPopupDialog(
    const QString &header_text,
    const QString &body_text,
    PopupType popup_type,
    QWidget *parent
) : QDialog(parent),
    ui_(new Ui::NowPlayingPopupDialog) {

    this->ui_->setupUi(this);

    this->setHeadertext(header_text);
    this->setBodyText(body_text);
    this->setPopupType(popup_type);

    this->setAttribute(Qt::WA_DeleteOnClose);

    connect(this, &QDialog::rejected, this, &NowPlayingPopupDialog::popupRejected);
    connect(this, &QDialog::accepted, this, [this] {
        emit popupAccepted(this->popup_type_);
    });

}

NowPlayingPopupDialog::~NowPlayingPopupDialog() {
    delete this->ui_;
}

void NowPlayingPopupDialog::setHeadertext(const QString &header_text) {
    this->ui_->labelPlayingTitle->setText(header_text);
}

void NowPlayingPopupDialog::setBodyText(const QString &body_text) {
    this->ui_->labelMessage->setText(body_text);
}

void NowPlayingPopupDialog::setPopupType(PopupType popup_type) {
    this->popup_type_ = popup_type;
}

