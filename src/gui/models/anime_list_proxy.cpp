#include "gui/models/anime_list_proxy.hpp"
#include "gui/models/anime_list_model.hpp"


AnimeListProxy::AnimeListProxy(QObject *parent) : QSortFilterProxyModel(parent) {
    this->setDynamicSortFilter(true);

    this->setSortRole(AnimeListModel::UserRoles::Sort);
}

void AnimeListProxy::setFilterText(const QString &text) {
    beginFilterChange();
    this->filter_text_ = text;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

void AnimeListProxy::setFilterTab(AnimeListTabBar::Tab tab) {
    beginFilterChange();
    this->filter_tab_ = tab;
    endFilterChange(QSortFilterProxyModel::Direction::Rows);
}

bool AnimeListProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    QModelIndex index = this->sourceModel()->index(
        source_row,
        static_cast<int>(AnimeListModel::Columns::Title),
        source_parent
    );

    if (!index.isValid()) {
        return false;
    }

    AnilistEntry::Status status = index.data(AnimeListModel::UserRoles::EntryStatus).value<AnilistEntry::Status>();

    switch (this->filter_tab_) {
        case AnimeListTabBar::Tab::Watching: {
            if (status != AnilistEntry::Status::CURRENT &&
                status != AnilistEntry::Status::REPEATING) {
                return false;
            }
            break;
        }
        case AnimeListTabBar::Tab::Completed: {
            if (status != AnilistEntry::Status::COMPLETED) {
                return false;
            }
            break;
        }
        case AnimeListTabBar::Tab::Paused: {
            if (status != AnilistEntry::Status::PAUSED) {
                return false;
            }
            break;
        }
        case AnimeListTabBar::Tab::Dropped: {
            if (status != AnilistEntry::Status::DROPPED) {
                return false;
            }
            break;
        }
        case AnimeListTabBar::Tab::Planning: {
            if (status != AnilistEntry::Status::PLANNING) {
                return false;
            }
            break;
        }
        case AnimeListTabBar::Tab::All:
            break;
    }

    AnilistMedia::Title title = index.data(AnimeListModel::UserRoles::MediaTitle).value<AnilistMedia::Title>();

    return title.romaji.contains(this->filter_text_, Qt::CaseInsensitive) ||
           title.english.contains(this->filter_text_, Qt::CaseInsensitive) ||
           title.native.contains(this->filter_text_, Qt::CaseInsensitive);
}

