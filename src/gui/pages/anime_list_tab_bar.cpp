#include "gui/pages/anime_list_tab_bar.hpp"

#include <QMetaEnum>


AnimeListTabBar::AnimeListTabBar(QWidget *parent) : QTabBar(parent) {
    this->setFocusPolicy(Qt::NoFocus);
    this->setExpanding(false);
    this->setContentsMargins(0, 0, 0, 0);

    connect(this, &QTabBar::currentChanged, this, [this] (int index) {
        emit currentTabChanged(static_cast<Tab>(index));
    });

    this->initTabs();
}

void AnimeListTabBar::setCurrentTab(Tab tab) {
    this->setCurrentIndex(static_cast<int>(tab));
}

AnimeListTabBar::Tab AnimeListTabBar::currentTab() const {
    return static_cast<Tab>(this->currentIndex());
}

void AnimeListTabBar::selectNextTab() {
    int next = this->currentIndex() + 1;
    if (next < this->count()) {
        this->setCurrentIndex(next);
    }
}

void AnimeListTabBar::selectPreviousTab() {
    int prev = this->currentIndex() - 1;
    if (prev >= 0) {
        this->setCurrentIndex(prev);
    }
}

void AnimeListTabBar::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void AnimeListTabBar::initTabs() {
    const QMetaEnum meta_enum = QMetaEnum::fromType<AnimeListTabBar::Tab>();

    for (int i = 0; i < meta_enum.keyCount(); i++) {
        Tab tab = static_cast<AnimeListTabBar::Tab>(meta_enum.value(i));
        QString name = QString::fromLatin1(meta_enum.key(i));

        this->setTabData(
            this->addTab(name),
            static_cast<int>(tab)
        );
    }
}
