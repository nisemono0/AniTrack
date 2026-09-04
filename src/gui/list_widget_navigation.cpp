#include "gui/list_widget_navigation.hpp"


ListWidgetNavigation::ListWidgetNavigation(QWidget *parent) : QListWidget(parent) {
    this->separator_delegate_ = new ListWidgetSeparatorDelegate(this);

    this->setItemDelegate(this->separator_delegate_);

    for (const auto &item : this->list_items_) {
        if (item.page == Page::Separator) {
            this->addSeparator();
        } else {
            this->addItem(item);
        }
    }

    connect(this, &QListWidget::currentItemChanged, this, [this] (QListWidgetItem *current, QListWidgetItem *) {
        emit pageChanged(
            current->data(UserRoles::PageType).value<Page>()
        );
    });

}

void ListWidgetNavigation::setCurrentPage(ListWidgetNavigation::Page page) {
    for (int i = 0; i < this->count(); i++) {
        auto *item = this->item(i);
        if (item->data(UserRoles::PageType).value<Page>() == page) {
            this->setCurrentItem(item);
        }
    }
}

void ListWidgetNavigation::addItem(const ListItem &list_item) {
    QListWidgetItem *item = new QListWidgetItem(
        QIcon(list_item.icon_path),
        list_item.title,
        this
    );
    item->setData(
        PageType,
        QVariant::fromValue(list_item.page)
    );
}

void ListWidgetNavigation::addSeparator() {
    QListWidgetItem *separator = new QListWidgetItem(this);
    separator->setFlags(Qt::NoItemFlags);
    separator->setData(
        UserRoles::PageType,
        QVariant::fromValue(Page::Separator)
    );
}

