#include "gui/widgets/progress_dialog.hpp"


ProgressDialog::ProgressDialog(
    const QString &title,
    const QString &message,
    int minimum,
    int maximum,
    QWidget *parent
) : QProgressDialog(parent) {

    this->setWindowTitle(title);
    this->setLabelText(message);

    this->setRange(minimum, maximum);

    this->setCancelButton(nullptr);
    this->setWindowFlag(Qt::WindowCloseButtonHint, false);

    this->setWindowFlag(Qt::FramelessWindowHint);
    this->setWindowFlag(Qt::WindowStaysOnTopHint);

    this->setAttribute(Qt::WA_DeleteOnClose);
}
