#pragma once

#include <QLineEdit>
#include <QWidget>
#include <QKeyEvent>


class LineEdit final : public QLineEdit {
    Q_OBJECT;
public:
    explicit LineEdit(QWidget *parent = nullptr);
    ~LineEdit() = default;

private:
    void keyPressEvent(QKeyEvent *event) override;
};

