#pragma once

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_anime.hpp"

#include <QAbstractItemModel>
#include <QModelIndex>


class AnimeSearchModel final : public QAbstractItemModel {
    Q_OBJECT;
public:
    enum class Columns {
        Title = 0,
        Format,
        Episodes,
        Score,
        Season,
        Status,
        InList,
        StartDate,
        EndDate,
        IsAdult,
        ColumnCount
    };
    Q_ENUM(Columns);

    enum UserRoles {
        Media = Qt::UserRole + 1,
        // For delegates
        IsAdult,
        InList,
        // For proxy
        Title,
        Sort
    };
    Q_ENUM(UserRoles);

    enum HeaderRoles {
        ContextMenuTitle = Qt::UserRole + 1
    };
    Q_ENUM(HeaderRoles);


    explicit AnimeSearchModel(QObject *parent = nullptr);
    ~AnimeSearchModel() = default;

    QModelIndex index(int row, int column, const QModelIndex &parent = QModelIndex()) const override;
    QModelIndex parent(const QModelIndex &index) const override;
    int rowCount(const QModelIndex &parent = QModelIndex()) const override;
    int columnCount(const QModelIndex &parent = QModelIndex()) const override;
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

    void updateUserPreferences(const AnilistAccount::User &user);

    void setExistingMediaIds(const QList<AnilistAnime> &anime_list);

    void setMedia(const QList<AnilistMedia> &media_list);
    void updateAddedMedia(const QList<AnilistMedia> &media_list);

private:
    AnilistAccount::TitleLanguage title_language_;

    QList<AnilistMedia> media_list_;

    QHash<int, int> id_to_media_;
    QSet<int> existing_media_ids_;

    void rebuildIdHash();

};

