#include "gui/widgets/label.hpp"


Label::Label(QWidget *parent) : QLabel(parent) {
    this->setWordWrap(true);
}

Label::Label(
    const QString &text,
    QWidget *parent,
    Qt::WindowFlags f
) : QLabel(text, parent, f) {

    this->setWordWrap(true);
}

void Label::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        emit clicked();
    }
    QLabel::mouseReleaseEvent(event);
}

