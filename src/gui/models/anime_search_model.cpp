#include "gui/models/anime_search_model.hpp"

#include "utils/anilist.hpp"

#include <QBrush>


AnimeSearchModel::AnimeSearchModel(QObject *parent) : QAbstractItemModel(parent) {}

QModelIndex AnimeSearchModel::index(int row, int column, const QModelIndex &parent) const {
    if (parent.isValid()) {
        return QModelIndex();
    }

    if (row < 0 || column >= static_cast<int>(Columns::ColumnCount)) {
        return QModelIndex();
    }

    return createIndex(row, column);
}

QModelIndex AnimeSearchModel::parent(const QModelIndex &index) const {
    return QModelIndex();
}

int AnimeSearchModel::rowCount(const QModelIndex &parent) const {
    if (parent.isValid()) {
        return 0;
    }
    return this->media_list_.size();
}

int AnimeSearchModel::columnCount(const QModelIndex &parent ) const {
    if (parent.isValid()) {
        return 0;
    }
    return static_cast<int>(Columns::ColumnCount);
}

QVariant AnimeSearchModel::data(const QModelIndex &index, int role) const {
    if (!index.isValid()) {
        return QVariant();
    }

    if (index.row() < 0 || index.row() >= this->media_list_.size()) {
        return QVariant();
    }

    const AnilistMedia &media = this->media_list_.at(index.row());
    Columns column = static_cast<Columns>(index.column());

    switch (role) {
        // User roles
        case UserRoles::Media:
            return QVariant::fromValue(media);
        case UserRoles::IsAdult:
            return media.is_adult;
        case UserRoles::InList:
            return media.in_list;
        case UserRoles::Title:
            return QVariant::fromValue(media.title);
        // Role used to sort the columns
        case UserRoles::Sort: {
            switch (column) {
                case Columns::Title:
                    return AnilistUtils::animeTitleToPrettyString(media.title, this->title_language_);
                case Columns::Format:
                    return static_cast<int>(media.format);
                case Columns::Episodes:
                    return media.episodes;
                case Columns::Score:
                    return media.average_score;
                case Columns::Season:
                    return media.season_year * 5 + static_cast<int>(media.season);
                case Columns::Status:
                    return static_cast<int>(media.status);
                case Columns::InList:
                    return media.in_list;
                case Columns::StartDate:
                    return media.start_date;
                case Columns::EndDate:
                    return media.end_date;
                case Columns::IsAdult:
                    return media.is_adult;
                default:
                    return QVariant();
            }
            break;
        }
        // Display role
        case Qt::DisplayRole: {
            switch (column) {
                case Columns::Title:
                    return AnilistUtils::animeTitleToPrettyString(media.title, this->title_language_);
                case Columns::Format:
                    return AnilistUtils::mediaFormatToPrettyString(media.format);
                case Columns::Episodes:
                    return AnilistUtils::mediaEpisodesToPrettyString(media.episodes);
                case Columns::Score:
                    return media.average_score;
                case Columns::Season:
                    return AnilistUtils::mediaSeasonToPrettyString(media.season, media.season_year);
                case Columns::Status:
                    return AnilistUtils::mediaStatusToPrettyString(media.status);
                case Columns::InList:
                    return QVariant(); // Delegate column
                case Columns::StartDate:
                    return AnilistUtils::dateToPrettyString(media.start_date);
                case Columns::EndDate:
                    return AnilistUtils::dateToPrettyString(media.end_date);
                case Columns::IsAdult:
                    return QVariant(); // Delegate column
                default:
                    return QVariant();
            }
            break;
        }
        // Color
        case Qt::ForegroundRole: {
            switch (column) {
                case Columns::Format: {
                    if (media.format == AnilistMedia::Format::INVALID) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::Episodes: {
                    if (media.episodes <= 0) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::Season: {
                    if (media.season == AnilistMedia::Season::INVALID) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::Status: {
                    if (media.status == AnilistMedia::Status::NOT_YET_RELEASED) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::StartDate: {
                    if (!media.start_date.isValid()) {
                        return QBrush(Qt::gray);
                    }
                    break;
                }
                case Columns::EndDate: {
                    if (!media.end_date.isValid()) {
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

QVariant AnimeSearchModel::headerData(int section, Qt::Orientation orientation, int role) const {
    if (orientation != Qt::Horizontal) {
        return QVariant();
    }

    Columns column = static_cast<Columns>(section);

    switch (role) {
        // Header titles used for context menu
        case HeaderRoles::ContextMenuTitle:
            return this->headerData(section, orientation, Qt::DisplayRole);
        // Header title
        case Qt::DisplayRole: {
            switch (column) {
                case Columns::Title:
                    return QStringLiteral("Anime title");
                case Columns::Format:
                    return QStringLiteral("Format");
                case Columns::Episodes:
                    return QStringLiteral("Episodes");
                case Columns::Score:
                    return QStringLiteral("Score");
                case Columns::Season:
                    return QStringLiteral("Season");
                case Columns::Status:
                    return QStringLiteral("Anime status");
                case Columns::InList:
                    return QStringLiteral("In list");
                case Columns::StartDate:
                    return QStringLiteral("Start date");
                case Columns::EndDate:
                    return QStringLiteral("End date");
                case Columns::IsAdult:
                    return QStringLiteral("Adult");
                default:
                    return QVariant();
            }
            break;
        }
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

void AnimeSearchModel::updateUserPreferences(const AnilistAccount::User &user) {
    this->title_language_ = user.title_language;

    emit dataChanged(
        this->index(0, 0),
        this->index(this->rowCount() - 1, this->columnCount() - 1),
        {Qt::DisplayRole}
    );
}

void AnimeSearchModel::setExistingMediaIds(const QList<AnilistAnime> &anime_list) {
    this->existing_media_ids_.clear();
    this->existing_media_ids_.reserve(anime_list.size());

    for (const auto &anime : anime_list) {
        this->existing_media_ids_.insert(anime.media.id);
    }

    for (int row = 0; row < this->media_list_.size(); row++) {
        bool in_list = this->existing_media_ids_.contains(
            this->media_list_.at(row).id
        );

        if (in_list == this->media_list_.at(row).in_list) {
            continue;
        }

        this->media_list_[row].in_list = in_list;

        emit dataChanged(
            this->index(row, 0),
            this->index(row, this->columnCount() - 1)
        );
    }
}

void AnimeSearchModel::setMedia(const QList<AnilistMedia> &media_list) {
    beginResetModel();

    this->media_list_.clear();
    this->media_list_.reserve(media_list.size());

    for (auto media : media_list) {
        if (this->existing_media_ids_.contains(media.id)) {
            media.in_list = true;
        }
        this->media_list_.append(std::move(media));
    }
    this->rebuildIdHash();

    endResetModel();
}

void AnimeSearchModel::updateAddedMedia(const QList<AnilistMedia> &media_list) {
    for (const auto &media : media_list) {
        auto it = this->id_to_media_.constFind(media.id);
        if (it == this->id_to_media_.constEnd()) {
            continue;
        }

        this->existing_media_ids_.insert(media.id);

        int media_idx = it.value();
        this->media_list_[media_idx].in_list = true;

        emit dataChanged(
            this->index(media_idx, 0),
            this->index(media_idx, this->columnCount() - 1)
        );
    }
}

void AnimeSearchModel::rebuildIdHash() {
    this->id_to_media_.clear();
    this->id_to_media_.reserve(this->media_list_.size());

    for (int i = 0; i < this->media_list_.size(); i++) {
        this->id_to_media_.insert(
            this->media_list_.at(i).id,
            i
        );
    }
}
