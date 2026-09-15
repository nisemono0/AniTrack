#pragma once

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>


class Label final : public QLabel {
    Q_OBJECT;
public:
    explicit Label(QWidget *parent = nullptr);
    explicit Label(
        const QString &text,
        QWidget *parent = nullptr,
        Qt::WindowFlags f = Qt::WindowFlags()
    );
    ~Label() = default;

signals:
    void clicked();

protected:
    void mouseReleaseEvent(QMouseEvent *event) override;
};

