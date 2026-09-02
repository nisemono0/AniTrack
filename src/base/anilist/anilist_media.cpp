#include "base/anilist/anilist_media.hpp"
#include "base/anilist/anilist_keys.hpp"

#include "base/database/database_columns.hpp"

#include "utils/anilist.hpp"
#include "utils/json.hpp"
#include "utils/enums.hpp"
#include "utils/date.hpp"


AnilistMedia AnilistMedia::fromResponseJson(const QJsonObject &json_obj) {
    AnilistMedia media;

    media.id = json_obj.value(AnilistKeys::Media::Id).toInt();
    media.is_adult = json_obj.value(AnilistKeys::Media::IsAdult).toBool();
    media.status = EnumsUtils::enumFromString(
        json_obj.value(AnilistKeys::Media::Status).toString(),
        AnilistMedia::Status::INVALID
    );
    media.season = EnumsUtils::enumFromString(
        json_obj.value(AnilistKeys::Media::Season).toString(),
        AnilistMedia::Season::INVALID
    );
    media.season_year = json_obj.value(AnilistKeys::Media::SeasonYear).toInt();
    media.average_score = json_obj.value(AnilistKeys::Media::AverageScore).toInt();
    media.site_url = json_obj.value(AnilistKeys::Media::SiteUrl).toString();

    QJsonObject titles_obj = json_obj.value(AnilistKeys::Media::Title).toObject();
    media.title = AnilistMedia::Title{
        titles_obj.value(AnilistKeys::MediaTitle::English).toString(),
        titles_obj.value(AnilistKeys::MediaTitle::Native).toString(),
        titles_obj.value(AnilistKeys::MediaTitle::Romaji).toString()
    };

    media.synonyms = JsonUtils::stringListFromJsonArray(
        json_obj.value(AnilistKeys::Media::Synonyms).toArray()
    );
    media.format = EnumsUtils::enumFromString(
        json_obj.value(AnilistKeys::Media::Format).toString(),
        AnilistMedia::Format::INVALID
    );
    media.description = json_obj.value(AnilistKeys::Media::Description).toString();
    media.start_date = AnilistUtils::dateFromFuzzyDate(
        json_obj.value(AnilistKeys::Media::StartDate).toObject()
    );
    media.end_date = AnilistUtils::dateFromFuzzyDate(
        json_obj.value(AnilistKeys::Media::EndDate).toObject()
    );
    media.updated_at = json_obj.value(AnilistKeys::Media::UpdatedAt).toInteger();
    media.episodes = json_obj.value(AnilistKeys::Media::Episodes).toInt();
    media.country_of_origin = json_obj.value(AnilistKeys::Media::CountryOfOrigin).toString();
    media.cover_url = json_obj.value(AnilistKeys::Media::CoverImage).toObject()
                              .value(AnilistKeys::MediaCoverImage::Large).toString();
    media.genres = JsonUtils::stringListFromJsonArray(
        json_obj.value(AnilistKeys::Media::Genres).toArray()
    );
    media.studios = AnilistUtils::studiosFromJsonArray(
        json_obj.value(AnilistKeys::Media::Studios).toObject()
                .value(AnilistKeys::StudioConnection::Edges).toArray()
    );
    media.producers = AnilistUtils::producersFromJsonArray(
        json_obj.value(AnilistKeys::Media::Studios).toObject()
                .value(AnilistKeys::StudioConnection::Edges).toArray()
    );
    media.in_list = false;

    return media;
}

AnilistMedia AnilistMedia::fromDatabaseQuery(const QSqlQuery &query) {
    AnilistMedia media;

    media.id = query.value(DatabaseColumns::Media::Id).toInt();
    media.is_adult = query.value(DatabaseColumns::Media::IsAdult).toBool();
    media.status = query.value(DatabaseColumns::Media::Status).value<AnilistMedia::Status>();
    media.season = query.value(DatabaseColumns::Media::Season).value<AnilistMedia::Season>();
    media.season_year = query.value(DatabaseColumns::Media::SeasonYear).toInt();
    media.average_score = query.value(DatabaseColumns::Media::AverageScore).toInt();
    media.site_url = query.value(DatabaseColumns::Media::SiteUrl).toString();
    media.title = AnilistMedia::Title{
        query.value(DatabaseColumns::Media::TitleEnglish).toString(),
        query.value(DatabaseColumns::Media::TitleNative).toString(),
        query.value(DatabaseColumns::Media::TitleRomaji).toString()
    };
    media.synonyms = JsonUtils::stringListFromJsonString(
        query.value(DatabaseColumns::Media::Synonyms).toString()
    );
    media.format = query.value(DatabaseColumns::Media::Format).value<AnilistMedia::Format>();
    media.description = query.value(DatabaseColumns::Media::Description).toString();
    media.start_date = DateUtils::dateFromString(
        query.value(DatabaseColumns::Media::StartDate).toString()
    );
    media.end_date = DateUtils::dateFromString(
        query.value(DatabaseColumns::Media::EndDate).toString()
    );
    media.updated_at = query.value(DatabaseColumns::Media::UpdatedAt).toLongLong();
    media.episodes = query.value(DatabaseColumns::Media::Episodes).toInt();
    media.country_of_origin = query.value(DatabaseColumns::Media::CountryOfOrigin).toString();
    media.cover_url = query.value(DatabaseColumns::Media::CoverUrl).toString();
    media.genres = JsonUtils::stringListFromJsonString(
        query.value(DatabaseColumns::Media::Genres).toString()
    );
    media.studios = AnilistUtils::studiosProducersFromJsonString<AnilistMedia::Studio>(
        query.value(DatabaseColumns::Media::Studios).toString()
    );
    media.producers = AnilistUtils::studiosProducersFromJsonString<AnilistMedia::Producer>(
        query.value(DatabaseColumns::Media::Producers).toString()
    );
    media.in_list = false;

    return media;
}

