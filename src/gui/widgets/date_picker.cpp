#include "gui/widgets/date_picker.hpp"

#include <QHBoxLayout>
#include <QVBoxLayout>


DatePicker::DatePicker(QWidget *parent) : QWidget(parent) {
    this->setupDatePopup();
    this->setupDateDisplay();
    this->setupLayout();

    connect(this->calendar_widget_, &QCalendarWidget::clicked, this, &DatePicker::onDateClicked);
    connect(this->clear_button_, &QPushButton::clicked, this, [this] {
        this->setDate(QDate());
        this->date_popup_->hide();
    });
    connect(this->current_date_button_, &QPushButton::clicked, this, [this] {
        this->setDate(QDate::currentDate());
        this->date_popup_->hide();
    });
}

void DatePicker::setDisplayFormat(const QString &format) {
    this->display_format_ = format;
    this->updateDateDisplay();
}

void DatePicker::setTextAlignment(Qt::Alignment alignment) {
    this->date_display_->setAlignment(alignment);
}

void DatePicker::setDate(QDate date) {
    if (date == this->date_) {
        return;
    }

    this->date_ = date;

    if (date.isValid()) {
        this->calendar_widget_->setSelectedDate(date);
    }

    this->updateDateDisplay();
    emit dateChanged(date);
}

QDate DatePicker::date() const {
    return this->date_;
}

void DatePicker::showPopup() {
    if (this->date_popup_->isVisible()) {
        return;
    }
    QPoint pos = this->date_display_->mapToGlobal(
        QPoint(0, this->date_display_->height())
    );

    this->date_popup_->move(pos);
    this->date_popup_->show();
}

void DatePicker::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        this->showPopup();
    }
    QWidget::mousePressEvent(event);
}

void DatePicker::updateDateDisplay() {
    if (this->date_.isValid()) {
        this->date_display_->setText(
            this->date_.toString(this->display_format_)
        );
    } else {
        this->date_display_->setText(
            QStringLiteral("-")
        );
    }
}

void DatePicker::setupDatePopup() {
    this->date_popup_ = new QWidget(this);
    this->date_popup_->setContentsMargins(0, 0, 0, 0);
    this->date_popup_->setWindowFlags(Qt::Popup);
    this->date_popup_->hide();

    this->clear_button_ = new QPushButton(
        QStringLiteral("Clear date"),
        this->date_popup_
    );
    this->clear_button_->setFocusPolicy(Qt::NoFocus);

    this->current_date_button_ = new QPushButton(
        QStringLiteral("Current date"),
        this->date_popup_
    );
    this->current_date_button_->setFocusPolicy(Qt::NoFocus);

    this->calendar_widget_ = new QCalendarWidget(this->date_popup_);
    this->calendar_widget_->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    this->calendar_widget_->setGridVisible(true);
}

void DatePicker::setupDateDisplay() {
    this->date_display_ = new QLineEdit(this);
    this->date_display_->setReadOnly(true);
    this->date_display_->setFocusPolicy(Qt::NoFocus);
    this->date_display_->setAttribute(Qt::WA_TransparentForMouseEvents);

    this->updateDateDisplay();
}

void DatePicker::setupLayout() {
    this->setContentsMargins(0, 0, 0, 0);

    auto *popup_layout = new QVBoxLayout(this->date_popup_);
    popup_layout->setContentsMargins(0, 0, 0, 0);
    popup_layout->addWidget(this->calendar_widget_);
    popup_layout->addWidget(this->clear_button_);
    popup_layout->addWidget(this->current_date_button_);

    auto *main_layout = new QHBoxLayout(this);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->addWidget(this->date_display_);
}

void DatePicker::onDateClicked(QDate date) {
    this->date_popup_->hide();
    this->setDate(date);
}

