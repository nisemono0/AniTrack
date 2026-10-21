#pragma once

#include "gui/models/anime_list_model.hpp"

#include <QTabBar>
#include <QWheelEvent>


class AnimeListTabBar final : public QTabBar {
    Q_OBJECT;
public:
    explicit AnimeListTabBar(QWidget *parent = nullptr);
    ~AnimeListTabBar() = default;

    enum class Tab {
        Watching = 0,
        Completed,
        Paused,
        Dropped,
        Planning,
        All
    };
    Q_ENUM(Tab);

    void setCurrentTab(Tab tab);
    Tab currentTab() const;

    void selectNextTab();
    void selectPreviousTab();

    void setTabName(Tab tab, const QString &name);

public slots:
    void updateTabCount(AnimeListModel::AnimeListStatusCount status_count);

signals:
    void currentTabChanged(AnimeListTabBar::Tab tab);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void initTabs();
};
