#include "gui/pages/anime_list_tab_bar.hpp"

#include "app/app_resources.hpp"

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
    const int next = this->currentIndex() + 1;
    if (next < this->count()) {
        this->setCurrentIndex(next);
    }
}

void AnimeListTabBar::selectPreviousTab() {
    const int prev = this->currentIndex() - 1;
    if (prev >= 0) {
        this->setCurrentIndex(prev);
    }
}

void AnimeListTabBar::setTabName(Tab tab, const QString &name) {
    this->setTabText(
        static_cast<int>(tab),
        name
    );
}

void AnimeListTabBar::updateTabCount(AnimeListModel::AnimeListStatusCount status_count) {
    const QMetaEnum meta_enum = QMetaEnum::fromType<AnimeListTabBar::Tab>();

    for (int i = 0; i < meta_enum.keyCount(); i++) {
        const Tab tab = static_cast<AnimeListTabBar::Tab>(meta_enum.value(i));

        switch (tab) {
            case Tab::Watching: {
                this->setTabName(
                    tab,
                    QStringLiteral("%1 (%2)").arg(QString::fromLatin1(meta_enum.key(i)))
                                             .arg(status_count.watching)
                );
                break;
            }
            case Tab::Completed: {
                this->setTabName(
                    tab,
                    QStringLiteral("%1 (%2)").arg(QString::fromLatin1(meta_enum.key(i)))
                                             .arg(status_count.completed)
                );
                break;
            }
            case Tab::Paused: {
                this->setTabName(
                    tab,
                    QStringLiteral("%1 (%2)").arg(QString::fromLatin1(meta_enum.key(i)))
                                             .arg(status_count.paused)
                );
                break;
            }
            case Tab::Dropped: {
                this->setTabName(
                    tab,
                    QStringLiteral("%1 (%2)").arg(QString::fromLatin1(meta_enum.key(i)))
                                             .arg(status_count.dropped)
                );
                break;
            }
            case Tab::Planning: {
                this->setTabName(
                    tab,
                    QStringLiteral("%1 (%2)").arg(QString::fromLatin1(meta_enum.key(i)))
                                             .arg(status_count.planning)
                );
                break;
            }
            case Tab::All: {
                this->setTabName(
                    tab,
                    QStringLiteral("%1 (%2)").arg(QString::fromLatin1(meta_enum.key(i)))
                                             .arg(status_count.all)
                );
                break;
            }
            default:
                break;
        }
    }
}

void AnimeListTabBar::wheelEvent(QWheelEvent *event) {
    event->ignore();
}

void AnimeListTabBar::initTabs() {
    const QMetaEnum meta_enum = QMetaEnum::fromType<AnimeListTabBar::Tab>();

    for (int i = 0; i < meta_enum.keyCount(); i++) {
        const Tab tab = static_cast<AnimeListTabBar::Tab>(meta_enum.value(i));
        const QString name = QStringLiteral("%1 (%2)").arg(
            QString::fromLatin1(meta_enum.key(i)),
            QStringLiteral("?")
        );

        this->setTabData(
            this->addTab(name),
            static_cast<int>(tab)
        );

        switch (tab) {
            case Tab::Watching: {
                this->setTabIcon(i, QIcon(AppResources::Icons::CirclePlay));
                break;
            }
            case Tab::Completed: {
                this->setTabIcon(i, QIcon(AppResources::Icons::CircleCheck));
                break;
            }
            case Tab::Paused: {
                this->setTabIcon(i, QIcon(AppResources::Icons::CirclePause));
                break;
            }
            case Tab::Dropped: {
                this->setTabIcon(i, QIcon(AppResources::Icons::CircleX));
                break;
            }
            case Tab::Planning: {
                this->setTabIcon(i, QIcon(AppResources::Icons::CircleClock));
                break;
            }
            case Tab::All: {
                this->setTabIcon(i, QIcon(AppResources::Icons::CircleAsterisk));
                break;
            }
        }
    }
}

