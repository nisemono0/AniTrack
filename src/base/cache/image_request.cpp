#include "base/cache/image_request.hpp"


ImageRequest::ImageRequest(QObject *parent) : QObject(parent) {}

void ImageRequest::sendFinish(QPixmap pixmap) {
    emit finished(pixmap);
    this->deleteLater();
}

void ImageRequest::sendFail() {
    emit failed();
    this->deleteLater();
}

