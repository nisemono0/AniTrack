#pragma once

#include "base/anilist/anilist_media.hpp"
#include "base/anilist/anilist_entry.hpp"
#include "base/anilist/anilist_account.hpp"

#include <QString>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QDate>


namespace AnilistUtils {
// Return a QDate from an anilist FuzzyDate json_object
QDate dateFromFuzzyDate(const QJsonObject &fuzzy_date_obj);
// Returns a FuzzyDate json object from a QDate
QJsonObject fuzzyDateFromDate(const QDate &date);

// Return a QList of Producers from an Edges json_array
QList<AnilistMedia::Producer> producersFromJsonArray(const QJsonArray &edges_array);
// Return a QList of Studios from an Edges json_array
QList<AnilistMedia::Studio> studiosFromJsonArray(const QJsonArray &edges_array);

// Template that returns a QString represeting a json array from a QList
// of AnilistMedia studios or producers
template<typename T>
QString jsonStringFromStudiosProducers(const QList<T> &studios_producers) {
    QJsonArray json_array;

    for (const auto &item : studios_producers) {
        QJsonObject json_obj;
        json_obj[QStringLiteral("name")] = item.name;
        json_obj[QStringLiteral("site_url")] = item.site_url;

        json_array.append(json_obj);
    }

    return QString::fromUtf8(
        QJsonDocument(json_array).toJson(QJsonDocument::Compact)
    );
}

// Template that returns a QList of AnilistMedia studios or producers from
// a json array QString
template<typename T>
QList<T> studiosProducersFromJsonString(const QString &json_string) {
    QList<T> list;

    QJsonDocument json_doc = QJsonDocument::fromJson(json_string.toUtf8());
    if (!json_doc.isArray()) {
        return list;
    }

    for (const auto &item : json_doc.array()) {
        QJsonObject json_obj = item.toObject();
        T type;
        type.name = json_obj.value(QStringLiteral("name")).toString();
        type.site_url = json_obj.value(QStringLiteral("site_url")).toString();

        list.append(type);
    }

    return list;
}

// Return formatted strings from various anime data
QString mediaFormatToPrettyString(AnilistMedia::Format format);
QString mediaSeasonToPrettyString(AnilistMedia::Season season, int season_year);
QString epochToPrettyString(qint64 epoch);
QString dateToPrettyString(const QDate &date);
QString entryScoreToPrettyString(int score, AnilistAccount::ScoreFormat format);
QString entryStatusToPrettyString(AnilistEntry::Status status);
QString rewatchToPrettyString(int rewatch);
QString mediaStatusToPrettyString(AnilistMedia::Status status);
QString animeTitleToPrettyString(const AnilistMedia::Title &title, AnilistAccount::TitleLanguage title_language);
QString mediaEpisodesToPrettyString(int episodes);

} // namespace AnilistUtils

