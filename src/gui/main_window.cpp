#include "gui/main_window.hpp"

#include "utils/dialog.hpp"
#include "utils/links.hpp"
#include "utils/web.hpp"
#include "utils/settings.hpp"

#include <QShortcut>


MainWindow::MainWindow(
    AppController *app_controller,
    QWidget *parent
) : QMainWindow(parent),
    ui_ (new Ui::MainWindow),
    app_controller_(app_controller) {

    this->initUi();

    this->applySettings();

    this->setupFileMenu();
    this->setupAnilistMenu();
    this->setupToolsMenu();
    this->setupViewMenu();

    this->setupQuickActionsToolBar();
    this->setupNavigationListWidget();
    this->setupStatusBar();

    this->setupPlayingPage();
    this->setupAnimeListPage();
    this->setupSearchPage();
    this->setupStatisticsPage();

    this->setupLoggerDialog();
    this->setupLoginTokenDialog();
    this->setupAnimeInfoEditDialog();

    this->setupProgressDialogs();

    this->setupMainWindow();
    this->setupTray();

    connect(App::instance()->settings(), &AppSettings::settingsChanged, this, &MainWindow::applySettings);
}

MainWindow::~MainWindow() {
    if (this->save_load_window_state_) {
        Settings::set(
            Settings::Ui::Window::State,
            this->saveGeometry()
        );
    }
    delete this->ui_;
}

void MainWindow::showAndFocus() {
    this->show();
    this->raise();
    this->activateWindow();
}

void MainWindow::toggleWindowVisibility() {
    if (this->isVisible()) {
        this->hide();
    } else {
        this->showAndFocus();
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    const QString text = event->text();

    if (event->modifiers() == Qt::NoModifier &&
        !text.isEmpty() &&
        text.constData()->isPrint()) {

        this->ui_->toolBarQuickActions->focusSearchInput();
        this->ui_->toolBarQuickActions->insertSearchText(text);

        event->accept();
        return;
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::hideEvent(QHideEvent *event) {
    emit windowVisibilityChanged(false);
    QMainWindow::hideEvent(event);
}

void MainWindow::showEvent(QShowEvent *event) {
    emit windowVisibilityChanged(true);
    QMainWindow::showEvent(event);
}

void MainWindow::initUi() {
    this->ui_->setupUi(this);

    this->logger_dialog_ = new LoggerDialog(this);
    this->login_token_dialog_ = new LoginTokenDialog(this);
    this->about_dialog_ = new AboutDialog(this);
    this->anime_info_edit_dialog_ = new AnimeInfoEditDialog(this);
    this->settings_dialog_ = new SettingsDialog(this);
}

void MainWindow::setupFileMenu() {
    connect(this->ui_->actionHide, &QAction::triggered, this, &MainWindow::toggleWindowVisibility);
    connect(this->ui_->actionExit, &QAction::triggered, this, &MainWindow::exitTriggered);
}

void MainWindow::setupAnilistMenu() {
    connect(this->ui_->actionSync, &QAction::triggered, this->app_controller_, &AppController::requestSync);
    connect(this->ui_->actionSyncAnimeInfo, &QAction::triggered, this->app_controller_, &AppController::requestMediaSync);

    connect(this->ui_->actionGotoProfile, &QAction::triggered, this->app_controller_, &AppController::requestProfileUrl);
    connect(this->ui_->actionGotoList, &QAction::triggered, this->app_controller_, &AppController::requestListUrl);
    connect(this->ui_->actionGotoStats, &QAction::triggered, this->app_controller_, &AppController::requestStatsUrl);

    connect(this->app_controller_, &AppController::requestOpenUrl, this, &MainWindow::openUrl);

    connect(this->ui_->actionAccountSetup, &QAction::triggered, this->app_controller_, &AppController::requestAuthUrl);
    connect(this->ui_->actionUpdateUserInfo, &QAction::triggered, this->app_controller_, &AppController::requestUserUpdate);
}

void MainWindow::setupToolsMenu() {
    connect(this->ui_->actionGotoAnilist, &QAction::triggered, this, [this] {
        this->openUrl(LinksUtils::ExternalLinks::AniList);
    });
    connect(this->ui_->actionGotoAnichart, &QAction::triggered, this, [this] {
        this->openUrl(LinksUtils::ExternalLinks::AniChart);
    });
    connect(this->ui_->actionGotoLivechart, &QAction::triggered, this, [this] {
        this->openUrl(LinksUtils::ExternalLinks::LiveChart);
    });

    connect(this->ui_->actionCleanupDatabase, &QAction::triggered, this->app_controller_, &AppController::requestCleanupUnusedMedia);
    connect(this->ui_->actionVacuumDatabase, &QAction::triggered, this->app_controller_, &AppController::requestVacuumDatabase);

    connect(this->ui_->actionRecreateDatabase, &QAction::triggered, this->app_controller_, &AppController::requestRecreateDatabase);

    connect(this->ui_->actionSettings, &QAction::triggered, this->settings_dialog_, &SettingsDialog::showOrFocus);
}

void MainWindow::setupViewMenu() {
    connect(this->ui_->actionLogs, &QAction::triggered, this->logger_dialog_, &LoggerDialog::showOrFocus);
    connect(this->ui_->actionAbout, &QAction::triggered, this->about_dialog_, &AboutDialog::showOrFocus);
}

void MainWindow::setupQuickActionsToolBar() {
    this->ui_->toolBarQuickActions->setupToolBar();

    connect(this->ui_->toolBarQuickActions, &ToolBarQuickActions::filterTextChanged, this->ui_->pageAnimeList, &AnimeListPage::onFilterTextChanged);
    connect(this->ui_->toolBarQuickActions, &ToolBarQuickActions::searchRequested, this->app_controller_, &AppController::requestAnimeSearch);

    auto *focus_search_shortcut = new QShortcut(this);
    focus_search_shortcut->setContext(Qt::WindowShortcut);
    focus_search_shortcut->setKeys({
        QKeySequence(Qt::Key_Slash),
        QKeySequence(Qt::CTRL | Qt::Key_F)
    });

    connect(focus_search_shortcut, &QShortcut::activated, this, [this] {
        if (!this->ui_->toolBarQuickActions->hasSearchFocus()) {
            this->ui_->toolBarQuickActions->focusSearchInput();
            this->ui_->toolBarQuickActions->selectSearchText();
        }
    });
}

void MainWindow::setupNavigationListWidget() {
    connect(this->ui_->listWidgetNavigation, &ListWidgetNavigation::pageChanged, this, &MainWindow::showPage);

    // Set the default startup page to the animelist page
    this->ui_->listWidgetNavigation->setCurrentPage(ListWidgetNavigation::Page::AnimeList);
}

void MainWindow::setupStatusBar() {
    connect(this->app_controller_, &AppController::userUpdated, this->ui_->statusBar, &StatusBar::onUserChanged);
}

void MainWindow::setupPlayingPage() {
    connect(this->app_controller_, &AppController::userUpdated, this->ui_->pagePlaying, &AnimePlayingPage::onUserUpdated);
    connect(this->app_controller_, &AppController::animeUpdateFinished, this->ui_->pagePlaying, &AnimePlayingPage::handleAnimeAddUpdateFinished);
    connect(this->app_controller_, &AppController::animeAddFinished, this->ui_->pagePlaying, &AnimePlayingPage::handleAnimeAddUpdateFinished);

    connect(this->app_controller_, &AppController::requestShowNoMatchPage, this->ui_->pagePlaying, &AnimePlayingPage::showNoMatchPage);
    connect(this->app_controller_, &AppController::requestShowIdlePage, this->ui_->pagePlaying, &AnimePlayingPage::showIdlePage);
    connect(
        this->app_controller_, &AppController::requestShowSelectAnimePage,
        this->ui_->pagePlaying, [this] (const QList<RecognizedAnime> &recognized_anime, const QString &title, int episode) {
            this->ui_->listWidgetNavigation->setCurrentPage(ListWidgetNavigation::Page::Playing);
            this->ui_->pagePlaying->showSelectAnimePage(recognized_anime, title, episode);
        }
    );
    connect(
        this->app_controller_, &AppController::requestShowNowPlayingPage,
        this->ui_->pagePlaying, [this] (const RecognizedAnime &recognized_anime, const QString &title) {
            this->ui_->listWidgetNavigation->setCurrentPage(ListWidgetNavigation::Page::Playing);
            this->ui_->pagePlaying->showNowPlayingPage(recognized_anime, title);
        }
    );
    connect(
        this->app_controller_, &AppController::requestShowSearchPage,
        this->ui_->pagePlaying, [this] (const QString &title, int episode) {
            this->ui_->listWidgetNavigation->setCurrentPage(ListWidgetNavigation::Page::Playing);
            this->ui_->pagePlaying->showSearchPage(title, episode);
        }
    );
    connect(this->ui_->pagePlaying, &AnimePlayingPage::requestQuietAnimeSearch, this->app_controller_, &AppController::requestQuietAnimeSearch);

    connect(this->ui_->pagePlaying, &AnimePlayingPage::requestShowAnimeInfoEditDialog, this->anime_info_edit_dialog_, &AnimeInfoEditDialog::showOrFocusInfoEdit);
    connect(this->ui_->pagePlaying, &AnimePlayingPage::requestAddMedia, this->app_controller_, &AppController::requestAddMedia);
    connect(this->ui_->pagePlaying, &AnimePlayingPage::requestSetAnimeProgress, this->app_controller_, &AppController::requestSetAnimeProgress);

    connect(this->ui_->pagePlaying, &AnimePlayingPage::recognizedAnimeSelected, this->app_controller_, &AppController::recognizedAnimeSelected);
}

void MainWindow::setupAnimeListPage() {
    connect(this->app_controller_, &AppController::userUpdated, this->ui_->pageAnimeList, &AnimeListPage::onUserUpdated);
    connect(this->app_controller_, &AppController::animeLoadFinished, this->ui_->pageAnimeList, &AnimeListPage::onAnimeLoadFinished);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestIncreaseAnimeProgress, this->app_controller_, &AppController::requestIncreaseAnimeProgress);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestDecreaseAnimeProgress, this->app_controller_, &AppController::requestDecreaseAnimeProgress);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeProgress, this->app_controller_, &AppController::requestSetAnimeProgress);

    connect(this->app_controller_, &AppController::animeUpdateFinished, this->ui_->pageAnimeList, &AnimeListPage::onAnimeUpdateFinished);
    connect(this->app_controller_, &AppController::animeAddFinished, this->ui_->pageAnimeList, &AnimeListPage::onAnimeAddFinished);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestUndoAnimeState, this->app_controller_, &AppController::requestUndoAnimeState);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestRedoAnimeState, this->app_controller_, &AppController::requestRedoAnimeState);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestClearAnimeStartedDate, this->app_controller_, &AppController::requestClearAnimeStartedDate);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeStartedDateFromMediaStartDate, this->app_controller_, &AppController::requestSetAnimeStartedDateFromMediaStartDate);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeStartedDate, this->app_controller_, &AppController::requestSetAnimeStartedDate);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestClearAnimeCompletedDate, this->app_controller_, &AppController::requestClearAnimeCompletedDate);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeCompletedDateFromLastUpdated, this->app_controller_, &AppController::requestSetAnimeCompletedDateFromLastUpdated);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeCompletedDateFromMediaEndDate, this->app_controller_, &AppController::requestSetAnimeCompletedDateFromMediaEndDate);
    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeCompletedDate, this->app_controller_, &AppController::requestSetAnimeCompletedDate);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeStatus, this->app_controller_, &AppController::requestSetAnimeStatus);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeEpisode, this->app_controller_, &AppController::requestSetAnimeProgress);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestSetAnimeScore, this->app_controller_, &AppController::requestSetAnimeScore);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestMarkAnimeForRemoval, this->app_controller_, &AppController::requestMarkAnimeForRemoval);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestRestoreAnimeState, this->app_controller_, &AppController::requestRestoreAnimeState);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestUpdateAnime, this->app_controller_, &AppController::requestUpdateAnime);

    connect(this->ui_->pageAnimeList, &AnimeListPage::requestShowAnimeInfoEditDialog, this->anime_info_edit_dialog_, &AnimeInfoEditDialog::showOrFocusInfoEdit);

    auto *next_tab_shortcut = new QShortcut(
        Qt::Key_Tab,
        this
    );
    next_tab_shortcut->setContext(Qt::WindowShortcut);
    connect(next_tab_shortcut, &QShortcut::activated, this, [this] {
        if (this->ui_->pageAnimeList->isVisible()) {
            this->ui_->pageAnimeList->selectNextTab();
        }
    });

    auto *prev_tab_shortcut = new QShortcut(
        Qt::Key_Backtab,
        this
    );
    prev_tab_shortcut->setContext(Qt::WindowShortcut);
    connect(prev_tab_shortcut, &QShortcut::activated, this, [this] {
        if (this->ui_->pageAnimeList->isVisible()) {
            this->ui_->pageAnimeList->selectPreviousTab();
        }
    });
}

void MainWindow::setupSearchPage() {
    connect(this->app_controller_, &AppController::searchFinished, this, [this] {
        this->ui_->listWidgetNavigation->setCurrentPage(ListWidgetNavigation::Page::Search);
    });

    connect(this->ui_->pageSearch, &AnimeSearchPage::requestAddMedia, this->app_controller_, &AppController::requestAddMedia);

    connect(this->app_controller_, &AppController::searchFinished, this->ui_->pageSearch, &AnimeSearchPage::onSearchFinished);

    connect(this->ui_->toolBarQuickActions, &ToolBarQuickActions::filterTextChanged, this->ui_->pageSearch, &AnimeSearchPage::onFilterTextChanged);

    connect(this->app_controller_, &AppController::userUpdated, this->ui_->pageSearch, &AnimeSearchPage::onUserUpdated);
    connect(this->app_controller_, &AppController::animeLoadFinished, this->ui_->pageSearch, &AnimeSearchPage::onAnimeLoadFinished);
    connect(this->app_controller_, &AppController::animeAddFinished, this->ui_->pageSearch, &AnimeSearchPage::onAnimeAddFinished);

    connect(this->ui_->pageSearch, &AnimeSearchPage::requestShowAnimeInfoEditDialog, this->anime_info_edit_dialog_, &AnimeInfoEditDialog::showOrFocusAdd);
}

void MainWindow::setupStatisticsPage() {
    connect(this->app_controller_, &AppController::userUpdated, this->ui_->pageStatistics, &StatisticsPage::onUserUpdated);
    connect(this->app_controller_, &AppController::databasePathChanged, this->ui_->pageStatistics, &StatisticsPage::onDatabasePathChanged);
    connect(this->app_controller_, &AppController::animeCountChanged, this->ui_->pageStatistics, &StatisticsPage::onAnimeCountChanged);

    connect(this->ui_->pageStatistics, &StatisticsPage::requestUserUpdate, this->app_controller_, &AppController::requestUserUpdate);
}

void MainWindow::setupLoggerDialog() {
    connect(this->app_controller_, &AppController::requestAppendLogMessage, this->logger_dialog_, &LoggerDialog::appendLogMessage);
}

void MainWindow::setupLoginTokenDialog() {
    connect(this->app_controller_, &AppController::requestShowLoginTokenDialog, this->login_token_dialog_, &LoginTokenDialog::openOrFocus);
    connect(this->login_token_dialog_, &LoginTokenDialog::authTokenAccepted, this->app_controller_, &AppController::setAuthToken);
}

void MainWindow::setupAnimeInfoEditDialog() {
    connect(this->app_controller_, &AppController::userUpdated, this->anime_info_edit_dialog_, &AnimeInfoEditDialog::onUserUpdated);
    connect(this->app_controller_, &AppController::animeUpdateFinished, this->anime_info_edit_dialog_, &AnimeInfoEditDialog::handleAnimeAddUpdateFinished);
    connect(this->app_controller_, &AppController::animeAddFinished, this->anime_info_edit_dialog_, &AnimeInfoEditDialog::handleAnimeAddUpdateFinished);

    connect(this->anime_info_edit_dialog_, &AnimeInfoEditDialog::requestUpdateAnime, this->app_controller_, &AppController::requestUpdateAnime);
    connect(this->anime_info_edit_dialog_, &AnimeInfoEditDialog::requestAddMedia, this->app_controller_, &AppController::requestAddMedia);
}

void MainWindow::setupProgressDialogs() {
    connect(this->app_controller_, &AppController::requestShowInfiniteProgressDialog, this, &MainWindow::showInfiniteProgressDialog);
    connect(this->app_controller_, &AppController::requestCloseInfiniteProgressDialog, this, &MainWindow::closeInfiniteProgressDialog);

    connect(this->app_controller_, &AppController::requestShowProgressDialog, this, &MainWindow::showProgressDialog);
    connect(this->app_controller_, &AppController::requestUpdateProgressDialog, this, &MainWindow::updateProgressDialog);
    connect(this->app_controller_, &AppController::requestCloseProgressDialog, this, &MainWindow::closeProgressDialog);
}

void MainWindow::setupMainWindow() {
    connect(this->app_controller_, &AppController::requestShowInfoDialog, this, &MainWindow::showInfo);
    connect(this->app_controller_, &AppController::requestShowWarningDialog, this, &MainWindow::showWarning);
    connect(this->app_controller_, &AppController::requestShowErrorDialog, this, &MainWindow::showError);

    if (this->save_load_window_state_) {
        this->restoreGeometry(
            Settings::get(Settings::Ui::Window::State, QByteArray())
        );
    }
}

void MainWindow::setupTray() {
    connect(this, &MainWindow::trayAccountSetupTriggered, this->ui_->actionAccountSetup, &QAction::trigger);
    connect(this, &MainWindow::trayUserUpdateTriggered, this->ui_->actionUpdateUserInfo, &QAction::trigger);

    connect(this, &MainWindow::traySyncTriggered, this->ui_->actionSync, &QAction::trigger);
    connect(this, &MainWindow::traySyncAnimeInfoTriggered, this->ui_->actionSyncAnimeInfo, &QAction::trigger);

    connect(this, &MainWindow::traySettingsTriggered, this->ui_->actionSettings, &QAction::trigger);
}

void MainWindow::showInfiniteProgressDialog(const QString &title, const QString &message) {
    if (this->infinite_progress_dialog_) {
        this->infinite_progress_dialog_->raise();
        this->infinite_progress_dialog_->activateWindow();
    } else {
        this->infinite_progress_dialog_ = new InfiniteProgressDialog(title, message, this);
        this->infinite_progress_dialog_->open();
    }
}

void MainWindow::closeInfiniteProgressDialog() {
    if (this->infinite_progress_dialog_) {
        this->infinite_progress_dialog_->close();
    }
}

void MainWindow::showProgressDialog(const QString &title, const QString &message, int minimum, int maximum) {
    if (this->progress_dialog_) {
        this->progress_dialog_->raise();
        this->progress_dialog_->activateWindow();
    } else {
        this->progress_dialog_ = new ProgressDialog(title, message, minimum, maximum, this);
        this->progress_dialog_->open();
    }
}

void MainWindow::updateProgressDialog(int progress) {
    if (this->progress_dialog_) {
        this->progress_dialog_->setValue(progress);
    }
}

void MainWindow::closeProgressDialog() {
    if (this->progress_dialog_) {
        this->progress_dialog_->close();
    }
}

void MainWindow::showInfo(const QString &context, const QString &message) {
    DialogUtils::information(context, message, this);
}

void MainWindow::showWarning(const QString &context, const QString &message) {
    DialogUtils::warning(context, message, this);
}

void MainWindow::showError(const QString &context, const QString &message) {
    DialogUtils::error(context, message, this);
}

void MainWindow::openUrl(const QUrl &url) {
    if (!WebUtils::openUrl(url)) {
        this->showWarning(
            QStringLiteral("Open url"),
            QStringLiteral("Failed to open: %1").arg(url.toString())
        );
    }
}

void MainWindow::showPage(ListWidgetNavigation::Page page) {
    switch (page) {
        case ListWidgetNavigation::Page::Playing: {
            this->ui_->stackedWidgetMainWindow->setCurrentWidget(
                this->ui_->pagePlaying
            );
            break;
        }
        case ListWidgetNavigation::Page::AnimeList: {
            this->ui_->stackedWidgetMainWindow->setCurrentWidget(
                this->ui_->pageAnimeList
            );
            break;
        }
        case ListWidgetNavigation::Page::Statistics: {
            this->ui_->stackedWidgetMainWindow->setCurrentWidget(
                this->ui_->pageStatistics
            );
            break;
        }
        case ListWidgetNavigation::Page::Search: {
            this->ui_->stackedWidgetMainWindow->setCurrentWidget(
                this->ui_->pageSearch
            );
            break;
        }
        default:
            break;
    }
}

void MainWindow::applySettings() {
    this->save_load_window_state_ = Settings::get(Settings::Ui::Window::SaveLoadWindowState, false);
}

