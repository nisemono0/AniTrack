#pragma once

#include <QString>
#include <QStringList>
#include <QJsonArray>


namespace JsonUtils {
// Return a QStringList from a json_array
QStringList stringListFromJsonArray(const QJsonArray &json_array);
// Return a QStringList from a json QString representing a json array
QStringList stringListFromJsonString(const QString &json_string);
// Return a QString representing a json array from a QStringList
QString jsonStringFromStringList(const QStringList &list);
} // namespace JsonUtils

