#include "gui/views/anime_list_view.hpp"

#include "gui/dialogs/set_episode_dialog.hpp"
#include "gui/dialogs//set_score_dialog.hpp"

#include "app/app_resources.hpp"

#include "utils/settings.hpp"

#include <QScrollBar>
#include <QMenu>

namespace {
// Used to initialize the default header view
struct HeaderSettings {
    AnimeListModel::Columns column;
    QHeaderView::ResizeMode resize_mode;
    int padding;
    int resize;
    bool hidden;
};
// Used to initialize the info/edit actions
struct InfoEditAction{
    QString name;
    AnimeInfoEditDialog::Page page;
    QString icon;
};
// Used to initialize the undo/redo actions
enum class UndoRedoType {
    Undo,
    Redo
};
struct UndoRedoAction{
    QString name;
    UndoRedoType type;
    QString icon;
};
// Used to initialize the episode actions
enum class EpisodeActionType {
    IncreaseEpisode,
    DecreaseEpisode,
    SetEpisode
};
struct EpisodeAction {
    QString name;
    EpisodeActionType type;
    QString icon;
};
// Used to initialize the date context menu
enum class DateActionType {
    Clear,
    SetFromMediaStart,
    SetFromMediaEnd,
    SetFromLastUpdated
};
struct DateMenuAction {
    QString name;
    DateActionType type;
    QString icon;
};
// Used to initialize the score context menu
struct ScoreAction {
    QString name;
    QString icon;
};
// Used to initialize the status context menu
struct StatusMenuAction {
    QString name;
    AnilistEntry::Status status;
    QString icon;
};
} // namespace


AnimeListView::AnimeListView(QWidget *parent) : QTreeView(parent) {
    this->initView();

    this->setupModel();
    this->setupDelegates();
    this->setupHeader();

    connect(this, &QTreeView::customContextMenuRequested, this, &AnimeListView::onCustomContextMenuRequested);
}

void AnimeListView::updateUserPreferences(const AnilistAccount::User &user) {
    this->anime_list_model_->updateUserPreferences(user);
    this->score_format_ = user.score_format;
    this->title_language_ = user.title_language;
}

void AnimeListView::setAnime(const QList<AnilistAnime> &anime_list) {
    this->anime_list_model_->setAnime(anime_list);
}

void AnimeListView::updateAnime(const QList<AnilistAnime> &anime_list) {
    this->anime_list_model_->updateAnime(anime_list);
}

void AnimeListView::deleteAnime(const QList<int> &local_ids) {
    this->anime_list_model_->deleteAnime(local_ids);
}

void AnimeListView::addAnime(const QList<AnilistAnime> &anime_list) {
    this->anime_list_model_->addAnime(anime_list);
}

void AnimeListView::stopEditing() {
    this->progress_delegate_->stopEditing();
    this->score_delegate_->stopEditing();
    this->status_delegate_->stopEditing();
    this->started_delegate_->stopEditing();
    this->completed_delegate_->stopEditing();
}

void AnimeListView::setFilterTab(AnimeListTabBar::Tab tab) {
    this->stopEditing();
    this->anime_list_proxy_->setFilterTab(tab);
}

void AnimeListView::setFilterText(const QString &text) {
    this->anime_list_proxy_->setFilterText(text);
}

void AnimeListView::wheelEvent(QWheelEvent *event) {
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

void AnimeListView::initView() {
    this->anime_list_model_ = new AnimeListModel(this);
    this->anime_list_proxy_ = new AnimeListProxy(this);
    this->anime_list_header_ = new AnimeListHeader(this);

    this->pending_delegate_ = new AnimeListPendingDelegate(this);
    this->progress_delegate_ = new AnimeListProgressDelegate(this);
    this->adult_delegate_ = new AnimeListAdultDelegate(this);
    this->private_delegate_ = new AnimeListPrivateDelegate(this);
    this->score_delegate_ = new AnimeListScoreDelegate(this);
    this->status_delegate_ = new AnimeListStatusDelegate(this);
    this->started_delegate_ = new AnimeListStartedDateDelegate(this);
    this->completed_delegate_ = new AnimeListCompletedDateDelegate(this);

    this->setHeader(this->anime_list_header_);
    this->setContextMenuPolicy(Qt::CustomContextMenu);
}

void AnimeListView::setupModel() {
    this->anime_list_proxy_->setSourceModel(this->anime_list_model_);
    this->setModel(this->anime_list_proxy_);

    this->sortByColumn(
        static_cast<int>(AnimeListModel::Columns::Title),
        Qt::AscendingOrder
    );

    this->anime_list_proxy_->setFilterTab(AnimeListTabBar::Tab::Watching);
}

void AnimeListView::setupDelegates() {
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::PendingIcon),
        this->pending_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::Progress),
        this->progress_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::IsAdult),
        this->adult_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::IsPrivate),
        this->private_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::Score),
        this->score_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::EntryStatus),
        this->status_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::StartedAt),
        this->started_delegate_
    );
    this->setItemDelegateForColumn(
        static_cast<int>(AnimeListModel::Columns::CompletedAt),
        this->completed_delegate_
    );

    connect(this->progress_delegate_, &AnimeListProgressDelegate::requestDecreaseAnimeProgress, this, [this] (const AnilistAnime &anime) {
        emit requestDecreaseAnimeProgress({anime});
    });
    connect(this->progress_delegate_, &AnimeListProgressDelegate::requestIncreaseAnimeProgress, this, [this] (const AnilistAnime &anime) {
        emit requestIncreaseAnimeProgress({anime});
    });
    connect(this->score_delegate_, &AnimeListScoreDelegate::requestSetAnimeScore, this, [this] (const AnilistAnime &anime, int score) {
        emit requestSetAnimeScore({anime}, score);
    });
    connect(
        this->status_delegate_, &AnimeListStatusDelegate::requestSetAnimeStatus,
        this, [this] (const AnilistAnime &anime, AnilistEntry::Status status) {
            emit requestSetAnimeStatus({anime}, status);
        }
    );
    connect(
        this->started_delegate_, &AnimeListStartedDateDelegate::requestSetAnimeStartedDate,
        this, [this] (const AnilistAnime &anime, const QDate &date) {
            emit requestSetAnimeStartedDate({anime}, date);
        }
    );
    connect(
        this->completed_delegate_, &AnimeListCompletedDateDelegate::requestSetAnimeCompletedDate,
        this, [this] (const AnilistAnime &anime, const QDate &date) {
            emit requestSetAnimeCompletedDate({anime}, date);
        }
    );
}

void AnimeListView::setupHeader() {
    static const QList<HeaderSettings> header_settings{
        // column, resize mode, padding, resize, hidden
        { AnimeListModel::Columns::PendingIcon,  QHeaderView::Fixed,       0,  15,  false },
        { AnimeListModel::Columns::Title,        QHeaderView::Interactive, 15, 350, false },
        { AnimeListModel::Columns::Progress,     QHeaderView::Interactive, 15, 115, false },
        { AnimeListModel::Columns::Score,        QHeaderView::Interactive, 15, 40,  false },
        { AnimeListModel::Columns::Format,       QHeaderView::Interactive, 15, 75,  false },
        { AnimeListModel::Columns::Season,       QHeaderView::Interactive, 15, 90,  false },
        { AnimeListModel::Columns::EntryStatus,  QHeaderView::Interactive, 15, 80,  false },
        { AnimeListModel::Columns::MediaStatus,  QHeaderView::Interactive, 15, 100, true  },
        { AnimeListModel::Columns::LastUpdated,  QHeaderView::Interactive, 15, 90,  false },
        { AnimeListModel::Columns::StartedAt,    QHeaderView::Interactive, 15, 90,  true  },
        { AnimeListModel::Columns::CompletedAt,  QHeaderView::Interactive, 15, 90,  true  },
        { AnimeListModel::Columns::IsAdult,      QHeaderView::Interactive, 15, 50,  true  },
        { AnimeListModel::Columns::IsPrivate,    QHeaderView::Interactive, 15, 50,  true  },
        { AnimeListModel::Columns::RewatchCount, QHeaderView::Interactive, 15, 80,  true  }
    };

    for (const auto &setting : header_settings) {
        this->anime_list_header_->setSectionResizeMode(static_cast<int>(setting.column), setting.resize_mode);
        this->anime_list_header_->setSectionPadding(static_cast<int>(setting.column), setting.padding);
        this->anime_list_header_->resizeSection(static_cast<int>(setting.column), setting.resize);
        this->anime_list_header_->setSectionHidden(static_cast<int>(setting.column), setting.hidden);
    }

    // Save the above state as default;
    this->anime_list_header_->saveCurrentState();

    // Set the previous user's header state if any, while keeping the above as default
    QByteArray saved_header_state = Settings::get(Settings::Ui::AnimeListView::HeaderState, QByteArray());
    if (!saved_header_state.isEmpty()) {
        this->anime_list_header_->restoreState(saved_header_state);
    }
}

void AnimeListView::addInfoEditActions(QMenu &menu, const AnilistAnime &selected_anime) {
    static const QList<InfoEditAction> anime_info_edit_actions{
        { QStringLiteral("Information"), AnimeInfoEditDialog::Page::Info, AppResources::Icons::Info },
        { QStringLiteral("Edit"),        AnimeInfoEditDialog::Page::Edit, AppResources::Icons::Edit },
    };

    for (const auto &action : anime_info_edit_actions) {
        QAction *menu_action = menu.addAction(
            QIcon(action.icon),
            action.name
        );
        connect(menu_action, &QAction::triggered, this, [this, selected_anime, page = action.page] {
            emit requestShowAnimeInfoEditDialog(selected_anime, page);
        });
    }
}

void AnimeListView::addStateHistoryActions(QMenu &menu, const AnilistAnime &selected_anime) {
    QMenu *history_menu = menu.addMenu(
        QIcon(AppResources::Icons::History),
        QStringLiteral("History")
    );

    const auto &state_history = selected_anime.entry.stateHistory();
    if (state_history.size() == 1) {
        history_menu->setEnabled(false);
        return;
    }

    const int current_state_index = selected_anime.entry.currentStateIndex();

    for (int i = 0; i < state_history.size(); i++) {
        QAction *menu_action = history_menu->addAction(
            QIcon(AppResources::Icons::History),
            QStringLiteral("%1: %2").arg(i).arg(state_history.at(i).name)
        );

        if (i == current_state_index) {
            menu_action->setEnabled(false);
        }

        connect(menu_action, &QAction::triggered, this, [this, selected_anime, i] {
            emit requestRestoreAnimeState(selected_anime, i);
        });
    }
}

void AnimeListView::addUndoRedoActions(QMenu &menu, const QList<AnilistAnime> &selected_anime) {
    static const QList<UndoRedoAction> undo_redo_actions{
        { QStringLiteral("Undo"), UndoRedoType::Undo, AppResources::Icons::Undo },
        { QStringLiteral("Redo"), UndoRedoType::Redo, AppResources::Icons::Redo },
    };

    for (const auto &action : undo_redo_actions) {
        QAction *menu_action = menu.addAction(
            QIcon(action.icon),
            action.name
        );
        if (action.type == UndoRedoType::Undo &&
            selected_anime.size() == 1 &&
            !selected_anime.constFirst().entry.canUndoState()) {
            menu_action->setEnabled(false);
        }
        if (action.type == UndoRedoType::Redo &&
            selected_anime.size() == 1 &&
            !selected_anime.constFirst().entry.canRedoState()) {
            menu_action->setEnabled(false);
        }

        connect(menu_action, &QAction::triggered, this, [this, selected_anime, type = action.type] {
            switch (type) {
                case UndoRedoType::Undo: {
                    emit requestUndoAnimeState(selected_anime);
                    break;
                }
                case UndoRedoType::Redo: {
                    emit requestRedoAnimeState(selected_anime);
                    break;
                }
                default:
                    break;
            }
        });
    }
}

void AnimeListView::addDeleteActions(QMenu &menu, const QList<AnilistAnime> &selected_anime) {
    const auto *remove = menu.addAction(
        QIcon(AppResources::Icons::Trash),
        QStringLiteral("Mark for removal")
    );
    connect(remove, &QAction::triggered, this, [this, selected_anime] {
        emit requestMarkAnimeForRemoval(selected_anime);
    });
}

void AnimeListView::addEpisodeActions(QMenu &menu, const QList<AnilistAnime> &selected_anime) {
    static const QList<EpisodeAction> episode_actions{
        { QStringLiteral("Increase episode"), EpisodeActionType::IncreaseEpisode, AppResources::Icons::Plus },
        { QStringLiteral("Decrease episode"), EpisodeActionType::DecreaseEpisode, AppResources::Icons::Minus },
        { QStringLiteral("Set episode"),      EpisodeActionType::SetEpisode,      AppResources::Icons::Numbers }
    };

    for (const auto &action : episode_actions) {
        QAction *menu_action = menu.addAction(
            QIcon(action.icon),
            action.name
        );
        if (action.type == EpisodeActionType::SetEpisode) {
            connect(menu_action, &QAction::triggered, this, [this, selected_anime] {
                auto *episode_dialog = new SetEpisodeDialog(selected_anime, this->title_language_, this);
                connect(episode_dialog, &SetEpisodeDialog::episodeAccepted, this, [this, selected_anime] (int episode) {
                    emit requestSetAnimeProgress(selected_anime, episode);
                });
                episode_dialog->open();
            });
        } else {
            connect(menu_action, &QAction::triggered, this, [this, selected_anime, type = action.type] {
                switch (type) {
                    case EpisodeActionType::IncreaseEpisode: {
                        emit requestIncreaseAnimeProgress(selected_anime);
                        break;
                    }
                    case EpisodeActionType::DecreaseEpisode: {
                        emit requestDecreaseAnimeProgress(selected_anime);
                        break;
                    }
                    default:
                        break;
                }
            });
        }
    }
}

void AnimeListView::addDateActions(QMenu &menu, const QList<AnilistAnime> &selected_anime) {
    QMenu *started_menu = menu.addMenu(
        QIcon(AppResources::Icons::Calendar),
        QStringLiteral("Set started date")
    );

    static const QList<DateMenuAction> date_started_actions{
        { QStringLiteral("Clear"),                 DateActionType::Clear,             AppResources::Icons::Calendar },
        { QStringLiteral("Set to started airing"), DateActionType::SetFromMediaStart, AppResources::Icons::Calendar }
    };

    for (const auto &action : date_started_actions) {
        QAction *menu_action = started_menu->addAction(
            QIcon(action.icon),
            action.name
        );
        connect(menu_action, &QAction::triggered, this, [this, selected_anime, type = action.type] {
            switch (type) {
                case DateActionType::Clear: {
                    emit requestClearAnimeStartedDate(selected_anime);
                    break;
                }
                case DateActionType::SetFromMediaStart: {
                    emit requestSetAnimeStartedDateFromMediaStartDate(selected_anime);
                    break;
                }
                default:
                    break;
            }
        });
    }


    QMenu *completed_menu = menu.addMenu(
        QIcon(AppResources::Icons::Calendar),
        QStringLiteral("Set completed date")
    );

    static const QList<DateMenuAction> date_completed_actions{
        { QStringLiteral("Clear"),                   DateActionType::Clear,              AppResources::Icons::Calendar },
        { QStringLiteral("Set to last updated"),     DateActionType::SetFromLastUpdated, AppResources::Icons::Calendar },
        { QStringLiteral("Set to completed airing"), DateActionType::SetFromMediaEnd,    AppResources::Icons::Calendar }
    };

    for (const auto &action : date_completed_actions) {
        QAction *menu_action = completed_menu->addAction(
            QIcon(action.icon),
            action.name
        );
        connect(menu_action, &QAction::triggered, this, [this, selected_anime, type = action.type] {
            switch (type) {
                case DateActionType::Clear: {
                    emit requestClearAnimeCompletedDate(selected_anime);
                    break;
                }
                case DateActionType::SetFromLastUpdated: {
                    emit requestSetAnimeCompletedDateFromLastUpdated(selected_anime);
                    break;
                }
                case DateActionType::SetFromMediaEnd: {
                    emit requestSetAnimeCompletedDateFromMediaEndDate(selected_anime);
                    break;
                }
                default:
                    break;
            }
        });
    }
}

void AnimeListView::addScoreActions(QMenu &menu, const QList<AnilistAnime> &selected_anime) {
    const auto *score = menu.addAction(
        QIcon(AppResources::Icons::Numbers),
        QStringLiteral("Set score")
    );
    connect(score, &QAction::triggered, this, [this, selected_anime] {
        auto *score_dialog = new SetScoreDialog(selected_anime, this->title_language_, this->score_format_, this);
        connect(score_dialog, &SetScoreDialog::scoreAccepted, this, [this, selected_anime] (int score) {
            emit requestSetAnimeScore(selected_anime, score);
        });
        score_dialog->open();
    });
}

void AnimeListView::addStatusActions(QMenu &menu, const QList<AnilistAnime> &selected_anime) {
    QMenu *status_menu = menu.addMenu(
        QIcon(AppResources::Icons::Edit),
        QStringLiteral("Set status")
    );

    static const QList<StatusMenuAction> status_actions{
        { QStringLiteral("Watching"),   AnilistEntry::Status::CURRENT,   AppResources::Icons::Edit },
        { QStringLiteral("Planning"),   AnilistEntry::Status::PLANNING,  AppResources::Icons::Edit },
        { QStringLiteral("Completed"),  AnilistEntry::Status::COMPLETED, AppResources::Icons::Edit },
        { QStringLiteral("Dropped"),    AnilistEntry::Status::DROPPED,   AppResources::Icons::Edit },
        { QStringLiteral("Paused"),     AnilistEntry::Status::PAUSED,    AppResources::Icons::Edit },
        { QStringLiteral("Rewatching"), AnilistEntry::Status::REPEATING, AppResources::Icons::Edit },
    };

    for (const auto &action : status_actions) {
        QAction *menu_action = status_menu->addAction(
            QIcon(action.icon),
            action.name
        );

        connect(menu_action, &QAction::triggered, this, [this, selected_anime, status = action.status] {
            emit requestSetAnimeStatus(selected_anime, status);
        });
    }
}

void AnimeListView::onCustomContextMenuRequested(const QPoint &pos) {
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

    QList<AnilistAnime> selected_anime;

    const QModelIndexList selected_rows = this->selectionModel()->selectedRows();
    for (const auto &row : selected_rows) {
        selected_anime.append(
            row.data(AnimeListModel::UserRoles::Anime).value<AnilistAnime>()
        );
    }

    QMenu menu(this);

    if (selected_anime.size() == 1) {
        this->addInfoEditActions(menu, selected_anime.constFirst());
        menu.addSeparator();
        this->addStateHistoryActions(menu, selected_anime.constFirst());
        menu.addSeparator();
    }

    this->addUndoRedoActions(menu, selected_anime);
    menu.addSeparator();
    this->addDeleteActions(menu, selected_anime);
    menu.addSeparator();
    this->addEpisodeActions(menu, selected_anime);
    menu.addSeparator();
    this->addDateActions(menu, selected_anime);
    menu.addSeparator();
    this->addScoreActions(menu, selected_anime);
    menu.addSeparator();
    this->addStatusActions(menu, selected_anime);

    menu.exec(this->viewport()->mapToGlobal(pos));
}

