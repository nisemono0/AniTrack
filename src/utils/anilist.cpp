#include "utils/anilist.hpp"

#include "base/anilist/anilist_keys.hpp"


QDate AnilistUtils::dateFromFuzzyDate(const QJsonObject &fuzzy_date_obj) {
    return QDate(
        fuzzy_date_obj.value(AnilistKeys::FuzzyDate::Year).toInt(),
        fuzzy_date_obj.value(AnilistKeys::FuzzyDate::Month).toInt(),
        fuzzy_date_obj.value(AnilistKeys::FuzzyDate::Day).toInt()
    );
}

QJsonObject AnilistUtils::fuzzyDateFromDate(const QDate &date) {
    QJsonObject fuzzy_date;
    fuzzy_date[QStringLiteral("day")];
    fuzzy_date[QStringLiteral("month")];
    fuzzy_date[QStringLiteral("year")];

    if (!date.isValid()) {
        return fuzzy_date;
    }

    fuzzy_date[QStringLiteral("day")] = date.day();
    fuzzy_date[QStringLiteral("month")] = date.month();
    fuzzy_date[QStringLiteral("year")] = date.year();

    return fuzzy_date;
}

QList<AnilistMedia::Producer> AnilistUtils::producersFromJsonArray(const QJsonArray &edges_array) {
    QList<AnilistMedia::Producer> producers;
    for (const auto &edge : edges_array) {
        QJsonObject edge_obj = edge.toObject();
        if (!edge_obj.value(AnilistKeys::StudioEdge::IsMain).toBool()) {
            QJsonObject studio_obj = edge_obj.value(AnilistKeys::StudioEdge::Node).toObject();
            producers.append(
                AnilistMedia::Producer{
                    studio_obj.value(AnilistKeys::Studio::Name).toString(),
                    studio_obj.value(AnilistKeys::Studio::SiteUrl).toString()
                }
            );
        }
    }
    return producers;
}

QList<AnilistMedia::Studio> AnilistUtils::studiosFromJsonArray(const QJsonArray &edges_array) {
    QList<AnilistMedia::Studio> studios;
    for (const auto &edge : edges_array) {
        QJsonObject edge_obj = edge.toObject();
        if (edge_obj.value(AnilistKeys::StudioEdge::IsMain).toBool()) {
            QJsonObject studio_obj = edge_obj.value(AnilistKeys::StudioEdge::Node).toObject();
            studios.append(
                AnilistMedia::Studio{
                    studio_obj.value(AnilistKeys::Studio::Name).toString(),
                    studio_obj.value(AnilistKeys::Studio::SiteUrl).toString()
                }
            );
        }
    }
    return studios;
}

QString AnilistUtils::mediaFormatToPrettyString(AnilistMedia::Format format) {
    switch (format) {
        case AnilistMedia::Format::INVALID:
            return QStringLiteral("Invalid");
        case AnilistMedia::Format::TV:
            return QStringLiteral("TV");
        case AnilistMedia::Format::TV_SHORT:
            return QStringLiteral("Short");
        case AnilistMedia::Format::MOVIE:
            return QStringLiteral("Movie");
        case AnilistMedia::Format::SPECIAL:
            return QStringLiteral("Special");
        case AnilistMedia::Format::OVA:
            return QStringLiteral("OVA");
        case AnilistMedia::Format::ONA:
            return QStringLiteral("ONA");
        case AnilistMedia::Format::MUSIC:
            return QStringLiteral("Music");
        default:
            return QStringLiteral("Error");
    }
}

QString AnilistUtils::mediaSeasonToPrettyString(AnilistMedia::Season season, int season_year) {
    QStringList season_format;
    switch (season) {
        case AnilistMedia::Season::INVALID: {
            season_format.append(QStringLiteral("Invalid"));
            break;
        }
        case AnilistMedia::Season::WINTER: {
            season_format.append(QStringLiteral("Winter"));
            break;
        }
        case AnilistMedia::Season::SPRING: {
            season_format.append(QStringLiteral("Spring"));
            break;
        }
        case AnilistMedia::Season::SUMMER: {
            season_format.append(QStringLiteral("Summer"));
            break;
        }
        case AnilistMedia::Season::FALL: {
            season_format.append(QStringLiteral("Fall"));
            break;
        }
    }

    if (season_year > 0) {
        season_format.append(QString::number(season_year));
    }

    return season_format.join(QChar::Space);
}

QString AnilistUtils::epochToPrettyString(qint64 epoch) {
    if (epoch <= 0) {
        return QStringLiteral("-");
    }
    return QDateTime::fromSecsSinceEpoch(epoch).toString(QStringLiteral("dd-MM-yyyy"));
}

QString AnilistUtils::dateToPrettyString(const QDate &date) {
    if (!date.isValid()) {
        return QStringLiteral("-");
    }
    return date.toString(QStringLiteral("dd-MM-yyyy"));
}

QString AnilistUtils::entryScoreToPrettyString(int score, AnilistAccount::ScoreFormat format) {
    if (score <= 0) {
        return QStringLiteral("-");
    }

    switch (format) {
        case AnilistAccount::ScoreFormat::INVALID:
            return QStringLiteral("Invalid");
        case AnilistAccount::ScoreFormat::POINT_100:
            return QString::number(score);
        case AnilistAccount::ScoreFormat::POINT_10_DECIMAL:
            return QString::number(score / 10.0f);
        case AnilistAccount::ScoreFormat::POINT_10:
            return QString::number(
                qRound(score / 10.0f)
            );
        case AnilistAccount::ScoreFormat::POINT_5:
            return QString::number(
                qRound(score / 20.0f)
            );
        case AnilistAccount::ScoreFormat::POINT_3: {
            if (score > 0 && score < 60) {
                return QStringLiteral(":(");
            }
            if (score >= 60 && score < 80) {
                return QStringLiteral(":|");
            }
            if (score >= 80 && score <= 100) {
                return QStringLiteral(":)");
            }
            return QStringLiteral("-");
        }
        default:
            return QStringLiteral("Error");
    }
}

QString AnilistUtils::entryStatusToPrettyString(AnilistEntry::Status status) {
    switch (status) {
        case AnilistEntry::Status::INVALID:
            return QStringLiteral("Invalid");
        case AnilistEntry::Status::CURRENT:
            return QStringLiteral("Watching");
        case AnilistEntry::Status::PLANNING:
            return QStringLiteral("Planning");
        case AnilistEntry::Status::COMPLETED:
            return QStringLiteral("Completed");
        case AnilistEntry::Status::DROPPED:
            return QStringLiteral("Dropped");
        case AnilistEntry::Status::PAUSED:
            return QStringLiteral("Paused");
        case AnilistEntry::Status::REPEATING:
            return QStringLiteral("Rewatching");
        default:
            return QStringLiteral("Error");
    }
}

QString AnilistUtils::rewatchToPrettyString(int rewatch) {
    if (rewatch <= 0) {
        return QStringLiteral("-");
    }
    return QString::number(rewatch);
}

QString AnilistUtils::mediaStatusToPrettyString(AnilistMedia::Status status) {
    switch (status) {
        case AnilistMedia::Status::INVALID:
            return QStringLiteral("Invalid");
        case AnilistMedia::Status::FINISHED:
            return QStringLiteral("Finished");
        case AnilistMedia::Status::RELEASING:
            return QStringLiteral("Releasing");
        case AnilistMedia::Status::NOT_YET_RELEASED:
            return QStringLiteral("Not yet released");
        case AnilistMedia::Status::CANCELLED:
            return QStringLiteral("Cancelled");
        case AnilistMedia::Status::HIATUS:
            return QStringLiteral("Hiatus");
        default:
            return QStringLiteral("Error");
    }
}

QString AnilistUtils::animeTitleToPrettyString(const AnilistMedia::Title &title, AnilistAccount::TitleLanguage title_language) {
    switch (title_language) {
        case AnilistAccount::TitleLanguage::INVALID:
            return QStringLiteral("Invalid");
        case AnilistAccount::TitleLanguage::ROMAJI:
            return title.romaji;
        case AnilistAccount::TitleLanguage::ENGLISH:
            return title.english;
        case AnilistAccount::TitleLanguage::NATIVE:
            return title.native;
        default:
            return QStringLiteral("Error");
    }
}

QString AnilistUtils::mediaEpisodesToPrettyString(int episodes) {
    if (episodes <= 0) {
        return QStringLiteral("-");
    }
    return QString::number(episodes);
}

