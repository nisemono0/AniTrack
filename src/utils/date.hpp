#pragma once

#include <QString>
#include <QDate>


namespace DateUtils {
// Convert a QDate to a QString ISODate
QString stringFromDate(const QDate &date);
// Convert QString ISODate to QDate
QDate dateFromString(const QString &date_string);
// Return a QDate from epoch
QDate dateFromEpoch(qint64 epoch);
// Return the current epoch
qint64 currentEpochTime();
// Return the current QDate
QDate currentDate();
} // namespace DateUtils
