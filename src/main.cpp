#include "app/app.hpp"
#include "app/command_line_parser.hpp"
#include "app/system_tray_icon.hpp"

#include "utils/dialog.hpp"
#include "utils/settings.hpp"

#include "gui/main_window.hpp"


int main (int argc, char *argv[]) {
    App app(argc, argv);

    if (app.isAlreadyRunning()) {
        return 0;
    }

    if (app.isInitFailed()) {
        DialogUtils::error(
            QStringLiteral("Startup error"),
            QStringLiteral("The application could not be initialized and will now close"),
            nullptr,
            true
        );
        return 1;
    }

    CommandLineParser cmd_parser(app);

    MainWindow *main_window = new MainWindow(
        app.controller()
    );
    SystemTrayIcon *system_tray = new SystemTrayIcon(&app);

    // Single instance
    QObject::connect(&app, &App::newInstanceStarted, main_window, &MainWindow::showAndFocus);
    // Tray hide/show
    QObject::connect(system_tray, &SystemTrayIcon::toggleVisibleTriggered, main_window, &MainWindow::toggleWindowVisibility);
    QObject::connect(main_window, &MainWindow::windowVisibilityChanged, system_tray, &SystemTrayIcon::onWindowVisibilityChanged);
    // Tray account
    QObject::connect(system_tray, &SystemTrayIcon::accountSetupTriggered, main_window, &MainWindow::trayAccountSetupTriggered);
    QObject::connect(system_tray, &SystemTrayIcon::userUpdateTriggered, main_window, &MainWindow::trayUserUpdateTriggered);
    // Tray sync
    QObject::connect(system_tray, &SystemTrayIcon::syncTriggered, main_window, &MainWindow::traySyncTriggered);
    QObject::connect(system_tray, &SystemTrayIcon::syncAnimeInfoTriggered, main_window, &MainWindow::traySyncAnimeInfoTriggered);
    // Tray settings
    QObject::connect(system_tray, &SystemTrayIcon::settingsTriggered, main_window, &MainWindow::traySettingsTriggered);
    // Tray view
    QObject::connect(system_tray, &SystemTrayIcon::logsTriggered, main_window, &MainWindow::trayLogsTriggered);
    QObject::connect(system_tray, &SystemTrayIcon::aboutTriggered, main_window, &MainWindow::trayAboutTriggered);
    // Tray exit
    QObject::connect(system_tray, &SystemTrayIcon::exitTriggered, &app, App::quit);
    QObject::connect(main_window, &MainWindow::exitTriggered, &app, App::quit);

    // Show the tray icon if system tray is available
    if (SystemTrayIcon::isSystemTrayAvailable()) {
        app.setQuitOnLastWindowClosed(false);
        system_tray->show();
    }

    // Run the window as dialog if -w flag is set
    // Hacky way for tiling window managers to start the window as floating
    if (cmd_parser.isWindowOptionSet()) {
        main_window->setWindowFlag(Qt::Dialog, true);
    }
    // Force set the selected style
    if (!cmd_parser.selectedStyle().isEmpty()) {
        app.setStyle(cmd_parser.selectedStyle());
    }

    app.controller()->init();
    if (!app.settings()->get(Settings::Ui::Window::StartMinimized, false)) {
        main_window->show();
    }

    const int app_return_code = app.exec();
    delete main_window;

    app.settings()->syncToDisk();

    return app_return_code;
}
