#pragma once

#include <QWidget>
#include <QString>
#include <QProgressDialog>


class InfiniteProgressDialog final : public QProgressDialog {
    Q_OBJECT;
public:
    explicit InfiniteProgressDialog(
        const QString &title,
        const QString &message,
        QWidget *parent = nullptr
    );
    ~InfiniteProgressDialog() = default;
};

