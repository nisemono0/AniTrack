#include "app/app.hpp"
#include "app/command_line_parser.hpp"
#include "app/system_tray_icon.hpp"

#include "utils/dialog.hpp"

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
    // Tray
    QObject::connect(system_tray, &SystemTrayIcon::toggleVisibleTriggered, main_window, &MainWindow::toggleWindowVisibility);
    QObject::connect(main_window, &MainWindow::windowVisibilityChanged, system_tray, &SystemTrayIcon::onWindowVisibilityChanged);
    // Exit
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
    main_window->show();

    int app_return_code = app.exec();
    delete main_window;

    app.settings()->syncToDisk();

    return app_return_code;
}
