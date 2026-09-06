#include "gui/models/anime_list_model.hpp"

#include "utils/anilist.hpp"

#include <algorithm>

#include <QBrush>


AnimeListModel::AnimeListModel(QObject *parent) : QAbstractItemModel(parent) {}

QModelIndex AnimeListModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid()) {
        return QModelIndex();
    }

    if (row < 0 || row >= this->anime_list_.size()) {
        return QModelIndex();
    }

    if (column < 0 || column >= static_cast<int>(Columns::ColumnCount)) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex AnimeListModel::parent(const QModelIndex &index) const {
    return QModelIndex();
}

int AnimeListModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return this->anime_list_.size();
}

int AnimeListModel::columnCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(Columns::ColumnCount);
}

QVariant AnimeListModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= this->anime_list_.size()) {
        return QVariant();
    }

    const AnilistAnime &anime = this->anime_list_.at(index.row());
    Columns column = static_cast<Columns>(index.column());

    switch (role) {
        // User roles
        case UserRoles::Anime:
            return QVariant::fromValue(anime);
        case UserRoles::PendingOperation:
            return QVariant::fromValue(anime.entry.state().pending_operation);
        case UserRoles::EntryProgress:
            return anime.entry.state().progress;
        case UserRoles::EntryScore:
            return anime.entry.state().score;
        case UserRoles::MediaEpisodes:
            return anime.media.episodes;
        case UserRoles::IsAdult:
            return anime.media.is_adult;
        case UserRoles::IsPrivate:
            return anime.entry.isPrivate();
        case UserRoles::MediaTitle:
            return QVariant::fromValue(anime.media.title);
        case UserRoles::EntryStatus:
            return QVariant::fromValue(anime.entry.state().status);
        // Role used to sort the columns
        case UserRoles::Sort: {
            switch (column) {
                case Columns::PendingIcon:
                    return static_cast<int>(anime.entry.state().pending_operation);
                case Columns::Title:
                    return AnilistUtils::animeTitleToPrettyString(anime.media.title, this->title_language_);
                case Columns::Progress:
                    return anime.entry.state().progress;
                case Columns::Score:
                    return anime.entry.state().score;
                case Columns::Format:
                    return static_cast<int>(anime.media.format);
                case Columns::Season:
                    return anime.media.season_year * 5 + static_cast<int>(anime.media.season);
                case Columns::EntryStatus:
                    return static_cast<int>(anime.entry.state().status);
                case Columns::MediaStatus:
                    return static_cast<int>(anime.media.status);
                case Columns::LastUpdated:
                    return anime.entry.state().updated_at;
                case Columns::StartedAt:
                    return anime.entry.state().started_at;
                case Columns::CompletedAt:
                    return anime.entry.state().completed_at;
                case Columns::IsAdult:
                    return anime.media.is_adult;
                case Columns::IsPrivate:
                    return anime.entry.isPrivate();
                case Columns::RewatchCount:
                    return anime.entry.state().repeat;
                default:
                    return QVariant();
            }
            break;
        }
        // Display role
        case Qt::DisplayRole: {
            switch (column) {
                case Columns::PendingIcon:
                    return QVariant(); // Delegate column
                case Columns::Title:
                    return AnilistUtils::animeTitleToPrettyString(anime.media.title, this->title_language_);
                case Columns::Progress:
                    return QVariant(); // Delegate column
                case Columns::Score:
                    return AnilistUtils::entryScoreToPrettyString(anime.entry.state().score, this->score_format_);
                case Columns::Format:
                    return AnilistUtils::mediaFormatToPrettyString(anime.media.format);
                case Columns::Season:
                    return AnilistUtils::mediaSeasonToPrettyString(anime.media.season, anime.media.season_year);
                case Columns::EntryStatus:
                    return AnilistUtils::entryStatusToPrettyString(anime.entry.state().status);
                case Columns::MediaStatus:
                    return AnilistUtils::mediaStatusToPrettyString(anime.media.status);
                case Columns::LastUpdated:
                    return AnilistUtils::epochToPrettyString(anime.entry.state().updated_at);
                case Columns::StartedAt:
                    return AnilistUtils::dateToPrettyString(anime.entry.state().started_at);
                case Columns::CompletedAt:
                    return AnilistUtils::dateToPrettyString(anime.entry.state().completed_at);
                case Columns::IsAdult:
                    return QVariant(); // Delegate column
                case Columns::IsPrivate:
                    return QVariant(); // Delegate column
                case Columns::RewatchCount:
                    return AnilistUtils::rewatchToPrettyString(anime.entry.state().repeat);
                default:
                    return QVariant();
            }
            break;
        }
        // Color
        case Qt::ForegroundRole: {
            switch (column) {
                case Columns::Score: {
                    if (anime.entry.state().score <= 0) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::Format: {
                    if (anime.media.format == AnilistMedia::Format::INVALID) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::Season: {
                    if (anime.media.season == AnilistMedia::Season::INVALID) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::MediaStatus: {
                    if (anime.media.status == AnilistMedia::Status::NOT_YET_RELEASED) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::LastUpdated: {
                    if (anime.entry.state().updated_at <= 0) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::StartedAt: {
                    if (!anime.entry.state().started_at.isValid()) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::CompletedAt: {
                    if (!anime.entry.state().completed_at.isValid()) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::RewatchCount: {
                    if (anime.entry.state().repeat <= 0) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                default:
                    return QVariant();
            }
            break;
        }
        // Text alignment
        case Qt::TextAlignmentRole: {
            switch (column) {
                case Columns::Title:
                    return QVariant(Qt::AlignLeft | Qt::AlignVCenter);
                case Columns::Season:
                    return QVariant(Qt::AlignRight | Qt::AlignVCenter);
                default:
                    return Qt::AlignCenter;
            }
            break;
        }
    }

    return QVariant();
}

QVariant AnimeListModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    Columns column = static_cast<Columns>(section);

    switch (role) {
        // Header titles used for context menu
        case HeaderRoles::ContextMenuTitle: {
            switch (column) {
                // Return custom context menu title for pendig icon
                case Columns::PendingIcon:
                    return QStringLiteral("Pending operation");
                // Use the displayed header title for the rest
                default:
                    return this->headerData(section, orientation, Qt::DisplayRole);
                    break;
            }
        }
        // Header title
        case Qt::DisplayRole: {
            switch (column) {
                case Columns::PendingIcon:
                    return QVariant();
                case Columns::Title:
                    return QStringLiteral("Anime title");
                case Columns::Progress:
                    return QStringLiteral("Progress");
                case Columns::Score:
                    return QStringLiteral("Score");
                case Columns::Format:
                    return QStringLiteral("Format");
                case Columns::Season:
                    return QStringLiteral("Season");
                case Columns::EntryStatus:
                    return QStringLiteral("Status");
                case Columns::MediaStatus:
                    return QStringLiteral("Anime status");
                case Columns::LastUpdated:
                    return QStringLiteral("Last updated");
                case Columns::StartedAt:
                    return QStringLiteral("Started");
                case Columns::CompletedAt:
                    return QStringLiteral("Completed");
                case Columns::IsAdult:
                    return QStringLiteral("Adult");
                case Columns::IsPrivate:
                    return QStringLiteral("Private");
                case Columns::RewatchCount:
                    return QStringLiteral("Rewatches");
                default:
                    return QVariant();
            }
        }
        // Text alignment
        case Qt::TextAlignmentRole: {
            switch (column) {
                case Columns::Title:
                    return QVariant::fromValue(Qt::AlignLeft | Qt::AlignVCenter);
                case Columns::Season:
                    return QVariant::fromValue(Qt::AlignRight | Qt::AlignVCenter);
                default:
                    return Qt::AlignCenter;
            }
        }
    }

    return QVariant();
}

Qt::ItemFlags AnimeListModel::flags(const QModelIndex &index) const {
    if (!index.isValid()) {
        return QAbstractItemModel::flags(index);
    }

    switch (static_cast<Columns>(index.column())) {
        case Columns::Progress:
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        case Columns::Score:
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        case Columns::EntryStatus:
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        case Columns::StartedAt:
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        case Columns::CompletedAt:
            return QAbstractItemModel::flags(index) | Qt::ItemIsEditable;
        default:
            return QAbstractItemModel::flags(index);
    }
}

void AnimeListModel::updateUserPreferences(const AnilistAccount::User &user) {
    this->score_format_ = user.score_format;
    this->title_language_ = user.title_language;

    emit dataChanged(
        this->index(0, 0),
        this->index(this->rowCount() - 1, this->columnCount() - 1),
        {Qt::DisplayRole}
    );

}

void AnimeListModel::setAnime(const QList<AnilistAnime> &anime_list) {
    beginResetModel();

    this->anime_list_ = anime_list;
    this->rebuildLocalIdHash();

    endResetModel();
}

void AnimeListModel::updateAnime(const QList<AnilistAnime> &anime_list) {
    for (const auto &anime : anime_list) {
        const auto it = this->local_id_to_anime_.constFind(anime.entry.localId());
        if (it == this->local_id_to_anime_.constEnd()) {
            continue;
        }

        int entry_idx = it.value();
        this->anime_list_[entry_idx] = anime;

        emit dataChanged(
            this->index(entry_idx, 0),
            this->index(entry_idx, this->columnCount() - 1)
        );
    }
}

void AnimeListModel::deleteAnime(const QList<int> &local_ids) {
    QList<int> entry_idxs;
    for (const auto &id : local_ids) {
        const auto it = this->local_id_to_anime_.constFind(id);
        if (it == this->local_id_to_anime_.constEnd()) {
            continue;
        }

        entry_idxs.append(it.value());
    }

    std::sort(entry_idxs.begin(), entry_idxs.end(), std::greater<int>());

    for (const auto &idx : entry_idxs) {
        beginRemoveRows(QModelIndex(), idx, idx);

        this->anime_list_.removeAt(idx);

        endRemoveRows();
    }

    this->rebuildLocalIdHash();
}

void AnimeListModel::addAnime(const QList<AnilistAnime> &anime_list) {
    int first_row = this->anime_list_.size();
    int last_row = first_row + anime_list.size() - 1;

    beginInsertRows(QModelIndex(), first_row, last_row);

    for (const auto &anime : anime_list) {
        int anime_idx = this->anime_list_.size();

        this->anime_list_.append(anime);
        this->local_id_to_anime_.insert(anime.entry.localId(), anime_idx);
    }

    endInsertRows();
}

void AnimeListModel::rebuildLocalIdHash() {
    this->local_id_to_anime_.clear();
    this->local_id_to_anime_.reserve(this->anime_list_.size());

    for (int i = 0; i < this->anime_list_.size(); i++) {
        this->local_id_to_anime_.insert(
            this->anime_list_.at(i).entry.localId(),
            i
        );
    }
}

