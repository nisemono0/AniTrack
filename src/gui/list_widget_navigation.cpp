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
        { AppResources::Icons::Playing,       QStringLiteral("Playing"),      Page::Playing },
        { "",                                 "",                             Page::Separator },
        { AppResources::Icons::List,          QStringLiteral("Anime list"),   Page::AnimeList },
        { AppResources::Icons::ClockQuestion, QStringLiteral("Pending list"), Page::PendingList },
        { "",                                 "",                             Page::Separator },
        { AppResources::Icons::Statistics,    QStringLiteral("Statistics"),   Page::Statistics },
        { "",                                 "",                             Page::Separator },
        { AppResources::Icons::Search,        QStringLiteral("Search"),       Page::Search },
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

void ListWidgetNavigation::selectNextPage() {
    int next_page_row = this->currentRow() + 1;

    while (next_page_row < this->count() &&
           this->item(next_page_row)->data(UserRoles::PageType).value<Page>() == Page::Separator) {
        next_page_row++;
    }

    if (next_page_row < this->count()) {
        this->setCurrentRow(next_page_row);
    }
}

void ListWidgetNavigation::selectPreviousPage() {
    int prev_page_row = this->currentRow() - 1;

    while (prev_page_row >= 0 &&
           this->item(prev_page_row)->data(UserRoles::PageType).value<Page>() == Page::Separator) {
        prev_page_row--;
    }

    if (prev_page_row >= 0) {
        this->setCurrentRow(prev_page_row);
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

