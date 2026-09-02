#pragma once

#include <QObject>
#include <QPixmap>


class ImageRequest final : public QObject {
    Q_OBJECT;
public:
    explicit ImageRequest(QObject *parent = nullptr);
    ~ImageRequest() = default;

    void sendFinish(QPixmap pixmap);
    void sendFail();

signals:
    void finished(QPixmap pixmap);
    void failed();
};

