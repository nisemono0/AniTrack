#pragma once

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

signals:
    void currentTabChanged(Tab tab);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    void initTabs();
};
