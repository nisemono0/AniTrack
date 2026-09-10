#include "gui/list_widget_navigation.hpp"

namespace {
struct ListItem {
    QString icon_path;
    QString title;
    ListWidgetNavigation::Page page_type;
};
}

ListWidgetNavigation::ListWidgetNavigation(QWidget *parent) : QListWidget(parent) {
    this->separator_delegate_ = new ListWidgetSeparatorDelegate(this);

    this->setItemDelegate(this->separator_delegate_);

    static const QList<ListItem> list_items = {
#ifdef ENABLE_WIP_FEATURES
        { AppResources::Icons::Playing,    QStringLiteral("Playing"),    Page::Playing },
        { "",                              "",                           Page::Separator },
#endif // ENABLE_WIP_FEATURES
        { AppResources::Icons::List,       QStringLiteral("Anime list"), Page::AnimeList },
        { AppResources::Icons::Statistics, QStringLiteral("Statistics"), Page::Statistics },
        { "",                              "",                           Page::Separator },
        { AppResources::Icons::Search,     QStringLiteral("Search"),     Page::Search },
    };

    for (const auto &item : list_items) {
        if (item.page_type == Page::Separator) {
            this->addSeparator();
        } else {
            this->addItem(item.icon_path, item.title, item.page_type);
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

void ListWidgetNavigation::addItem(const QString &icon_path, const QString &name, ListWidgetNavigation::Page page_type) {
    QListWidgetItem *item = new QListWidgetItem(
        QIcon(icon_path),
        name,
        this
    );
    item->setData(
        PageType,
        QVariant::fromValue(page_type)
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

