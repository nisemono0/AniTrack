#include "base/network/network.hpp"

Network::Network(QObject *parent) : QObject(parent) {
    this->network_manager_ = new QNetworkAccessManager(this);
}

QNetworkReply* Network::getImage(const QString &url) {
    QNetworkRequest request(url);

    request.setRawHeader(
        QByteArray("Accept"),
        QByteArray("image/*")
    );

    return this->network_manager_->get(request);
}

QNetworkReply* Network::postJson(const QString &url, const QByteArray &body, const QString &auth_token) {
    QNetworkRequest request(url);

    request.setHeader(
        QNetworkRequest::ContentTypeHeader,
        QStringLiteral("application/json")
    );
    request.setRawHeader(
        QByteArray("Accept"),
        QByteArray("application/json")
    );

    if (!auth_token.isEmpty()) {
        request.setRawHeader(
            QByteArray("Authorization"),
            QStringLiteral("Bearer %1").arg(auth_token).toUtf8()
        );
    }

    return this->network_manager_->post(request, body);
}
