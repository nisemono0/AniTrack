#include "base/database/database.hpp"
#include "base/database/database_config.hpp"
#include "base/database/database_resources.hpp"
#include "base/database/database_keys.hpp"
#include "base/database/database_columns.hpp"

#include "utils/log.hpp"
#include "utils/file.hpp"
#include "utils/anilist.hpp"
#include "utils/json.hpp"
#include "utils/date.hpp"

#include <expected>

#include <QDir>
#include <QStandardPaths>
#include <QSqlQuery>
#include <QSqlError>


namespace {
enum class QueryType {
    EnableForeignKeys,
    CreateMediaTable,
    CreateEntryTable,
    UpsertMedia,
    UpsertEntry,
    InsertEntry,
    SelectAllAnime,
    SelectAllPending,
    DeleteEntry,
    CountAnimeEntries,
    CleanupUnusedMedia,
    Vacuum
};

std::expected<QSqlQuery, QString> createQuery(QueryType type, const QSqlDatabase &db) {
    QString query_text;
    switch (type) {
        case QueryType::EnableForeignKeys: {
            query_text = QStringLiteral("PRAGMA foreign_keys = ON;");
            break;
        }
        case QueryType::CreateMediaTable: {
            query_text = FileUtils::readFile(DatabaseResource::CreateMediaTable);
            break;
        }
        case QueryType::CreateEntryTable: {
            query_text = FileUtils::readFile(DatabaseResource::CreateEntryTable);
            break;
        }
        case QueryType::UpsertMedia: {
            query_text = FileUtils::readFile(DatabaseResource::UpsertMedia);
            break;
        }
        case QueryType::UpsertEntry: {
            query_text = FileUtils::readFile(DatabaseResource::UpsertEntry);
            break;
        }
        case QueryType::InsertEntry: {
            query_text = FileUtils::readFile(DatabaseResource::InsertEntry);
            break;
        }
        case QueryType::SelectAllAnime: {
            query_text = FileUtils::readFile(DatabaseResource::SelectAllAnime);
            break;
        }
        case QueryType::SelectAllPending: {
            query_text = FileUtils::readFile(DatabaseResource::SelectAllPending);
            break;
        }
        case QueryType::DeleteEntry: {
            query_text = FileUtils::readFile(DatabaseResource::DeleteEntry);
            break;
        }
        case QueryType::CountAnimeEntries: {
            query_text = FileUtils::readFile(DatabaseResource::CountAnimeEntries);
            break;
        }
        case QueryType::CleanupUnusedMedia: {
            query_text = FileUtils::readFile(DatabaseResource::CleanupUnusedMedia);
            break;
        }
        case QueryType::Vacuum: {
            query_text = QStringLiteral("VACUUM;");
            break;
        }
        default:
            return std::unexpected(QStringLiteral("Failed to load query"));
    }

    QSqlQuery query(db);
    if (!query.prepare(query_text)) {
        return std::unexpected(query.lastError().text());
    }

    return query;
}
}

Database::Database(QObject *parent) : QObject(parent) {}

bool Database::init() {
    return this->setDatabasePath() &&
           this->openDatabase() &&
           this->enablePragmas() &&
           this->createTables();
}

bool Database::upsertAnime(const QList<AnilistAnime> &anime_list) {
    if (!this->upsertMedias(anime_list)) {
        return false;
    }

    if (!this->upsertEntries(anime_list)) {
        return false;
    }

    return true;
}

bool Database::upsertAnime(const AnilistAnime &anime) {
    return this->upsertAnime(QList<AnilistAnime>{anime});
}

bool Database::insertAnime(QList<AnilistAnime> &anime_list) {
    if (!this->upsertMedias(anime_list)) {
        return false;
    }

    if (!this->insertEntries(anime_list)) {
        return false;
    }

    return true;
}

bool Database::insertAnime(AnilistAnime &anime) {
    if (!this->upsertMedia(anime)) {
        return false;
    }

    if (!this->insertEntry(anime)) {
        return false;
    }

    return true;
}

bool Database::upsertEntries(const QList<AnilistAnime> &anime_list) {
    auto entry_query = createQuery(QueryType::UpsertEntry, this->db_);
    if (!entry_query) {
        Log::error(
            CONTEXT_CLASS,
            entry_query.error()
        );
        return false;
    }

    if (!this->db_.transaction()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to start database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    for (const auto &anime : anime_list) {
        this->bindEntryQuery(entry_query.value(), anime);

        if (!entry_query->exec()) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to upsert entry: %1").arg(
                    entry_query->lastError().text()
                )
            );
            this->db_.rollback();
            return false;
        }
    }

    if (!this->db_.commit()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to commit database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    return true;
}

bool Database::upsertEntry(const AnilistAnime &anime) {
    return this->upsertEntries(QList<AnilistAnime>{anime});
}

bool Database::insertEntries(QList<AnilistAnime> &anime_list) {
    auto entry_query = createQuery(QueryType::InsertEntry, this->db_);
    if (!entry_query) {
        Log::error(
            CONTEXT_CLASS,
            entry_query.error()
        );
        return false;
    }

    if (!this->db_.transaction()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to start database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    for (auto &anime : anime_list) {
        this->bindEntryQuery(entry_query.value(), anime);

        if (!entry_query->exec()) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to insert entry: %1").arg(
                    entry_query->lastError().text()
                )
            );
            this->db_.rollback();
            return false;
        }

        if (!entry_query->next()) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to retrieved the inserted entry_local_id: %1").arg(
                    entry_query->lastError().text()
                )
            );
            return false;
        }

        int inserted_local_id = entry_query->value(DatabaseColumns::Entry::LocalId).toInt();
        anime.entry.setLocalId(inserted_local_id);

        entry_query->finish();
    }

    if (!this->db_.commit()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to commit database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    return true;
}

bool Database::insertEntry(AnilistAnime &anime) {
    auto entry_query = createQuery(QueryType::InsertEntry, this->db_);
    if (!entry_query) {
        Log::error(
            CONTEXT_CLASS,
            entry_query.error()
        );
        return false;
    }

    if (!this->db_.transaction()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to start database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    this->bindEntryQuery(entry_query.value(), anime);

    if (!entry_query->exec()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to insert entry: %1").arg(
                entry_query->lastError().text()
            )
        );
        this->db_.rollback();
        return false;
    }

    if (!entry_query->next()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to retrieved the inserted entry_local_id: %1").arg(
                entry_query->lastError().text()
            )
        );
        return false;
    }

    int inserted_local_id = entry_query->value(DatabaseColumns::Entry::LocalId).toInt();
    anime.entry.setLocalId(inserted_local_id);

    entry_query->finish();

    if (!this->db_.commit()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to commit database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    return true;
}

bool Database::upsertMedias(const QList<AnilistAnime> &anime_list) {
    auto media_query = createQuery(QueryType::UpsertMedia, this->db_);
    if (!media_query) {
        Log::error(
            CONTEXT_CLASS,
            media_query.error()
        );
        return false;
    }

    if (!this->db_.transaction()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to start database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    for (const auto &anime : anime_list) {
        this->bindMediaQuery(media_query.value(), anime);

        if (!media_query->exec()) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to upsert media: %1").arg(
                    media_query->lastError().text()
                )
            );
            this->db_.rollback();
            return false;
        }
    }

    if (!this->db_.commit()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to commit database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    return true;
}

bool Database::upsertMedia(const AnilistAnime &anime) {
    return this->upsertMedias(QList<AnilistAnime>{anime});
}

bool Database::deleteEntries(const QList<int> &local_ids) {
    auto delete_query = createQuery(QueryType::DeleteEntry, this->db_);
    if (!delete_query) {
        Log::error(
            CONTEXT_CLASS,
            delete_query.error()
        );
        return false;
    }

    if (!this->db_.transaction()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to start database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }

    for (const auto &id : local_ids) {
        this->bindDeleteQuery(delete_query.value(), id);
        if (!delete_query->exec()) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to delete entry: %1").arg(
                    delete_query->lastError().text()
                )
            );
            this->db_.rollback();
            return false;
        }
    }

    if (!this->db_.commit()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to commit database transaction: %1").arg(
                this->db_.lastError().text()
            )
        );
        return false;
    }
    return true;
}

bool Database::deleteEntry(int local_id) {
    return this->deleteEntries({local_id});
}

std::expected<int, QString> Database::entriesCount() {
    auto query = createQuery(QueryType::CountAnimeEntries, this->db_);

    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return std::unexpected(query.error());
    }

    if (!query->exec()) {
        QString msg = QStringLiteral("Failed to count entries: %1").arg(
            query->lastError().text()
        );
        Log::error(
            CONTEXT_CLASS,
            msg
        );
        return std::unexpected(msg);
    }

    if (!query->next()) {
        QString msg = QStringLiteral("Failed to retrieve entry count: %1").arg(
            query->lastError().text()
        );
        return std::unexpected(msg);
    }

    return query->value(0).toInt();
}

bool Database::cleanupUnusedMedia() {
    auto query = createQuery(QueryType::CleanupUnusedMedia, this->db_);

    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return false;
    }

    if (!query->exec()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to clean unused media from the database: %1").arg(query->lastError().text())
        );
        return false;
    }

    return true;
}

bool Database::vacuum() {
    auto query = createQuery(QueryType::Vacuum, this->db_);

    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return false;
    }

    if (!query->exec()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to vacuum the database: %1").arg(query->lastError().text())
        );
        return false;
    }

    return true;
}

bool Database::recreateDatabase() {
    if (this->db_.isOpen()) {
        this->db_.close();

        this->db_ = QSqlDatabase();
    }

    QSqlDatabase::removeDatabase(DatabaseConfig::ConnectionName);

    if (!QFile::remove(this->db_path_)) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to delete: %1").arg(this->db_path_)
        );
        return false;
    }

    return this->init();
}

std::expected<QList<AnilistAnime>, QString> Database::selectAllEntries() {
    auto query = createQuery(QueryType::SelectAllAnime, this->db_);
    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return std::unexpected(query.error());
    }

    if (!query->exec()) {
        QString msg = QStringLiteral("Failed to select all entries: %1").arg(
            query->lastError().text()
        );
        Log::error(
            CONTEXT_CLASS,
            msg
        );
        return std::unexpected(msg);
    }

    QList<AnilistAnime> loaded_anime;
    while (query->next()) {
        loaded_anime.append({
            AnilistEntry::fromDatabaseQuery(query.value()),
            AnilistMedia::fromDatabaseQuery(query.value())
        });
    }

    return loaded_anime;
}

std::expected<QList<AnilistAnime>, QString> Database::selectAllPendingEntries() {
    auto query = createQuery(QueryType::SelectAllPending, this->db_);
    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return std::unexpected(query.error());
    }

    if (!query->exec()) {
        QString msg = QStringLiteral("Failed to select all pending entries: %1").arg(
            query->lastError().text()
        );
        Log::error(
            CONTEXT_CLASS,
            msg
        );
        return std::unexpected(msg);
    }

    QList<AnilistAnime> loaded_anime;
    while (query->next()) {
        loaded_anime.append({
            AnilistEntry::fromDatabaseQuery(query.value()),
            AnilistMedia::fromDatabaseQuery(query.value())
        });
    }

    return loaded_anime;
}

QString Database::databasePath() const {
    return this->db_path_;
}

bool Database::setDatabasePath() {
    QDir db_dir(
        QStandardPaths::writableLocation(QStandardPaths::AppDataLocation)
    );

    if (!db_dir.exists()) {
        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Trying to create database dir: %1").arg(db_dir.absolutePath())
        );
        if (!db_dir.mkpath(db_dir.absolutePath())) {
            Log::error(
                CONTEXT_CLASS,
                QStringLiteral("Failed to create database dir: %1").arg(db_dir.absolutePath())
            );
            return false;
        }
    }

    QString db_file = db_dir.filePath(DatabaseConfig::DatabaseFilename);

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Using database: %1").arg(db_file)
    );

    this->db_ = QSqlDatabase::addDatabase(
        DatabaseConfig::DriverName,
        DatabaseConfig::ConnectionName
    );

    this->db_path_ = db_file;
    this->db_.setDatabaseName(this->db_path_);

    return true;
}

bool Database::openDatabase() {
    if (!this->db_.open()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to open the database")
        );
        return false;
    }
    return true;
}

bool Database::enablePragmas() {
    auto query = createQuery(QueryType::EnableForeignKeys, this->db_);

    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return false;
    }
    if (!query->exec()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to enable foreign keys: %1").arg(
                query->lastError().text()
            )
        );
        return false;
    }

    return true;
}

bool Database::createTables() {
    auto query = createQuery(QueryType::CreateMediaTable, this->db_);
    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return false;
    }
    if (!query->exec()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to create media table: %1").arg(
                query->lastError().text()
            )
        );
        return false;
    }

    query = createQuery(QueryType::CreateEntryTable, this->db_);
    if (!query) {
        Log::error(
            CONTEXT_CLASS,
            query.error()
        );
        return false;
    }
    if (!query->exec()) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to create entry table: %1").arg(
                query->lastError().text()
            )
        );
        return false;
    }

    return true;
}

void Database::bindMediaQuery(QSqlQuery &query, const AnilistAnime &anime) {
    query.bindValue(DatabaseKeys::Media::Id, anime.media.id);
    query.bindValue(DatabaseKeys::Media::IsAdult, anime.media.is_adult);
    query.bindValue(DatabaseKeys::Media::MediaStatus, static_cast<int>(anime.media.status));
    query.bindValue(DatabaseKeys::Media::Season, static_cast<int>(anime.media.season));
    query.bindValue(DatabaseKeys::Media::SeasonYear, anime.media.season_year);
    query.bindValue(DatabaseKeys::Media::AverageScore, anime.media.average_score);
    query.bindValue(DatabaseKeys::Media::SiteUrl, anime.media.site_url);
    query.bindValue(DatabaseKeys::Media::TitleEnglish, anime.media.title.english);
    query.bindValue(DatabaseKeys::Media::TitleNative, anime.media.title.native);
    query.bindValue(DatabaseKeys::Media::TitleRomaji, anime.media.title.romaji);
    query.bindValue(DatabaseKeys::Media::Synonyms, JsonUtils::jsonStringFromStringList(anime.media.synonyms));
    query.bindValue(DatabaseKeys::Media::Format, static_cast<int>(anime.media.format));
    query.bindValue(DatabaseKeys::Media::Description, anime.media.description);
    query.bindValue(
        DatabaseKeys::Media::StartDate,
        anime.media.start_date.isValid() ? DateUtils::stringFromDate(anime.media.start_date) : QVariant()
    );
    query.bindValue(
        DatabaseKeys::Media::EndDate,
        anime.media.end_date.isValid() ? DateUtils::stringFromDate(anime.media.end_date) : QVariant()
    );
    query.bindValue(DatabaseKeys::Media::UpdatedAt, anime.media.updated_at);
    query.bindValue(DatabaseKeys::Media::Episodes, anime.media.episodes);
    query.bindValue(DatabaseKeys::Media::CountryOfOrigin, anime.media.country_of_origin);
    query.bindValue(DatabaseKeys::Media::CoverUrl, anime.media.cover_url);
    query.bindValue(DatabaseKeys::Media::Genres, JsonUtils::jsonStringFromStringList(anime.media.genres));
    query.bindValue(DatabaseKeys::Media::Studios, AnilistUtils::jsonStringFromStudiosProducers(anime.media.studios));
    query.bindValue(DatabaseKeys::Media::Producers, AnilistUtils::jsonStringFromStudiosProducers(anime.media.producers));
}

void Database::bindEntryQuery(QSqlQuery &query, const AnilistAnime &anime) {
    query.bindValue(
        DatabaseKeys::Entry::Id,
        anime.entry.id() != AnilistEntry::InvalidId ? anime.entry.id() : QVariant()
    );
    query.bindValue(DatabaseKeys::Entry::MediaId, anime.media.id);
    query.bindValue(DatabaseKeys::Entry::IsPrivate, anime.entry.isPrivate());

    const auto &state = anime.entry.state();
    query.bindValue(DatabaseKeys::Entry::Status, static_cast<int>(state.status));
    query.bindValue(DatabaseKeys::Entry::Progress, state.progress);
    query.bindValue(DatabaseKeys::Entry::Repeat, state.repeat);
    query.bindValue(DatabaseKeys::Entry::Notes, state.notes);
    query.bindValue(DatabaseKeys::Entry::Score, state.score);
    query.bindValue(
        DatabaseKeys::Entry::StartedAt,
        state.started_at.isValid() ? DateUtils::stringFromDate(state.started_at) : QVariant()
    );
    query.bindValue(
        DatabaseKeys::Entry::CompletedAt,
        state.completed_at.isValid() ? DateUtils::stringFromDate(state.completed_at) : QVariant()
    );
    query.bindValue(DatabaseKeys::Entry::UpdatedAt, state.updated_at);
    query.bindValue(DatabaseKeys::Entry::PendingOperation, static_cast<int>(state.pending_operation));
}

void Database::bindDeleteQuery(QSqlQuery &query, int local_id) {
    query.bindValue(DatabaseKeys::Entry::LocalId, local_id);
}

