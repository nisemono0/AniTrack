#include "gui/views/anime_search_view.hpp"

#include "app/app_resources.hpp"

#include "utils/settings.hpp"
#include "utils/dialog.hpp"
#include "utils/web.hpp"

#include <QScrollBar>
#include <QMenu>
#include <QClipboard>


namespace {
// Used to initialize the default header view
struct HeaderSettings {
    AnimeSearchModel::Columns column;
    QHeaderView::ResizeMode resize_mode;
    int padding;
    int resize;
    bool hidden;
};
// Used to initialize the copy actions
enum class CopyType {
    AnilistUrl,
    TitleRomaji,
    TitleNative,
    TitleEnglish
};
struct CopyActions {
    QString name;
    CopyType type;
    QString icon;
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

    this->applySettings();

    this->setupModel();
    this->setupDelegates();
    this->setupHeader();

    connect(this, &QTreeView::customContextMenuRequested, this, &AnimeSearchView::onCustomContextMenuRequested);
    connect(App::instance()->settings(), &AppSettings::settingsChanged, this, &AnimeSearchView::applySettings);
}

AnimeSearchView::~AnimeSearchView() {
    if (this->save_load_header_state_) {
        Settings::set(
            Settings::Ui::Window::AnimeSearch::HeaderState,
            this->anime_search_header_->saveState()
        );
    }
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

void AnimeSearchView::updateAddedAnime(const QList<AnilistAnime> &anime_list) {
    this->anime_search_model_->updateAddedAnime(anime_list);
}

void AnimeSearchView::setFilterText(const QString &text) {
    this->anime_search_proxy_->setFilterText(text);
}

void AnimeSearchView::wheelEvent(QWheelEvent *event) {
    if (event->modifiers().testFlag(Qt::ShiftModifier)) {
        QScrollBar *h_scroll = this->horizontalScrollBar();
        const int delta = event->angleDelta().y();
        h_scroll->setValue(
            h_scroll->value() - delta
        );
        event->accept();
        return;
    }
    QTreeView::wheelEvent(event);
}

void AnimeSearchView::keyPressEvent(QKeyEvent *event) {
    QTreeView::keyPressEvent(event);

    switch (event->key()) {
        case Qt::Key_Enter:
        case Qt::Key_Return: {
            const auto selected_media = this->selectedMedia();
            if (selected_media.isEmpty()) {
                event->ignore();
            } else {
                emit requestShowAnimeInfoEditDialog(selected_media.constFirst());
            }
            break;
        }
        case Qt::Key_I: {
            if (event->modifiers().testFlag(Qt::ControlModifier)) {
                const auto selected_media = this->selectedMedia();
                if (!selected_media.isEmpty()) {
                    emit requestShowAnimeInfoEditDialog(selected_media.constFirst());
                }
                event->accept();
            } else {
                event->ignore();
            }
            break;
        }
        default:
            event->ignore();
    }
}

bool AnimeSearchView::viewportEvent(QEvent *event) {
    switch (event->type()) {
        case QEvent::ToolTip: {
            auto *tooltip_event = static_cast<QHelpEvent*>(event);

            const QModelIndex index = this->indexAt(tooltip_event->pos());
            if (!index.isValid()) {
                this->last_tooltip_media_id_ = AnilistMedia::InvalidId;
                this->anime_tooltip_->hide();
                return true;
            }

            const int index_media_id = index.data(AnimeSearchModel::UserRoles::MediaId).toInt();
            if (this->last_tooltip_media_id_ != index_media_id) {
                this->last_tooltip_media_id_ = index_media_id;
                this->anime_tooltip_->updateAnimeInfo(
                    index.data(AnimeSearchModel::UserRoles::ToolTipCoverUrl).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipTitle).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipFormat).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipEpisodes).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipStatus).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipSeason).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipGenres).toString(),
                    index.data(AnimeSearchModel::UserRoles::ToolTipScore).toString()
                );
            }

            this->anime_tooltip_->showAtPosition(tooltip_event->globalPos());

            return true;
        }
        case QEvent::MouseMove: {
            auto *mouse_event = static_cast<QMouseEvent*>(event);
            this->anime_tooltip_->updatePosition(mouse_event->globalPosition().toPoint());
            return QTreeView::viewportEvent(event);
        }
        default: {
            this->anime_tooltip_->hide();
            return QTreeView::viewportEvent(event);
        }
    }
}

void AnimeSearchView::initView() {
    this->anime_search_model_ = new AnimeSearchModel(this);
    this->anime_search_proxy_ = new AnimeSearchProxy(this);
    this->anime_search_header_ = new AnimeSearchHeader(this);

    this->adult_delegate_ = new AnimeSearchAdultDelegate(this);
    this->in_list_delegate_ = new AnimeSearchInListDelegate(this);

    this->anime_tooltip_ = new AnimeTooltip(this);

    this->setHeader(this->anime_search_header_);
    this->setContextMenuPolicy(Qt::CustomContextMenu);

    this->viewport()->setMouseTracking(true);
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
        { AnimeSearchModel::Columns::Status,    QHeaderView::Interactive, 15, 90,  false },
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

    // Save the above state as default
    this->anime_search_header_->saveCurrentState();

    // load the saved header state if option is set
    if (this->save_load_header_state_) {
        const QByteArray saved_header_state = Settings::get(Settings::Ui::Window::AnimeSearch::HeaderState, QByteArray());

        if (!saved_header_state.isEmpty()) {
            this->anime_search_header_->restoreState(saved_header_state);
        }
    }
}

QList<AnilistMedia> AnimeSearchView::selectedMedia() {
    QList<AnilistMedia> selected_media;

    const QModelIndexList selected_rows = this->selectionModel()->selectedRows();
    for (const auto &row : selected_rows) {
        selected_media.append(
            row.data(AnimeSearchModel::UserRoles::Media).value<AnilistMedia>()
        );
    }

    return selected_media;
}

void AnimeSearchView::addInfoAddActions(QMenu &menu, const AnilistMedia &selected_media) {
    QAction *info_add_action = menu.addAction(
        QIcon(AppResources::Icons::Info),
        QStringLiteral("Information")
    );
    connect(info_add_action, &QAction::triggered, this, [this, selected_media] {
        emit requestShowAnimeInfoEditDialog(selected_media);
    });
}

void AnimeSearchView::addCopyActions(QMenu &menu, const AnilistMedia &selected_media) {
    static const QList<CopyActions> copy_actions{
        { QStringLiteral("AniList link"),  CopyType::AnilistUrl,   AppResources::Icons::Copy },
        { QStringLiteral("Romaji title"),  CopyType::TitleRomaji,  AppResources::Icons::Copy },
        { QStringLiteral("Native title"),  CopyType::TitleNative,  AppResources::Icons::Copy },
        { QStringLiteral("English title"), CopyType::TitleEnglish, AppResources::Icons::Copy },
    };

    auto *copy_menu = menu.addMenu(
        QIcon(AppResources::Icons::Copy),
        QStringLiteral("Copy")
    );
    for (const auto &action : copy_actions) {
        QAction *menu_action = copy_menu->addAction(
            QIcon(action.icon),
            action.name
        );

        if (action.type == CopyType::AnilistUrl) {
            copy_menu->addSeparator();
        }

        connect(menu_action, &QAction::triggered, this, [this, selected_media, type = action.type] {
            switch (type) {
                case CopyType::AnilistUrl: {
                    QApplication::clipboard()->setText(selected_media.site_url);
                    break;
                }
                case CopyType::TitleRomaji: {
                    QApplication::clipboard()->setText(selected_media.title.romaji);
                    break;
                }
                case CopyType::TitleNative: {
                    QApplication::clipboard()->setText(selected_media.title.native);
                    break;
                }
                case CopyType::TitleEnglish: {
                    QApplication::clipboard()->setText(selected_media.title.english);
                    break;
                }
                default:
                    break;
            }
        });
    }
}

void AnimeSearchView::addLinkActions(QMenu &menu, const QList<AnilistMedia> &selected_media) {
    QAction *open_url_action = menu.addAction(
        QIcon(AppResources::Icons::Link),
        QStringLiteral("Open AniList url")
    );

    connect(open_url_action, &QAction::triggered, this, [this, selected_media] {
        for (const auto &media : selected_media) {
            if (!WebUtils::openUrl(media.site_url)) {
                DialogUtils::warning(
                    QStringLiteral("Open AniList url"),
                    QStringLiteral("Failed to open: %1").arg(media.site_url),
                    this
                );
            }
        }
    });
}

void AnimeSearchView::addAddToListActions(QMenu &menu, const QList<AnilistMedia> &selected_media) {
    static const QList<MediaAddAction> media_add_actions{
        { QStringLiteral("Watching"),   AnilistEntry::Status::CURRENT,   AppResources::Icons::CirclePlay },
        { QStringLiteral("Planning"),   AnilistEntry::Status::PLANNING,  AppResources::Icons::CircleClock },
        { QStringLiteral("Completed"),  AnilistEntry::Status::COMPLETED, AppResources::Icons::CircleCheck },
        { QStringLiteral("Dropped"),    AnilistEntry::Status::DROPPED,   AppResources::Icons::CircleX },
        { QStringLiteral("Paused"),     AnilistEntry::Status::PAUSED,    AppResources::Icons::CirclePause },
        { QStringLiteral("Rewatching"), AnilistEntry::Status::REPEATING, AppResources::Icons::CircleReplay },
    };

    for (const auto &action : media_add_actions) {
        QAction *menu_action = menu.addAction(
            QIcon(action.icon),
            action.name
        );
        connect(menu_action, &QAction::triggered, this, [this, selected_media, status = action.status] {
            emit requestAddMedia(selected_media, status);
        });
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

    QList<AnilistMedia> selected_media = this->selectedMedia();

    QMenu menu(this);

    const bool is_single_media = selected_media.size() == 1;
    if (is_single_media) {
        this->addInfoAddActions(menu, selected_media.constFirst());
        menu.addSeparator();

        this->addCopyActions(menu, selected_media.constFirst());
        menu.addSeparator();
    }

    this->addLinkActions(menu, selected_media);
    menu.addSeparator();

    QMenu *add_to_list_menu = menu.addMenu(
        QIcon(AppResources::Icons::CirclePlus),
        QStringLiteral("Add to list")
    );
    if (is_single_media && selected_media.constFirst().in_list) {
        add_to_list_menu->setEnabled(false);
    } else {
        this->addAddToListActions(*add_to_list_menu, selected_media);
    }

    menu.exec(this->viewport()->mapToGlobal(pos));
}

void AnimeSearchView::applySettings() {
    this->save_load_header_state_ = Settings::get(Settings::Ui::Window::SaveLoadSearchHeaderState, false);
}
