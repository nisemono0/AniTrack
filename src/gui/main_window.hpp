#pragma once

#include "ui_main_window.h"

#include "app/app_controller.hpp"

#include "gui/dialogs/logger_dialog.hpp"
#include "gui/dialogs/login_token_dialog.hpp"
#include "gui/dialogs/about_dialog.hpp"

#include "gui/widgets/infinite_progress_dialog.hpp"
#include "gui/widgets/progress_dialog.hpp"

#include <QWidget>
#include <QMainWindow>
#include <QHideEvent>
#include <QShowEvent>
#include <QPointer>


namespace Ui {
class MainWindow;
};

class MainWindow final : public QMainWindow {
    Q_OBJECT;
public:
    explicit MainWindow(
        AppController *app_controller,
        QWidget *parent = nullptr
    );
    ~MainWindow();


public slots:
    // Show and focus mainwindow
    void showAndFocus();
    // Toggle window visibility
    void toggleWindowVisibility();


signals:
    void windowVisibilityChanged(bool visible);
    void exitTriggered();

protected:
    void hideEvent(QHideEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    // Ui
    Ui::MainWindow *ui_;
    // Controller
    AppController *app_controller_;
    // Dialogs
    LoggerDialog *logger_dialog_;
    LoginTokenDialog *login_token_dialog_;
    AboutDialog *about_dialog_;
    // Widgets
    QPointer<InfiniteProgressDialog> infinite_progress_dialog_;
    QPointer<ProgressDialog> progress_dialog_;

    // Setup
    void initUi();

    void setupFileMenu();
    void setupAnilistMenu();
    void setupToolsMenu();
    void setupViewMenu();

    void setupQuickActionsToolBar();
    void setupNavigationListWidget();
    void setupStatusBar();

    void setupAnimeListPage();
    void setupSearchPage();
    void setupStatisticsPage();

    void setupLoggerDialog();
    void setupLoginTokenDialog();

    void setupProgressDialogs();

    void setupMainWindow();

private slots:
    // Show/Close the infinite progress dialog
    void showInfiniteProgressDialog(const QString &title, const QString &message);
    void closeInfiniteProgressDialog();

    // Show/Update/Close the progress dialog
    void showProgressDialog(const QString &title, const QString &message, int minimum, int maximum);
    void updateProgressDialog(int progress);
    void closeProgressDialog();

    // Show info/warning/error dialogs
    void showInfo(const QString &context, const QString &message);
    void showWarning(const QString &context, const QString &message);
    void showError(const QString &context, const QString &message);

    // Open external url
    void openUrl(const QUrl &url);

    // Display the stacked page
    void showPage(ListWidgetNavigation::Page page);
};

