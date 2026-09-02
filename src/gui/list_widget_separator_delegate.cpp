#include "gui/list_widget_separator_delegate.hpp"

#include "gui/list_widget_navigation.hpp"


ListWidgetSeparatorDelegate::ListWidgetSeparatorDelegate(QObject *parent) : QStyledItemDelegate(parent) {}


void ListWidgetSeparatorDelegate::paint(QPainter *painter, const QStyleOptionViewItem &option, const QModelIndex &index) const {
    ListWidgetNavigation::Page page = index.data(ListWidgetNavigation::UserRoles::PageType).value<ListWidgetNavigation::Page>();

    if (page != ListWidgetNavigation::Page::Separator) {
        QStyledItemDelegate::paint(painter, option, index);
        return;
    }

    QStyle *style = option.widget->style();

    QStyleOptionMenuItem menu_item_opts;
    menu_item_opts.rect = option.rect;
    menu_item_opts.state = QStyle::State_Enabled;
    menu_item_opts.menuItemType = QStyleOptionMenuItem::Separator;

    style->drawPrimitive(QStyle::PE_IndicatorToolBarSeparator, &option, painter, option.widget);
}

QSize ListWidgetSeparatorDelegate::sizeHint(const QStyleOptionViewItem &option, const QModelIndex &index) const {
    ListWidgetNavigation::Page page = index.data(ListWidgetNavigation::UserRoles::PageType).value<ListWidgetNavigation::Page>();

    if (page != ListWidgetNavigation::Page::Separator) {
        return QStyledItemDelegate::sizeHint(option, index);
    }

    QStyle *style = option.widget->style();

    QStyleOptionMenuItem menu_item_opts;
    menu_item_opts.initFrom(option.widget);
    menu_item_opts.state = QStyle::State_Enabled;
    menu_item_opts.menuItemType = QStyleOptionMenuItem::Separator;

    QSize size = style->sizeFromContents(QStyle::CT_MenuItem, &menu_item_opts, QSize(), option.widget);

    return size;
}

