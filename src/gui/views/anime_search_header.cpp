#include "gui/views/anime_search_header.hpp"
#include "gui/views/anime_search_view.hpp"
#include "gui/models/anime_search_model.hpp"

#include "app/app_resources.hpp"

#include "utils/settings.hpp"

#include <QMenu>


AnimeSearchHeader::AnimeSearchHeader(QWidget *parent) : HeaderView(Qt::Horizontal, parent) {
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->setSortIndicatorShown(false);
    this->setStretchLastSection(false);
    this->setMinimumSectionSize(15);
    this->setSectionsMovable(true);

    connect(this, &QHeaderView::customContextMenuRequested, this, &AnimeSearchHeader::onCustomContextMenuRequested);
}

AnimeSearchHeader::~AnimeSearchHeader() {
    Settings::set(Settings::Ui::AnimeSearchView::HeaderState, this->saveState());
}

void AnimeSearchHeader::onCustomContextMenuRequested(const QPoint &pos) {
    QMenu menu(this);

    int columns = this->count();
    int visible_columns = columns - this->hiddenSectionCount();

    for (int index = 0; index < columns; index++) {
        QString column_name = this->model()->headerData(index, Qt::Horizontal, AnimeSearchModel::HeaderRoles::ContextMenuTitle).toString();

        QAction *menu_action = menu.addAction(column_name);
        menu_action->setCheckable(true);
        menu_action->setChecked(
            !this->isSectionHidden(index)
        );

        if (visible_columns == 1 && !this->isSectionHidden(index)) {
            menu_action->setEnabled(false);
        }

        connect(menu_action, &QAction::toggled, this, [this, index] (bool checked) {
            this->setSectionHidden(index, !checked);
        });

        switch (static_cast<AnimeSearchModel::Columns>(index)) {
            case AnimeSearchModel::Columns::Status: {
                menu.addSeparator();
                break;
            }
            case AnimeSearchModel::Columns::InList: {
                menu.addSeparator();
                break;
            }
            case AnimeSearchModel::Columns::EndDate: {
                menu.addSeparator();
                break;
            }
            case AnimeSearchModel::Columns::IsAdult: {
                menu.addSeparator();
                break;
            }
            default:
                break;
        }
    }

    menu.addSeparator();
    QAction *restore_action = menu.addAction(QIcon(AppResources::Icons::Restore), QStringLiteral("Restore default"));
    connect(restore_action, &QAction::triggered, this, [this] {
        this->restorePreviousState();
        // Update tha parent viewport on restore
        AnimeSearchView *parent_view = qobject_cast<AnimeSearchView*>(this->parent());
        if (parent_view) {
            parent_view->viewport()->update();
        }
    });

    menu.exec(this->mapToGlobal(pos));
}
