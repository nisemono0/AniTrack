#pragma once

#include "ui_now_playing_popup_dialog.h"

#include <QDialog>
#include <QWidget>


namespace Ui {
class NowPlayingPopupDialog;
}

class NowPlayingPopupDialog final : public QDialog {
    Q_OBJECT;
public:
    enum class PopupType {
        Add,
        Update,
        Complete
    };
    explicit NowPlayingPopupDialog(
        const QString &header_text,
        const QString &body_text,
        PopupType popup_type,
        QWidget *parent = nullptr
    );
    ~NowPlayingPopupDialog();

    void setHeadertext(const QString &header_text);
    void setBodyText(const QString &body_text);
    void setPopupType(PopupType popup_type);

signals:
    void popupAccepted(NowPlayingPopupDialog::PopupType popup_type);
    void popupRejected();

private:
    Ui::NowPlayingPopupDialog *ui_;

    PopupType popup_type_;
};

