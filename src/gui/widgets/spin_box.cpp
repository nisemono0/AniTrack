#include "gui/widgets/spin_box.hpp"


SpinBox::SpinBox(QWidget *parent) : QSpinBox(parent) {}

void SpinBox::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Enter ||
        event->key() == Qt::Key_Return) {
        emit returnPressed();
        return;
    }
    QSpinBox::keyPressEvent(event);
}

void SpinBox::focusOutEvent(QFocusEvent *event) {
    emit focusLost();
    QSpinBox::focusOutEvent(event);
}

