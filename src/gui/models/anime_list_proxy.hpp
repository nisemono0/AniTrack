#pragma once

#include "gui/pages/anime_list_tab_bar.hpp"

#include <QSortFilterProxyModel>
#include <QObject>


class AnimeListProxy final : public QSortFilterProxyModel {
    Q_OBJECT;
public:
    explicit AnimeListProxy(QObject *parent = nullptr);
    ~AnimeListProxy() = default;

    void setFilterText(const QString &text);
    void setFilterTab(AnimeListTabBar::Tab tab);

protected:
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;

private:
    QString filter_text_;
    AnimeListTabBar::Tab filter_tab_;
};

