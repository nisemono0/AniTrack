#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_keys.hpp"
#include "base/anilist/anilist_config.hpp"

#include "utils/enums.hpp"
#include "utils/settings.hpp"
#include "utils/links.hpp"

#include <QUrlQuery>


AnilistAccount::AnilistAccount(QObject *parent) : QObject(parent) {
    this->auth_token_ = Settings::get(Settings::Anilist::User::AuthToken, QString());

    this->anilist_user_.user_id = Settings::get(Settings::Anilist::User::Id, InvalidId);
    this->anilist_user_.username = Settings::get(Settings::Anilist::User::Name, QString());
    this->anilist_user_.avatar_url = Settings::get(Settings::Anilist::User::AvatarUrl, QString());

    this->anilist_user_.score_format = Settings::get(
        Settings::Anilist::UserSettings::ScoreFormat,
        AnilistAccount::ScoreFormat::INVALID
    );
    this->anilist_user_.title_language = Settings::get(
        Settings::Anilist::UserSettings::TitleLanguage,
        AnilistAccount::TitleLanguage::INVALID
    );

    this->anilist_user_.anime_count = Settings::get(Settings::Anilist::UserStats::AnimeCount, 0);
    this->anilist_user_.episodes_watched = Settings::get(Settings::Anilist::UserStats::EpisodesWatched, 0);
    this->anilist_user_.minutes_watched = Settings::get(Settings::Anilist::UserStats::MinutesWatched, 0);
    this->anilist_user_.mean_score = Settings::get(Settings::Anilist::UserStats::MeanScore, 0.0);
    this->anilist_user_.std_deviation = Settings::get(Settings::Anilist::UserStats::StandardDeviation, 0.0);
}

AnilistAccount::User AnilistAccount::userFromViewerJson(const QJsonObject &json_object) {
    AnilistAccount::User anilist_user;

    anilist_user.user_id = json_object.value(AnilistKeys::User::Id).toInt();
    anilist_user.username = json_object.value(AnilistKeys::User::Name).toString();
    anilist_user.avatar_url = json_object.value(AnilistKeys::User::Avatar).toObject()
                                         .value(AnilistKeys::UserAvatar::Medium).toString();

    anilist_user.score_format = EnumsUtils::enumFromString(
        json_object.value(AnilistKeys::User::MediaListOptions).toObject()
                   .value(AnilistKeys::MediaListOptions::ScoreFormat).toString(),
        AnilistAccount::ScoreFormat::INVALID
    );
    anilist_user.title_language = EnumsUtils::enumFromString(
        json_object.value(AnilistKeys::User::Options).toObject()
                   .value(AnilistKeys::UserOptions::UserTitleLanguage).toString(),
        AnilistAccount::TitleLanguage::INVALID
    );

    QJsonObject stats_object = json_object.value(AnilistKeys::User::Statistics).toObject()
                                          .value(AnilistKeys::UserStatisticTypes::Anime).toObject();

    anilist_user.anime_count = stats_object.value(AnilistKeys::UserStatistics::Count).toInt();
    anilist_user.episodes_watched = stats_object.value(AnilistKeys::UserStatistics::EpisodesWatched).toInt();
    anilist_user.minutes_watched = stats_object.value(AnilistKeys::UserStatistics::MinutesWatched).toInt();
    anilist_user.mean_score = stats_object.value(AnilistKeys::UserStatistics::MeanScore).toDouble();
    anilist_user.std_deviation = stats_object.value(AnilistKeys::UserStatistics::StandardDeviation).toDouble();

    return anilist_user;
}

void AnilistAccount::setAuthToken(const QString &auth_token) {
    this->auth_token_ = auth_token;

    // Save auth token
    Settings::set(Settings::Anilist::User::AuthToken, auth_token);
}

void AnilistAccount::setUser(const AnilistAccount::User &user) {
    this->anilist_user_ = user;

    // Save user
    Settings::set(Settings::Anilist::User::Id, user.user_id);
    Settings::set(Settings::Anilist::User::Name, user.username);
    Settings::set(Settings::Anilist::User::AvatarUrl, user.avatar_url);

    Settings::set(Settings::Anilist::UserSettings::ScoreFormat, user.score_format);
    Settings::set(Settings::Anilist::UserSettings::TitleLanguage, user.title_language);

    Settings::set(Settings::Anilist::UserStats::AnimeCount, user.anime_count);
    Settings::set(Settings::Anilist::UserStats::EpisodesWatched, user.episodes_watched);
    Settings::set(Settings::Anilist::UserStats::MinutesWatched, user.minutes_watched);
    Settings::set(Settings::Anilist::UserStats::MeanScore, user.mean_score);
    Settings::set(Settings::Anilist::UserStats::StandardDeviation, user.std_deviation);
}

bool AnilistAccount::isValid() const {
    return this->anilist_user_.user_id != InvalidId &&
           !this->anilist_user_.username.isEmpty() &&
           !this->auth_token_.isEmpty();
}

const QString& AnilistAccount::authToken() const {
    return this->auth_token_;
}

const AnilistAccount::User& AnilistAccount::user() const {
    return this->anilist_user_;
}

std::expected<QUrl, QString> AnilistAccount::profileUrl() const {
    if (!this->isValid()) {
        return std::unexpected(
            QStringLiteral("Anilist account is not valid")
        );
    }
    return QUrl(LinksUtils::ExternalLinks::AniList).resolved(
        QStringLiteral("/user/%1").arg(this->anilist_user_.username)
    );
}

std::expected<QUrl, QString> AnilistAccount::listUrl() const {
    if (!this->isValid()) {
        return std::unexpected(
            QStringLiteral("Anilist account is not valid")
        );
    }
    return QUrl(LinksUtils::ExternalLinks::AniList).resolved(
        QStringLiteral("/user/%1/animelist").arg(this->anilist_user_.username)
    );
}

std::expected<QUrl, QString> AnilistAccount::statsUrl() const {
    if (!this->isValid()) {
        return std::unexpected(
            QStringLiteral("Anilist account is not valid")
        );
    }
    return QUrl(LinksUtils::ExternalLinks::AniList).resolved(
        QStringLiteral("/user/%1/stats/anime/overview").arg(this->anilist_user_.username)
    );
}

QUrl AnilistAccount::authUrl() const {
    QUrl auth_url(AnilistConfig::AuthUrl);
    QUrlQuery auth_query;

    auth_query.addQueryItem(
        QStringLiteral("client_id"),
        AnilistConfig::ClientId
    );

    auth_query.addQueryItem(
        QStringLiteral("response_type"),
        QStringLiteral("token")
    );

    auth_url.setQuery(auth_query);

    return auth_url;
}

bool AnilistAccount::operator==(const AnilistAccount &anilist_account) const {
    return this->anilist_user_.user_id == anilist_account.anilist_user_.user_id;
}

bool AnilistAccount::operator!=(const AnilistAccount &anilist_account) const {
    return !(*this == anilist_account);
}

