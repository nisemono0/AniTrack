#include "app/app_controller.hpp"


AppController::AppController(
    Logger *logger,
    AccountManager *account_manager,
    DatabaseController *database_controller,
    SyncManager *sync_manager,
    SearchManager *search_manager,
    QObject *parent
) : QObject(parent),
    search_manager_(search_manager),
    sync_manager_(sync_manager),
    database_controller_(database_controller),
    account_manager_(account_manager),
    logger_(logger) {

    this->setupLoggerConnections();
    this->setupDatabaseConnections();
    this->setupAccountManagerConnections();
    this->setupSyncConnections();
    this->setupSearchConnections();
}

void AppController::init() {
    this->account_manager_->ensureLoggedIn();
    this->database_controller_->initialLoad();
}

void AppController::setupLoggerConnections() {
    connect(this->logger_, &Logger::messageLogged, this, &AppController::requestAppendLogMessage);
}

void AppController::setupDatabaseConnections() {
    connect(this, &AppController::requestIncreaseAnimeProgress, this->database_controller_, &DatabaseController::requestIncreaseAnimeProgress);
    connect(this, &AppController::requestDecreaseAnimeProgress, this->database_controller_, &DatabaseController::requestDecreaseAnimeProgress);
    connect(this, &AppController::requestSetAnimeProgress, this->database_controller_, &DatabaseController::requestSetAnimeProgress);

    connect(this, &AppController::requestAddMedia, this->database_controller_, &DatabaseController::requestAddMedia);

    connect(this->database_controller_, &DatabaseController::errorOccurred, this, &AppController::onErrorOccurred);
    connect(this->database_controller_, &DatabaseController::infoOccurred, this, &AppController::onInfoOccurred);

    connect(this->database_controller_, &DatabaseController::databasePathChanged, this, &AppController::databasePathChanged);

    connect(this->database_controller_, &DatabaseController::animeLoadFinished, this, &AppController::animeLoadFinished);

    connect(this->database_controller_, &DatabaseController::animeUpdateFinished, this, &AppController::animeUpdateFinished);
    connect(this->database_controller_, &DatabaseController::animeDeleteFinished, this, &AppController::animeDeleteFinished);

    connect(this->database_controller_, &DatabaseController::animeAddFinished, this, &AppController::animeAddFinished);
    connect(this->database_controller_, &DatabaseController::mediaAddFinished, this, &AppController::mediaAddFinished);

    connect(this, &AppController::requestUndoAnimeState, this->database_controller_, &DatabaseController::requestUndoAnimeState);
    connect(this, &AppController::requestRedoAnimeState, this->database_controller_, &DatabaseController::requestRedoAnimeState);

    connect(this, &AppController::requestMarkAnimeForRemoval, this->database_controller_, &DatabaseController::requestMarkAnimeForRemoval);

    connect(this, &AppController::requestClearAnimeStartedDate, this->database_controller_, &DatabaseController::requestClearAnimeStartedDate);
    connect(this, &AppController::requestSetAnimeStartedDateFromMediaStartDate, this->database_controller_, &DatabaseController::requestSetAnimeStartedDateFromMediaStartDate);
    connect(this, &AppController::requestSetAnimeStartedDate, this->database_controller_, &DatabaseController::requestSetAnimeStartedDate);

    connect(this, &AppController::requestClearAnimeCompletedDate, this->database_controller_, &DatabaseController::requestClearAnimeCompletedDate);
    connect(this, &AppController::requestSetAnimeCompletedDateFromLastUpdated, this->database_controller_, &DatabaseController::requestSetAnimeCompletedDateFromLastUpdated);
    connect(this, &AppController::requestSetAnimeCompletedDateFromMediaEndDate, this->database_controller_, &DatabaseController::requestSetAnimeCompletedDateFromMediaEndDate);
    connect(this, &AppController::requestSetAnimeCompletedDate, this->database_controller_, &DatabaseController::requestSetAnimeCompletedDate);

    connect(this, &AppController::requestSetAnimeStatus, this->database_controller_, &DatabaseController::requestSetAnimeStatus);

    connect(this, &AppController::requestSetAnimeScore, this->database_controller_, &DatabaseController::requestSetAnimeScore);

    connect(this, &AppController::requestRestoreAnimeState, this->database_controller_, &DatabaseController::requestRestoreAnimeState);

    connect(this, &AppController::requestUpdateAnime, this->database_controller_, &DatabaseController::requestUpdateAnime);

    connect(this->database_controller_, &DatabaseController::animeCountChanged, this, &AppController::animeCountChanged);

    connect(this, &AppController::requestCleanupUnusedMedia, this->database_controller_, &DatabaseController::requestCleanupUnusedMedia);
    connect(this, &AppController::requestVacuumDatabase, this->database_controller_, &DatabaseController::requestVacuumDatabase);
    connect(this, &AppController::requestRecreateDatabase, this->database_controller_, &DatabaseController::requestRecreateDatabase);
}

void AppController::setupAccountManagerConnections() {
    connect(this, &AppController::requestAuthUrl, this->account_manager_, &AccountManager::requestAuthUrl);
    connect(this, &AppController::setAuthToken, this->account_manager_, &AccountManager::setAuthToken);

    connect(this, &AppController::requestProfileUrl, this->account_manager_, &AccountManager::requestProfileUrl);
    connect(this->account_manager_, &AccountManager::profileUrlReady, this, &AppController::requestOpenUrl);

    connect(this, &AppController::requestListUrl, this->account_manager_, &AccountManager::requestListUrl);
    connect(this->account_manager_, &AccountManager::listUrlReady, this, &AppController::requestOpenUrl);

    connect(this, &AppController::requestStatsUrl, this->account_manager_, &AccountManager::requestStatsUrl);
    connect(this->account_manager_, &AccountManager::statsUrlReady, this, &AppController::requestOpenUrl);

    connect(this->account_manager_, &AccountManager::authUrlReady, this, [this] (const QUrl &auth_url) {
        emit requestOpenUrl(auth_url);
        emit requestShowLoginTokenDialog();
    });

    connect(this->account_manager_, &AccountManager::fetchStarted, this, &AppController::requestShowInfiniteProgressDialog);
    connect(this->account_manager_, &AccountManager::fetchFinished, this, &AppController::requestCloseInfiniteProgressDialog);

    connect(this, &AppController::requestUserUpdate, this->account_manager_, &AccountManager::requestUserUpdate);
    connect(this->account_manager_, &AccountManager::userUpdated, this, &AppController::userUpdated);

    connect(this->account_manager_, &AccountManager::requestRecreateDatabase, this->database_controller_, &DatabaseController::requestRecreateDatabase);

    connect(this->account_manager_, &AccountManager::warningOccurred, this, &AppController::onWarningOccurred);
    connect(this->account_manager_, &AccountManager::errorOccurred, this, &AppController::onErrorOccurred);
}

void AppController::setupSyncConnections() {
    connect(this, &AppController::requestSync, this->sync_manager_, &SyncManager::requestSync);

    connect(this->sync_manager_, &SyncManager::syncProgressStarted, this, &AppController::requestShowProgressDialog);
    connect(this->sync_manager_, &SyncManager::syncProgressUpdated, this, &AppController::requestUpdateProgressDialog);
    connect(this->sync_manager_, &SyncManager::syncProgressFinished, this, &AppController::requestCloseProgressDialog);

    connect(this->sync_manager_, &SyncManager::syncFinished, this->database_controller_, &DatabaseController::requestLoadAnime);

    connect(this->sync_manager_, &SyncManager::listFetchStarted, this, &AppController::requestShowInfiniteProgressDialog);
    connect(this->sync_manager_, &SyncManager::listFetchFinished, this, &AppController::requestCloseInfiniteProgressDialog);

    connect(this->sync_manager_, &SyncManager::syncFailed, this, &AppController::onErrorOccurred);
}

void AppController::setupSearchConnections() {
    connect(this, &AppController::requestAnimeSearch, this->search_manager_, &SearchManager::requestAnimeSearch);

    connect(this->search_manager_, &SearchManager::searchProgressStarted, this, &AppController::requestShowInfiniteProgressDialog);
    connect(this->search_manager_, &SearchManager::searchProgressFinished, this, &AppController::requestCloseInfiniteProgressDialog);

    connect(this->search_manager_, &SearchManager::searchFinished, this, &AppController::searchFinished);

    connect(this->search_manager_, &SearchManager::searchFailed, this, &AppController::onErrorOccurred);
}

void AppController::onInfoOccurred(const QString &context, const QString &message) {
    this->logger_->log(context, message, Logger::Level::INFO);
    emit requestShowInfoDialog(context, message);
}

void AppController::onWarningOccurred(const QString &context, const QString &message) {
    this->logger_->log(context, message, Logger::Level::WARNING);
    emit requestShowWarningDialog(context, message);
}

void AppController::onErrorOccurred(const QString &context, const QString &message) {
    this->logger_->log(context, message, Logger::Level::ERROR);
    emit requestShowErrorDialog(context, message);
}

