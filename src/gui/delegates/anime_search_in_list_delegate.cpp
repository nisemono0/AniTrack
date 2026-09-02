#include "gui/delegates/anime_search_in_list_delegate.hpp"

#include "gui/models/anime_search_model.hpp"


AnimeSearchInListDelegate::AnimeSearchInListDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeSearchInListDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    bool in_list = index.data(AnimeSearchModel::UserRoles::InList).toBool();

    if (in_list) {
        this->icon_yes_.paint(
            painter,
            option.rect
        );
    } else {
        this->icon_no_.paint(
            painter,
            option.rect
        );
    }
}
