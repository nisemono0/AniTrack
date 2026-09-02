#pragma once

#include "base/anilist/anilist_anime.hpp"

#include <expected>

#include <QObject>
#include <QSqlDatabase>
#include <QList>


class Database final : public QObject {
    Q_OBJECT;
public:
    explicit Database(QObject *parent = nullptr);
    ~Database() = default;

    // Initialize the database
    bool init();

    // Upsert both entries and media from an AnilistAnime list
    bool upsertAnime(const QList<AnilistAnime> &anime_list);
    bool upsertAnime(const AnilistAnime &anime);

    // Insert entries and upsert medias from an AnilistAnime list
    // The inserted entries have their local_id updated to the
    // inserted database local_id
    bool insertAnime(QList<AnilistAnime> &anime_list);
    bool insertAnime(AnilistAnime &anime);

    // Upsert only the entries from an AnilistAnime list
    bool upsertEntries(const QList<AnilistAnime> &anime_list);
    bool upsertEntry(const AnilistAnime &anime);

    // Insert only the entries from an AnilistAnime list
    // The inserted entries have their local_id updated to the
    // inserted database local_id
    bool insertEntries(QList<AnilistAnime> &anime_list);
    bool insertEntry(AnilistAnime &anime);

    // Upsert only the medias from an AnilistAnime list
    bool upsertMedias(const QList<AnilistAnime> &anime_list);
    bool upsertMedia(const AnilistAnime &anime);

    // Delete based on local ids
    bool deleteEntries(const QList<int> &local_ids);
    bool deleteEntry(int local_id);

    // Return the number of entries in the database
    std::expected<int, QString> entriesCount();

    // Cleanup unused media from the database
    bool cleanupUnusedMedia();

    // Vacuums the database
    bool vacuum();

    // Recreate the database
    bool recreateDatabase();

    // Return a list of anilist anime from the database or a string message on error
    std::expected<QList<AnilistAnime>, QString> selectAllEntries();
    std::expected<QList<AnilistAnime>, QString> selectAllPendingEntries();

    // Return the databse file location
    QString databasePath() const;

private:
    QSqlDatabase db_;
    QString db_path_;

    bool setDatabasePath();
    bool openDatabase();
    bool enablePragmas();
    bool createTables();

    // Bind anime media to query
    void bindMediaQuery(QSqlQuery &query, const AnilistAnime &anime);
    // Bind anime entry to query
    void bindEntryQuery(QSqlQuery &query, const AnilistAnime &anime);

    // Bind local id to query
    void bindDeleteQuery(QSqlQuery &query, int local_id);
};

