#include "gui/models/anime_search_proxy.hpp"
#include "gui/models/anime_search_model.hpp"


AnimeSearchProxy::AnimeSearchProxy(QObject *parent) : QSortFilterProxyModel(parent) {
    this->setDynamicSortFilter(true);

    this->setSortRole(AnimeSearchModel::UserRoles::Sort);
}

void AnimeSearchProxy::setFilterText(const QString &text) {
    beginFilterChange();
    this->filter_text_ = text;
    endFilterChange();
}

bool AnimeSearchProxy::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const {
    QModelIndex index = this->sourceModel()->index(
        source_row,
        static_cast<int>(AnimeSearchModel::Columns::Title),
        source_parent
    );

    if (!index.isValid()) {
        return false;
    }

    AnilistMedia::Title title = index.data(AnimeSearchModel::UserRoles::Title).value<AnilistMedia::Title>();

    return title.romaji.contains(this->filter_text_, Qt::CaseInsensitive) ||
           title.english.contains(this->filter_text_, Qt::CaseInsensitive) ||
           title.native.contains(this->filter_text_, Qt::CaseInsensitive);
}

