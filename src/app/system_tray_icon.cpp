#include "app/system_tray_icon.hpp"

#include "app/app_config.hpp"
#include "app/app_resources.hpp"


SystemTrayIcon::SystemTrayIcon(QObject *parent) : QSystemTrayIcon(parent) {
    this->setIcon(
        QIcon(AppResources::Tray::Icon)
    );
    this->setToolTip(AppConfig::Name);

    this->setupTrayMenu();

    connect(this, &QSystemTrayIcon::activated, this, &SystemTrayIcon::onTrayActivated);

}

SystemTrayIcon::~SystemTrayIcon() {
    delete this->tray_menu_;
}

void SystemTrayIcon::onWindowVisibilityChanged(bool visible) {
    if (visible) {
        this->toggle_visible_action_->setText(QStringLiteral("Hide"));
        this->toggle_visible_action_->setIcon(QIcon(AppResources::Icons::Hide));
    } else {
        this->toggle_visible_action_->setText(QStringLiteral("Show"));
        this->toggle_visible_action_->setIcon(QIcon(AppResources::Icons::Show));
    }
}

void SystemTrayIcon::setupTrayMenu() {
    this->tray_menu_ = new QMenu(AppConfig::Name);

    this->addToggleVisibleAction();
    this->tray_menu_->addSeparator();
    this->addAccountActions();
    this->tray_menu_->addSeparator();
    this->addSyncActions();
    this->tray_menu_->addSeparator();
    this->addSettingsAction();
    this->tray_menu_->addSeparator();
    this->addViewActions();
    this->tray_menu_->addSeparator();
    this->addExitAction();

    this->setContextMenu(this->tray_menu_);
}

void SystemTrayIcon::addToggleVisibleAction() {
    this->toggle_visible_action_ = new QAction(
        QIcon(AppResources::Icons::Show),
        QStringLiteral("Show"),
        this->tray_menu_
    );
    this->tray_menu_->addAction(this->toggle_visible_action_);

    connect(this->toggle_visible_action_, &QAction::triggered, this, &SystemTrayIcon::toggleVisibleTriggered);
}

void SystemTrayIcon::addAccountActions() {
    auto *account_setup = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::Account),
        QStringLiteral("Account setup")
    );
    connect(account_setup, &QAction::triggered, this, &SystemTrayIcon::accountSetupTriggered);

    auto *user_update = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::AccountInfoUpdate),
        QStringLiteral("Update user info")
    );
    connect(user_update, &QAction::triggered, this, &SystemTrayIcon::userUpdateTriggered);
}

void SystemTrayIcon::addSyncActions() {
    auto *sync_action = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::Sync),
        QStringLiteral("Sync")
    );
    connect(sync_action, &QAction::triggered, this, &SystemTrayIcon::syncTriggered);

    auto *sync_anime_info_action = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::CloudDown),
        QStringLiteral("Sync anime info")
    );
    connect(sync_anime_info_action, &QAction::triggered, this, &SystemTrayIcon::syncAnimeInfoTriggered);
}

void SystemTrayIcon::addSettingsAction() {
    auto *settings_action = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::Settings),
        QStringLiteral("Settings")
    );
    connect(settings_action, &QAction::triggered, this, &SystemTrayIcon::settingsTriggered);
}

void SystemTrayIcon::addViewActions() {
    auto *logs_action = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::Logs),
        QStringLiteral("Logs")
    );
    connect(logs_action, &QAction::triggered, this, &SystemTrayIcon::logsTriggered);

    auto *about_action = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::Info),
        QStringLiteral("About")
    );
    connect(about_action, &QAction::triggered, this, &SystemTrayIcon::aboutTriggered);
}

void SystemTrayIcon::addExitAction() {
    auto *exit_action = this->tray_menu_->addAction(
        QIcon(AppResources::Icons::X),
        QStringLiteral("Exit")
    );
    connect(exit_action, &QAction::triggered, this, &SystemTrayIcon::exitTriggered);
}

void SystemTrayIcon::onTrayActivated(QSystemTrayIcon::ActivationReason reason) {
    switch (reason) {
        case QSystemTrayIcon::Unknown:
        case QSystemTrayIcon::Context:
        case QSystemTrayIcon::DoubleClick:
        case QSystemTrayIcon::MiddleClick:
            break;
        case QSystemTrayIcon::Trigger: {
            emit toggleVisibleTriggered();
            break;
        }
        default:
            break;
    }
}
