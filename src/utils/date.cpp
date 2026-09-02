#include "utils/date.hpp"

#include <QDateTime>


QString DateUtils::stringFromDate(const QDate &date) {
    if (!date.isValid()) {
        return {};
    }
    return date.toString(Qt::ISODate);
}

QDate DateUtils::dateFromString(const QString &date_string) {
    if (date_string.isEmpty()) {
        return {};
    }
    return QDate::fromString(date_string, Qt::ISODate);
}

QDate DateUtils::dateFromEpoch(qint64 epoch) {
    return QDateTime::fromSecsSinceEpoch(epoch).date();
}

qint64 DateUtils::currentEpochTime() {
    return QDateTime::currentSecsSinceEpoch();
}

QDate DateUtils::currentDate() {
    return QDate::currentDate();
}

