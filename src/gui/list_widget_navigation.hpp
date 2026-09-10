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
    Q_ENUM(UserRoles);

    // Select the current page
    // This emits the currentItemChanged and pageChanged signals
    void setCurrentPage(ListWidgetNavigation::Page page);

signals:
    void pageChanged(ListWidgetNavigation::Page new_page);

private:
    ListWidgetSeparatorDelegate *separator_delegate_;

    void addItem(const QString &icon_path, const QString &name, ListWidgetNavigation::Page page_type);
    void addSeparator();

};

