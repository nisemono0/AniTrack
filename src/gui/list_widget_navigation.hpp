#pragma once

#include "app/app_resources.hpp"

#include "gui/list_widget_separator_delegate.hpp"

#include <QListWidget>
#include <QListWidgetItem>
#include <QWidget>


class ListWidgetNavigation final : public QListWidget {
    Q_OBJECT;
public:
    explicit ListWidgetNavigation(QWidget *parent = nullptr);
    ~ListWidgetNavigation() = default;

    enum class Page {
        Playing = 0,
        AnimeList,
        Statistics,
        Search,
        Separator
    };
    Q_ENUM(Page);

    enum UserRoles {
        PageType = Qt::UserRole + 1,
    };

signals:
    void pageChanged(Page new_page);

private:
    struct ListItem {
        QString icon_path;
        QString title;
        Page page;
    };
    inline static const QList<ListItem> list_items_ = {
        { AppResources::Icons::Playing,    QStringLiteral("Playing"),    Page::Playing },
        { "",                              "",                           Page::Separator },
        { AppResources::Icons::List,       QStringLiteral("Anime list"), Page::AnimeList },
        { AppResources::Icons::Statistics, QStringLiteral("Statistics"), Page::Statistics },
        { "",                              "",                           Page::Separator },
        { AppResources::Icons::Search,     QStringLiteral("Search"),     Page::Search },
    };

    ListWidgetSeparatorDelegate *separator_delegate_;

    void addItem(const ListItem &list_item);
    void addSeparator();

};

