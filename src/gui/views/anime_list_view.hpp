#pragma once

#include "base/anilist/anilist_account.hpp"

#include "gui/models/anime_list_model.hpp"
#include "gui/models/anime_list_proxy.hpp"

#include "gui/views/anime_list_header.hpp"

#include "gui/delegates/anime_list_progress_delegate.hpp"
#include "gui/delegates/anime_list_pending_delegate.hpp"
#include "gui/delegates/anime_list_adult_delegate.hpp"
#include "gui/delegates/anime_list_private_delegate.hpp"
#include "gui/delegates/anime_list_score_delegate.hpp"
#include "gui/delegates/anime_list_status_delegate.hpp"
#include "gui/delegates/anime_list_started_date_delegate.hpp"
#include "gui/delegates/anime_list_completed_date_delegate.hpp"

#include "gui/dialogs/anime_info_edit_dialog.hpp"

#include <QTreeView>
#include <QWidget>


class AnimeListView final : public QTreeView {
    Q_OBJECT;
public:
    explicit AnimeListView(QWidget *parent = nullptr);
    ~AnimeListView() = default;

    void updateUserPreferences(const AnilistAccount::User &user);

    void setAnime(const QList<AnilistAnime> &anime_list);
    void updateAnime(const QList<AnilistAnime> &anime_list);
    void deleteAnime(const QList<int> &local_ids);
    void addAnime(const QList<AnilistAnime> &anime_list);

    void setFilterTab(AnimeListTabBar::Tab tab);
    void setFilterText(const QString &text);

signals:
    void requestIncreaseAnimeProgress(const QList<AnilistAnime> &anime_list);
    void requestDecreaseAnimeProgress(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeProgress(const QList<AnilistAnime> &anime_list, int progress);

    void requestUndoAnimeState(const QList<AnilistAnime> &anime_list);
    void requestRedoAnimeState(const QList<AnilistAnime> &anime_list);

    void requestMarkAnimeForRemoval(const QList<AnilistAnime> &anime_list);

    void requestClearAnimeStartedDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeStartedDateFromMediaStartDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeStartedDate(const QList<AnilistAnime> &anime_list, const QDate &date);

    void requestClearAnimeCompletedDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeCompletedDateFromLastUpdated(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeCompletedDateFromMediaEndDate(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeCompletedDate(const QList<AnilistAnime> &anime_list, const QDate &date);

    void requestSetAnimeStatus(const QList<AnilistAnime> &anime_list, AnilistEntry::Status status);

    void requestSetAnimeScore(const QList<AnilistAnime> &anime_list, int score);

    void requestShowAnimeInfoEditDialog(const AnilistAnime &anime, AnimeInfoEditDialog::Page page);

    void requestRestoreAnimeState(const AnilistAnime &anime, int index);

protected:
    void wheelEvent(QWheelEvent *event) override;

private:
    AnilistAccount::ScoreFormat score_format_;
    AnilistAccount::TitleLanguage title_language_;

    AnimeListModel *anime_list_model_;
    AnimeListProxy *anime_list_proxy_;
    AnimeListHeader *anime_list_header_;

    AnimeListPendingDelegate *pending_delegate_;
    AnimeListProgressDelegate *progress_delegate_;
    AnimeListAdultDelegate *adult_delegate_;
    AnimeListPrivateDelegate *private_delegate_;
    AnimeListScoreDelegate *score_delegate_;
    AnimeListStatusDelegate *status_delegate_;
    AnimeListStartedDateDelegate *started_delegate_;
    AnimeListCompletedDateDelegate *completed_delegate_;

    void initView();

    void setupModel();
    void setupDelegates();
    void setupHeader();

    void addInfoEditActions(QMenu &menu, const AnilistAnime &selected_anime);
    void addStateHistoryActions(QMenu &menu, const AnilistAnime &selected_anime);
    void addUndoRedoActions(QMenu &menu, const QList<AnilistAnime> &selected_anime);
    void addDeleteActions(QMenu &menu, const QList<AnilistAnime> &selected_anime);
    void addEpisodeActions(QMenu &menu, const QList<AnilistAnime> &selected_anime);
    void addDateActions(QMenu &menu, const QList<AnilistAnime> &selected_anime);
    void addScoreActions(QMenu &menu, const QList<AnilistAnime> &selected_anime);
    void addStatusActions(QMenu &menu, const QList<AnilistAnime> &selected_anime);

private slots:
    void onCustomContextMenuRequested(const QPoint &pos);

};

