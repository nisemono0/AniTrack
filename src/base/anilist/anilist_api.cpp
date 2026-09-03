#include "base/anilist/anilist_api.hpp"
#include "base/anilist/anilist_resources.hpp"
#include "base/anilist/anilist_config.hpp"
#include "base/anilist/anilist_keys.hpp"

#include "utils/file.hpp"
#include "utils/log.hpp"
#include "utils/enums.hpp"
#include "utils/anilist.hpp"

#include <expected>

#include <QByteArray>
#include <QRestReply>
#include <QJsonObject>
#include <QJsonArray>


namespace {
enum class QueryType {
    Viewer,
    MediaListCollection,
    MediaSearch,
    SaveMediaListEntry,
    DeleteMediaListEntry
};

// Create a QByteArray of a query_type with added variables if given
// Returns a QString with an error message on failure
std::expected<QByteArray, QString> createQuery(QueryType query_type, const QJsonObject &variables = {}) {
    QString query;
    switch (query_type) {
        case QueryType::Viewer: {
            query = FileUtils::readFile(AnilistResources::Viewer);
            break;
        }
        case QueryType::MediaListCollection: {
            QStringList fragmented_query;
            fragmented_query.append(FileUtils::readFile(AnilistResources::MediaFields));
            fragmented_query.append(FileUtils::readFile(AnilistResources::EntryFields));
            fragmented_query.append(FileUtils::readFile(AnilistResources::MediaListCollection));
            query = fragmented_query.join(QChar::LineFeed);
            break;
        }
        case QueryType::MediaSearch:{
            QStringList fragmented_query;
            fragmented_query.append(FileUtils::readFile(AnilistResources::MediaFields));
            fragmented_query.append(FileUtils::readFile(AnilistResources::MediaSearch));
            query = fragmented_query.join(QChar::LineFeed);
            break;
        }
        case QueryType::SaveMediaListEntry: {
            QStringList fragmented_query;
            fragmented_query.append(FileUtils::readFile(AnilistResources::MediaFields));
            fragmented_query.append(FileUtils::readFile(AnilistResources::EntryFields));
            fragmented_query.append(FileUtils::readFile(AnilistResources::SaveMediaListEntry));
            query = fragmented_query.join(QChar::LineFeed);
            break;
        }
        case QueryType::DeleteMediaListEntry: {
            query = FileUtils::readFile(AnilistResources::DeleteMediaListEntry);
            break;
        }
        default:
            return std::unexpected(
                QStringLiteral("Query type does not exist")
            );
    }

    if (query.isEmpty()) {
        return std::unexpected(
            QStringLiteral("Query is empty")
        );
    }

    QJsonObject query_json;
    query_json[QStringLiteral("query")] = query;
    query_json[QStringLiteral("variables")] = variables;

    return QJsonDocument(query_json).toJson(QJsonDocument::Compact);
}

// Read the json api response from a network_reply and return the QJsonDocument
// Returns a QString with an error message on failure
std::expected<QJsonDocument, QString> readJsonResponse(QNetworkReply *network_reply) {
    QRestReply rest_reply(network_reply);

    // Network error
    if (rest_reply.hasError()) {
        return std::unexpected(rest_reply.errorString());
    }

    QJsonParseError parse_error;
    auto response = rest_reply.readJson(&parse_error);

    // Parse error
    if (!response) {
        return std::unexpected(parse_error.errorString());
    }

    if (!response->isObject()) {
        return std::unexpected(QStringLiteral("JSON response is not and object"));
    }

    // Response json object
    const QJsonObject response_obj = response->object();

    // Return the response QJsonDocument if response json has no api errors
    if (!response_obj.contains(AnilistKeys::Common::Errors)) {
        return response.value();
    }

    // Read the api errors and return a formatted QString with them
    QStringList api_errors;
    const QJsonArray response_errors = response_obj.value(AnilistKeys::Common::Errors).toArray();
    // Base errors
    for (const auto &error : response_errors) {
        const QJsonObject error_obj = error.toObject();
        QString error_text = error_obj.value(AnilistKeys::Common::Message).toString();
        QString error_status = QString::number(
            error_obj.value(AnilistKeys::Common::Status).toInt()
        );
        api_errors.append(
            QStringLiteral("%1 [%2].").arg(error_text, error_status)
        );

        // No validation errors, continue
        if (!error_obj.contains(AnilistKeys::Common::Validation)) {
            continue;
        }

        // Validation errors
        const QJsonObject validation_obj = error_obj.value(AnilistKeys::Common::Validation).toObject();
        for (const auto &[key, value] : validation_obj.asKeyValueRange()) {
            QStringList validation_errors;
            QJsonArray json_array = value.toArray();
            for (const auto &validation_text : std::as_const(json_array)) {
                validation_errors.append(
                    QStringLiteral(" -> %1: %2").arg(key.toString(), validation_text.toString())
                );
            }
            api_errors.append(
                validation_errors.join(QChar::LineFeed)
            );
        }
    }

    return std::unexpected(api_errors.join(QChar::LineFeed));
}


} // namespace


AnilistApi::AnilistApi(
    Network *network,
    AnilistAccount *anilist_account,
    QObject *parent
) : QObject(parent),
    anilist_account_(anilist_account),
    network_(network) {}

void AnilistApi::fetchUser() {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Fetching anilist user...")
    );

    auto query = createQuery(QueryType::Viewer);

    if (!query) {
        emit userFetchFailed(query.error());
        return;
    }

    QNetworkReply *network_reply = this->network_->postJson(
        AnilistConfig::ApiUrl,
        query.value(),
        this->anilist_account_->authToken()
    );

    connect(network_reply, &QNetworkReply::finished, this, [this, network_reply] {
        auto response = readJsonResponse(network_reply);
        network_reply->deleteLater();

        if (!response) {
            emit userFetchFailed(response.error());
            return;
        }

        AnilistAccount::User user = AnilistAccount::userFromViewerJson(
            response->object().value(AnilistKeys::Common::Data).toObject()
                              .value(AnilistKeys::Query::Viewer).toObject()
        );

        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Fetched user %1 (%2)").arg(user.username).arg(user.user_id)
        );

        emit userFetchFinished(user);
    });
}

void AnilistApi::fetchList() {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Fetching anilist anime list...")
    );

    QJsonObject query_variables;
    query_variables[AnilistKeys::Variables::UserId] = this->anilist_account_->user().user_id;

    auto query = createQuery(
        QueryType::MediaListCollection,
        query_variables
    );

    if (!query) {
        emit fetchListFailed(query.error());
        return;
    }

    QNetworkReply *network_reply = this->network_->postJson(
        AnilistConfig::ApiUrl,
        query.value(),
        this->anilist_account_->authToken()
    );

    connect(network_reply, &QNetworkReply::finished, this, [this, network_reply] {
        auto response = readJsonResponse(network_reply);
        network_reply->deleteLater();

        if (!response) {
            emit fetchListFailed(response.error());
            return;
        }

        QList<AnilistAnime> anilist_anime;

        QJsonArray lists_array = response->object().value(AnilistKeys::Common::Data).toObject()
                                                   .value(AnilistKeys::Query::MediaListCollection).toObject()
                                                   .value(AnilistKeys::MediaListCollection::Lists).toArray();

        for (const auto &list : std::as_const(lists_array)) {
            QJsonArray entries_array = list.toObject().value(AnilistKeys::MediaListGroup::Entries).toArray();
            for (const auto &entry : std::as_const(entries_array)) {
                AnilistAnime anime;
                anime.entry = AnilistEntry::fromResponseJson(entry.toObject());
                anime.media = AnilistMedia::fromResponseJson(
                    entry.toObject().value(AnilistKeys::MediaList::Media).toObject()
                );
                anime.media.in_list = true;

                anilist_anime.append(std::move(anime));
            }
        }

        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Fetched %1 anime entries").arg(anilist_anime.size())
        );

        emit fetchListFinished(anilist_anime);
    });
}

void AnilistApi::addAnime(const AnilistAnime &anime) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Adding anime to list...")
    );

    QJsonObject query_variables;
    query_variables[AnilistKeys::Variables::MediaId] = anime.media.id;
    query_variables[AnilistKeys::Variables::Status] = EnumsUtils::stringFromEnum(anime.entry.state().status, QStringLiteral("INVALID"));
    query_variables[AnilistKeys::Variables::ScoreRaw] = anime.entry.state().score;
    query_variables[AnilistKeys::Variables::Progress] = anime.entry.state().progress;
    query_variables[AnilistKeys::Variables::Repeat] = anime.entry.state().repeat;
    query_variables[AnilistKeys::Variables::Notes] = anime.entry.state().notes;
    query_variables[AnilistKeys::Variables::StartedAt] = AnilistUtils::fuzzyDateFromDate(anime.entry.state().started_at);
    query_variables[AnilistKeys::Variables::CompletedAt] = AnilistUtils::fuzzyDateFromDate(anime.entry.state().completed_at);

    auto query = createQuery(
        QueryType::SaveMediaListEntry,
        query_variables
    );

    if (!query) {
        emit addAnimeFailed(query.error());
        return;
    }

    QNetworkReply *network_reply = this->network_->postJson(
        AnilistConfig::ApiUrl,
        query.value(),
        this->anilist_account_->authToken()
    );

    connect(network_reply, &QNetworkReply::finished, this, [this, network_reply, anime] {
        auto response = readJsonResponse(network_reply);
        network_reply->deleteLater();

        if (!response) {
            emit addAnimeFailed(response.error());
            return;
        }

        QJsonObject anime_obj = response->object().value(AnilistKeys::Common::Data).toObject()
                                                  .value(AnilistKeys::Mutation::SaveMediaListEntry).toObject();
        AnilistAnime added_anime{
            AnilistEntry::fromResponseJson(anime_obj),
            AnilistMedia::fromResponseJson(anime_obj.value(AnilistKeys::MediaList::Media).toObject())
        };

        added_anime.entry.setLocalId(anime.entry.localId());

        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Added local anime to anilist: %1").arg(added_anime.media.id)
        );

        emit addAnimeFinished(added_anime);
    });
}

void AnilistApi::updateAnime(const AnilistAnime &anime) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Update anime on anilist...")
    );

    QJsonObject query_variables;
    query_variables[AnilistKeys::Variables::SaveMediaListEntryId] = anime.entry.id();
    query_variables[AnilistKeys::Variables::MediaId] = anime.media.id;
    query_variables[AnilistKeys::Variables::Status] = EnumsUtils::stringFromEnum(anime.entry.state().status, QStringLiteral("INVALID"));
    query_variables[AnilistKeys::Variables::ScoreRaw] = anime.entry.state().score;
    query_variables[AnilistKeys::Variables::Progress] = anime.entry.state().progress;
    query_variables[AnilistKeys::Variables::Repeat] = anime.entry.state().repeat;
    query_variables[AnilistKeys::Variables::Notes] = anime.entry.state().notes;
    query_variables[AnilistKeys::Variables::StartedAt] = AnilistUtils::fuzzyDateFromDate(anime.entry.state().started_at);
    query_variables[AnilistKeys::Variables::CompletedAt] = AnilistUtils::fuzzyDateFromDate(anime.entry.state().completed_at);

    auto query = createQuery(
        QueryType::SaveMediaListEntry,
        query_variables
    );

    if (!query) {
        emit updateAnimeFailed(query.error());
        return;
    }

    QNetworkReply *network_reply = this->network_->postJson(
        AnilistConfig::ApiUrl,
        query.value(),
        this->anilist_account_->authToken()
    );

    connect(network_reply, &QNetworkReply::finished, this, [this, network_reply, anime] {
        auto response = readJsonResponse(network_reply);
        network_reply->deleteLater();

        if (!response) {
            emit updateAnimeFailed(response.error());
            return;
        }

        QJsonObject anime_obj = response->object().value(AnilistKeys::Common::Data).toObject()
                                                  .value(AnilistKeys::Mutation::SaveMediaListEntry).toObject();
        AnilistAnime updated_anime{
            AnilistEntry::fromResponseJson(anime_obj),
            AnilistMedia::fromResponseJson(anime_obj.value(AnilistKeys::MediaList::Media).toObject())
        };

        updated_anime.entry.setLocalId(anime.entry.localId());

        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Updated local anime to anilist: %1").arg(updated_anime.media.id)
        );

        emit updateAnimeFinished(updated_anime);
    });
}

void AnilistApi::deleteAnime(const AnilistAnime &anime) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Removing anime from anilist...")
    );

    QJsonObject query_variables;
    query_variables[AnilistKeys::Variables::DeleteMediaListEntryId] = anime.entry.id();

    auto query = createQuery(
        QueryType::DeleteMediaListEntry,
        query_variables
    );

    if (!query) {
        emit deleteAnimeFailed(query.error());
        return;
    }

    QNetworkReply *network_reply = this->network_->postJson(
        AnilistConfig::ApiUrl,
        query.value(),
        this->anilist_account_->authToken()
    );

    connect(network_reply, &QNetworkReply::finished, this, [this, network_reply, anime] {
        auto response = readJsonResponse(network_reply);
        network_reply->deleteLater();

        if (!response) {
            emit deleteAnimeFailed(response.error());
            return;
        }

        bool is_deleted = response->object().value(AnilistKeys::Common::Data).toObject()
                                            .value(AnilistKeys::Mutation::DeleteMediaListEntry).toObject()
                                            .value(AnilistKeys::DeleteMediaListEntry::Deleted).toBool();

        if (!is_deleted) {
            QString msg = QStringLiteral("Anilist failed to remove: %1").arg(anime.entry.id());
            Log::warning(
                CONTEXT_CLASS,
                msg
            );
            emit deleteAnimeFailed(msg);
            return;
        }

        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Removed anime from anilist: %1").arg(anime.media.id)
        );

        emit deleteAnimeFinished(anime.entry.localId());
    });
}

void AnilistApi::searchAnime(const QString &title) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Searching anime...")
    );

    QJsonObject query_variables;
    query_variables[AnilistKeys::Variables::Search] = title;

    auto query = createQuery(
        QueryType::MediaSearch,
        query_variables
    );

    if (!query) {
        emit searchAnimeFailed(query.error());
        return;
    }

    QNetworkReply *network_reply = this->network_->postJson(
        AnilistConfig::ApiUrl,
        query.value(),
        this->anilist_account_->authToken()
    );

    connect(network_reply, &QNetworkReply::finished, this, [this, network_reply, title] {
        auto response = readJsonResponse(network_reply);
        network_reply->deleteLater();

        if (!response) {
            emit searchAnimeFailed(response.error());
            return;
        }

        QList<AnilistMedia> anime_search_results;
        QJsonArray media_array = response->object().value(AnilistKeys::Common::Data).toObject()
                                                   .value(AnilistKeys::Query::Page).toObject()
                                                   .value(AnilistKeys::Page::Media).toArray();

        for (const auto &media : std::as_const(media_array)) {
            AnilistMedia media_result = AnilistMedia::fromResponseJson(media.toObject());
            media_result.in_list = false;

            anime_search_results.append(std::move(media_result));
        }

        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Search finished for: %1 (found %2)").arg(title).arg(anime_search_results.size())
        );

        emit searchAnimeFinished(anime_search_results);
    });
}

