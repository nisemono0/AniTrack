#pragma once

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QByteArray>


class Network final : public QObject {
    Q_OBJECT;
public:
    explicit Network(QObject *parent = nullptr);
    ~Network() = default;

    // Send get request to url to download an image
    // Returns a QNetworkReply pointer, so you have to connect to its signals
    QNetworkReply* getImage(const QString &url);
    // Send post request with json body to url. Sets Authorization header to auth_token if not empty
    // Returns a QNetworkReply pointer, so you have to connect to its finished signal
    QNetworkReply* postJson(const QString &url, const QByteArray &body, const QString &auth_token = {});

private:
    QNetworkAccessManager *network_manager_;

};

