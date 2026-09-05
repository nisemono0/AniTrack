#include "gui/widgets/line_edit.hpp"


LineEdit::LineEdit(QWidget *parent) : QLineEdit(parent) {}

void LineEdit::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_Escape) {
        this->clearFocus();
        return;
    }
    QLineEdit::keyPressEvent(event);
}

