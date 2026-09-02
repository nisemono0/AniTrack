#pragma once

#include <QString>


namespace DatabaseResource {
inline const QString CreateMediaTable   = QStringLiteral(":/database/CreateEntryTable");
inline const QString CreateEntryTable   = QStringLiteral(":/database/CreateMediaTable");
inline const QString UpsertMedia        = QStringLiteral(":/database/UpsertMedia");
inline const QString UpsertEntry        = QStringLiteral(":/database/UpsertEntry");
inline const QString SelectAllAnime     = QStringLiteral(":/database/SelectAllAnime");
inline const QString SelectAllPending   = QStringLiteral(":/database/SelectAllPending");
inline const QString DeleteEntry        = QStringLiteral(":/database/DeleteEntry");
inline const QString CountAnimeEntries  = QStringLiteral(":/database/CountAnimeEntries");
inline const QString CleanupUnusedMedia = QStringLiteral(":/database/CleanupUnusedMedia");
inline const QString InsertEntry        = QStringLiteral(":/database/InsertEntry");
} // namespace DatabaseResource

