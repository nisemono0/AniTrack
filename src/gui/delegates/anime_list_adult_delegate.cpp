#include "gui/delegates/anime_list_adult_delegate.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListAdultDelegate::AnimeListAdultDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListAdultDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    bool is_adult = index.data(AnimeListModel::UserRoles::IsAdult).toBool();

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

