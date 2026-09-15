#include "gui/widgets/label.hpp"

#include <QRect>

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

void Label::mouseMoveEvent(QMouseEvent *event) {
    if (event->buttons().testAnyFlags(Qt::LeftButton)) {
        this->pressed_ = this->rect().contains(
            event->position().toPoint()
        );
    }
    QLabel::mouseMoveEvent(event);
}

void Label::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->pressed_ = true;
    }
    QLabel::mousePressEvent(event);
}

void Label::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (this->pressed_) {
            emit clicked();
        }
        this->pressed_ = false;
    }
    QLabel::mouseReleaseEvent(event);
}

