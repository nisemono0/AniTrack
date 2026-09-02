#include "gui/delegates/anime_list_private_delegate.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListPrivateDelegate::AnimeListPrivateDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListPrivateDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    bool is_private = index.data(AnimeListModel::UserRoles::IsPrivate).toBool();

    if (is_private) {
        this->icon_hidden_.paint(
            painter,
            option.rect
        );
    } else {
        this->icon_visible_.paint(
            painter,
            option.rect
        );
    }
}

