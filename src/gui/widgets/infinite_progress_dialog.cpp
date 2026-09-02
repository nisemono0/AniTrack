#include "gui/widgets/infinite_progress_dialog.hpp"


InfiniteProgressDialog::InfiniteProgressDialog(
    const QString &title,
    const QString &message,
    QWidget *parent
) : QProgressDialog(parent) {

    this->setRange(0, 0);

    this->setWindowTitle(title);
    this->setLabelText(message);

    this->setCancelButton(nullptr);
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);
    this->setWindowFlag(Qt::FramelessWindowHint, true);
    this->setWindowFlag(Qt::WindowStaysOnTopHint, true);

    this->setAttribute(Qt::WA_DeleteOnClose);
}

