#include "gui/delegates/anime_list_pending_delegate.hpp"

#include "gui/models/anime_list_model.hpp"


AnimeListPendingDelegate::AnimeListPendingDelegate(QObject *parent) : QStyledItemDelegate(parent) {}

void AnimeListPendingDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    QStyledItemDelegate::paint(painter, option, index);

    AnilistEntry::PendingOperation pending_operation = index.data(AnimeListModel::UserRoles::PendingOperation)
                                                            .value<AnilistEntry::PendingOperation>();


    switch (pending_operation) {
        case AnilistEntry::PendingOperation::NONE: {
            this->icon_point_gray_.paint(
                painter,
                option.rect
            );
            break;
        }
        case AnilistEntry::PendingOperation::ADD: {
            this->icon_point_blue_.paint(
                painter,
                option.rect
            );
            break;
        }
        case AnilistEntry::PendingOperation::UPDATE: {
            this->icon_point_orange_.paint(
                painter,
                option.rect
            );
            break;
        }
        case AnilistEntry::PendingOperation::REMOVE: {
            this->icon_point_red_.paint(
                painter,
                option.rect
            );
            break;
        }
        default:
            break;
    }
}

