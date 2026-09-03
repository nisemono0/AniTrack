#include "gui/views/anime_search_view.hpp"

#include "app/app_resources.hpp"

#include "utils/settings.hpp"

#include <QScrollBar>
#include <QMenu>


namespace {
// Used to initialize the default header view
struct HeaderSettings {
    AnimeSearchModel::Columns column;
    QHeaderView::ResizeMode resize_mode;
    int padding;
    int resize;
    bool hidden;
};
// Used to initialize the context menu
struct MediaAddAction {
    QString name;
    AnilistEntry::Status status;
    QString icon;
};
} // namespace


AnimeSearchView::AnimeSearchView(QWidget *parent) : QTreeView(parent) {
    this->initView();

    this->setupModel();
    this->setupDelegates();
    this->setupHeader();

    connect(this, &QTreeView::customContextMenuRequested, this, &AnimeSearchView::onCustomContextMenuRequested);
}

void AnimeSearchView::updateUserPreferences(const AnilistAccount::User &user) {
    this->score_format_ = user.score_format;
    this->title_language_ = user.title_language;

    this->anime_search_model_->updateUserPreferences(user);
}

void AnimeSearchView::setExistingMediaIds(const QList<AnilistAnime> &anime_list) {
    this->anime_search_model_->setExistingMediaIds(anime_list);
}

void AnimeSearchView::setMedia(const QList<AnilistMedia> &media_list) {
    this->anime_search_model_->setMedia(media_list);
}

void AnimeSearchView::updateAddedMedia(const QList<AnilistMedia> &media_list) {
    this->anime_search_model_->updateAddedMedia(media_list);
}

void AnimeSearchView::setFilterText(const QString &text) {
    this->anime_search_proxy_->setFilterText(text);
}

void AnimeSearchView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        QScrollBar *h_scroll = this->horizontalScrollBar();
        int delta = event->angleDelta().y();
        h_scroll->setValue(
            h_scroll->value() - delta
        );
        event->accept();
        return;
    }
    QTreeView::wheelEvent(event);
}

void AnimeSearchView::initView() {
    this->anime_search_model_ = new AnimeSearchModel(this);
    this->anime_search_proxy_ = new AnimeSearchProxy(this);
    this->anime_search_header_ = new AnimeSearchHeader(this);

    this->adult_delegate_ = new AnimeSearchAdultDelegate(this);
    this->in_list_delegate_ = new AnimeSearchInListDelegate(this);

    this->setHeader(this->anime_search_header_);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void AnimeSearchView::setupModel() {
    this->anime_search_proxy_->setSourceModel(this->anime_search_model_);
    this->setModel(this->anime_search_proxy_);

    this->sortByColumn(
        static_cast<int>(AnimeSearchModel::Columns::Title),
        Qt::AscendingOrder
    );
}

void AnimeSearchView::setupDelegates() {
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeSearchModel::Columns::IsAdult),
        this->adult_delegate_
    );

    this->setItemDelegateForColumn(
        static_cast<int>(AnimeSearchModel::Columns::InList),
        this->in_list_delegate_
    );
}

void AnimeSearchView::setupHeader() {
    static const QList<HeaderSettings> header_settings{
        // column, resize mode, padding, resize, hidden
        { AnimeSearchModel::Columns::Title,     QHeaderView::Interactive, 15, 350, false },
        { AnimeSearchModel::Columns::Format,    QHeaderView::Interactive, 15, 75,  false },
        { AnimeSearchModel::Columns::Episodes,  QHeaderView::Interactive, 15, 80,  false },
        { AnimeSearchModel::Columns::Score,     QHeaderView::Interactive, 15, 55,  false },
        { AnimeSearchModel::Columns::Season,    QHeaderView::Interactive, 15, 90,  false },
        { AnimeSearchModel::Columns::Status,    QHeaderView::Interactive, 15, 100, false },
        { AnimeSearchModel::Columns::InList,    QHeaderView::Interactive, 15, 45,  false },
        { AnimeSearchModel::Columns::IsAdult,   QHeaderView::Interactive, 15, 45,  false },
        { AnimeSearchModel::Columns::StartDate, QHeaderView::Interactive, 15, 90,  true },
        { AnimeSearchModel::Columns::EndDate,   QHeaderView::Interactive, 15, 90,  true },
    };

    for (const auto &setting : header_settings) {
        this->anime_search_header_->setSectionResizeMode(static_cast<int>(setting.column), setting.resize_mode);
        this->anime_search_header_->setSectionPadding(static_cast<int>(setting.column), setting.padding);
        this->anime_search_header_->resizeSection(static_cast<int>(setting.column), setting.resize);
        this->anime_search_header_->setSectionHidden(static_cast<int>(setting.column), setting.hidden);
    }

    this->anime_search_header_->saveCurrentState();

    QByteArray saved_header_state = Settings::get(Settings::Ui::AnimeSearchView::HeaderState, QByteArray());
    if (!saved_header_state.isEmpty()) {
        this->anime_search_header_->restoreState(saved_header_state);
    }
}

void AnimeSearchView::onCustomContextMenuRequested(const QPoint &pos) {
    const QModelIndex index = this->indexAt(pos);

    if (!index.isValid()) {
        return;
    }

    if (!this->selectionModel()->isSelected(index)) {
        this->selectionModel()->select(
            index,
            QItemSelectionModel::Select | QItemSelectionModel::Rows
        );
    }

    QList<AnilistMedia> selected_media;

    const QModelIndexList selected_rows = this->selectionModel()->selectedRows();
    for (const auto &row : selected_rows) {
        selected_media.append(
            row.data(AnimeSearchModel::UserRoles::Media).value<AnilistMedia>()
        );
    }

    QMenu menu(this);

    bool is_single_media = selected_media.size() == 1;
    if (is_single_media) {
        QAction *info_add_action = menu.addAction(
            QIcon(AppResources::Icons::Info),
            QStringLiteral("Information")
        );
        connect(info_add_action, &QAction::triggered, this, [this, selected_media] {
            emit requestShowAnimeInfoSearchDialog(selected_media.constFirst());
        });
        menu.addSeparator();
    }

    QMenu *add_to_list_menu = menu.addMenu(
        QIcon(AppResources::Icons::Edit),
        QStringLiteral("Add to list")
    );
    if (is_single_media && selected_media.constFirst().in_list) {
        add_to_list_menu->setEnabled(false);
    } else {
        static const QList<MediaAddAction> media_add_actions{
            { QStringLiteral("Watching"),   AnilistEntry::Status::CURRENT,   AppResources::Icons::Edit },
            { QStringLiteral("Planning"),   AnilistEntry::Status::PLANNING,  AppResources::Icons::Edit },
            { QStringLiteral("Completed"),  AnilistEntry::Status::COMPLETED, AppResources::Icons::Edit },
            { QStringLiteral("Dropped"),    AnilistEntry::Status::DROPPED,   AppResources::Icons::Edit },
            { QStringLiteral("Paused"),     AnilistEntry::Status::PAUSED,    AppResources::Icons::Edit },
            { QStringLiteral("Rewatching"), AnilistEntry::Status::REPEATING, AppResources::Icons::Edit },
        };

        for (const auto &action : media_add_actions) {
            QAction *menu_action = add_to_list_menu->addAction(
                QIcon(action.icon),
                action.name
            );
            connect(menu_action, &QAction::triggered, this, [this, selected_media, status = action.status] {
                emit requestAddMedia(selected_media, status);
            });
        }
    }

    menu.exec(this->viewport()->mapToGlobal(pos));
}
