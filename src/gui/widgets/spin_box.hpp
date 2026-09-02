#pragma once

#include <QWidget>
#include <QSpinBox>
#include <QKeyEvent>


class SpinBox final : public QSpinBox {
    Q_OBJECT;
public:
    explicit SpinBox(QWidget *parent = nullptr);
    ~SpinBox() = default;

signals:
    // Signals when the enter key is pressed
    void returnPressed();
    // Signals when the focus of the spinbox is lost
    void focusLost();

protected:
    void keyPressEvent(QKeyEvent *event) override;
    void focusOutEvent(QFocusEvent *event) override;

};

