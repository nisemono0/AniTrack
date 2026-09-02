#pragma once

#include <QWidget>
#include <QLineEdit>
#include <QCalendarWidget>
#include <QPushButton>
#include <QMouseEvent>


class DatePicker final : public QWidget {
    Q_OBJECT;
public:
    explicit DatePicker(QWidget *parent = nullptr);
    ~DatePicker() = default;

    // Set the date display format
    void setDisplayFormat(const QString &format);
    // Set the text alignment for the displayed date
    void setTextAlignment(Qt::Alignment alignment);
    // Set date date
    void setDate(QDate date);
    // Return the set date
    QDate date() const;

    // Show the date picker popup
    void showPopup();

signals:
    void dateChanged(QDate date);

protected:
    void mousePressEvent(QMouseEvent *event) override;

private:
    QLineEdit *date_display_;
    QWidget *date_popup_;
    QPushButton *clear_button_;
    QCalendarWidget *calendar_widget_;

    QDate date_;

    QString display_format_ =  QStringLiteral("dd-MM-yyyy");

    void updateDateDisplay();

    void setupDatePopup();
    void setupDateDisplay();
    void setupLayout();

private slots:
    void onDateClicked(QDate date);

};

