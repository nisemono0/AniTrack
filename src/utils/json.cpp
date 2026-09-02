#include "utils/json.hpp"

#include <QJsonObject>


QStringList JsonUtils::stringListFromJsonArray(const QJsonArray &json_array) {
    QStringList list;
    for (const auto &item : json_array) {
        list.append(item.toString());
    }
    return list;
}

QStringList JsonUtils::stringListFromJsonString(const QString &json_string) {
    QJsonDocument json_doc = QJsonDocument::fromJson(json_string.toUtf8());
    return JsonUtils::stringListFromJsonArray(json_doc.array());
}

QString JsonUtils::jsonStringFromStringList(const QStringList &list) {
    QJsonArray json_array;
    for (const auto &item : list) {
        json_array.append(item);
    }

    return QString::fromUtf8(
        QJsonDocument(json_array).toJson(QJsonDocument::Compact)
    );
}

