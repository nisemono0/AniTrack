#pragma once

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include <QAbstractItemModel>
#include <QModelIndex>
#include <QHash>


class AnimeListModel final : public QAbstractItemModel {
    Q_OBJECT;
public:
    enum class Columns {
        PendingIcon = 0,
        Title,
        Progress,
        Score,
        Format,
        Season,
        EntryStatus,
        MediaStatus,
        LastUpdated,
        StartedAt,
        CompletedAt,
        IsAdult,
        IsPrivate,
        RewatchCount,
        // Total columns
        ColumnCount
    };
    Q_ENUM(Columns);

    enum UserRoles {
        Anime = Qt::UserRole + 1,
        // For delegate
        PendingOperation,
        EntryProgress,
        EntryScore,
        MediaEpisodes,
        IsAdult,
        IsPrivate,
        // For proxy
        MediaTitle,
        EntryStatus,
        Sort,
    };
    Q_ENUM(UserRoles);

    enum HeaderRoles {
        ContextMenuTitle = Qt::UserRole + 1
    };
    Q_ENUM(HeaderRoles);


    explicit AnimeListModel(QObject *parent = nullptr);
    ~AnimeListModel() = default;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;

    void updateUserPreferences(const AnilistAccount::User &user);

    void setAnime(const QList<AnilistAnime> &anime_list);
    void updateAnime(const QList<AnilistAnime> &anime_list);
    void deleteAnime(const QList<int> &local_ids);
    void addAnime(const QList<AnilistAnime> &anime_list);

private:
    AnilistAccount::ScoreFormat score_format_;
    AnilistAccount::TitleLanguage title_language_;

    QList<AnilistAnime> anime_list_;
    QHash<int, int> local_id_to_anime_;

    void rebuildLocalIdHash();
};

