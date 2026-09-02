#pragma once

#include <QString>
#include <QStringList>
#include <QList>
#include <QDate>
#include <QJsonObject>
#include <QSqlQuery>


struct AnilistMedia final {
    Q_GADGET;

public:
    static AnilistMedia fromResponseJson(const QJsonObject &json_obj);
    static AnilistMedia fromDatabaseQuery(const QSqlQuery &query);

    // The enums names MUST be exactly as the ones the anilist api returns
    enum class Status {
        INVALID = 0,
        FINISHED,
        RELEASING,
        NOT_YET_RELEASED,
        CANCELLED,
        HIATUS
    };
    Q_ENUM(Status);

    // The enums names MUST be exactly as the ones the anilist api returns
    struct Title final {
        QString english;
        QString native;
        QString romaji;
    };
    Q_ENUM(Title);

    // The enums names MUST be exactly as the ones the anilist api returns
    enum class Season {
        INVALID = 0,
        WINTER,
        SPRING,
        SUMMER,
        FALL
    };
    Q_ENUM(Season);

    // The enums names MUST be exactly as the ones the anilist api returns
    enum class Format {
        INVALID = 0,
        TV,
        TV_SHORT,
        MOVIE,
        SPECIAL,
        OVA,
        ONA,
        MUSIC
    };
    Q_ENUM(Format);

    struct Studio final {
        QString name;
        QString site_url;
    };

    struct Producer final {
        QString name;
        QString site_url;
    };

    int id = 0;
    bool is_adult = false;
    Status status = Status::INVALID;
    Season season = Season::INVALID;
    int season_year = 0;
    int average_score = 0;
    QString site_url;
    Title title;
    QStringList synonyms;
    Format format = Format::INVALID;
    QString description;
    QDate start_date;
    QDate end_date;
    qint64 updated_at = 0;
    int episodes = 0;
    QString country_of_origin;
    QString cover_url;
    QStringList genres;
    QList<Studio> studios;
    QList<Producer> producers;
    bool in_list;

};

