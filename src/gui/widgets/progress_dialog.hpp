#pragma once

#include <QProgressDialog>
#include <QWidget>

class ProgressDialog final : public QProgressDialog {
    Q_OBJECT;
public:
    explicit ProgressDialog(
        const QString &title,
        const QString &message,
        int minimum,
        int maximum,
        QWidget *parent = nullptr
    );
    ~ProgressDialog() = default;
};

