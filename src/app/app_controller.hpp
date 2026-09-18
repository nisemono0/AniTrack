#pragma once

#include "base/logger/logger.hpp"
#include "base/account/account_manager.hpp"
#include "base/database/database_controller.hpp"
#include "base/sync/sync_manager.hpp"
#include "base/search/search_manager.hpp"
#include "base/recognition/recognition_manager.hpp"

#include <QObject>


class AppController final : public QObject {
    Q_OBJECT;
public:
    explicit AppController(
        Logger *logger,
        AccountManager *account_manager,
        DatabaseController *database_controller,
        SyncManager *sync_manager,
        SearchManager *search_manager,
        RecognitionManager *recognition_manager,
        QObject *parent = nullptr
    );
    ~AppController() = default;

    // App init
    void init();

signals:
    void requestShowInfoDialog(const QString &context, const QString &message);
    void requestShowWarningDialog(const QString &context, const QString &message);
    void requestShowErrorDialog(const QString &context, const QString &message);

    void requestOpenUrl(const QUrl &url);

    // Logger
    void requestAppendLogMessage(const QString &message);

    // Account manager
    void requestAuthUrl();
    void setAuthToken(const QString &auth_token);

    void requestProfileUrl();
    void requestListUrl();
    void requestStatsUrl();

    void requestShowLoginTokenDialog();

    void requestUserUpdate();
    void userUpdated(const AnilistAccount::User &user);

    // Database controller
    void requestIncreaseAnimeProgress(const QList<AnilistAnime> &anime_list);
    void requestDecreaseAnimeProgress(const QList<AnilistAnime> &anime_list);
    void requestSetAnimeProgress(const QList<AnilistAnime> &anime_list, int progress);

    void requestAddMedia(const QList<AnilistMedia> &media_list, AnilistEntry::Status status);

    void databasePathChanged(const QString &database_path);

    void animeLoadFinished(const QList<AnilistAnime> &anime_list);

    void animeUpdateFinished(const QList<AnilistAnime> &anime_list);

    void animeAddFinished(const QList<AnilistAnime> &anime_list);

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

    void requestRestoreAnimeState(const AnilistAnime &anime, int index);

    void requestUpdateAnime(const AnilistAnime &anime);

    void animeCountChanged(int count);

    void requestCleanupUnusedMedia();
    void requestVacuumDatabase();
    void requestRecreateDatabase();

    // Sync manager
    void requestSync();
    void requestMediaSync();

    // Search manager
    void requestAnimeSearch(const QString &title);
    void requestQuietAnimeSearch(const QString &title);

    void searchFinished(const QList<AnilistMedia> &media_list);

    // Recognition manager
    void requestShowNoMatchPage(const QString &message);
    void requestShowIdlePage();
    void requestShowSelectAnimePage(const QList<RecognizedAnime> &recognized_anime, const QString &title, int episode, const QString &release_group);
    void requestShowNowPlayingPage(const RecognizedAnime &recognized_anime, const QString &title);

    void requestShowSearchPage(const QString &title, int episode);

    void recognizedAnimeSelected(const RecognizedAnime &recognized_anime);

    // Ui
    void requestShowInfiniteProgressDialog(const QString &title, const QString &message);
    void requestCloseInfiniteProgressDialog();

    void requestShowProgressDialog(const QString &title, const QString &message, int minimum, int maximum);
    void requestUpdateProgressDialog(int progress);
    void requestCloseProgressDialog();

private:
    // Controllers/Managers
    Logger *logger_;
    AccountManager *account_manager_;
    DatabaseController *database_controller_;
    SyncManager *sync_manager_;
    SearchManager *search_manager_;
    RecognitionManager *recognition_manager_;

    // Setup
    void setupLoggerConnections();
    void setupDatabaseConnections();
    void setupAccountManagerConnections();
    void setupSyncConnections();
    void setupSearchConnections();
    void setupRecognitionConnections();

private slots:
    void onInfoOccurred(const QString &context, const QString &message);
    void onWarningOccurred(const QString &context, const QString &message);
    void onErrorOccurred(const QString &context, const QString &message);
};

