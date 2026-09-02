#include "gui/delegates/anime_search_adult_delegate.hpp"

#include "gui/models/anime_search_model.hpp"


AnimeSearchAdultDelegate::AnimeSearchAdultDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeSearchAdultDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    bool is_adult = index.data(AnimeSearchModel::UserRoles::IsAdult).toBool();

    if (is_adult) {
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

