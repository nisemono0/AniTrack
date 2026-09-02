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

    connect(this->toggle_visible_action_, &QAction::triggered, this, &SystemTrayIcon::toggleVisibleTriggered);
    connect(this->exit_action_, &QAction::triggered, this, &SystemTrayIcon::exitTriggered);
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

    this->toggle_visible_action_ = new QAction(QStringLiteral("Hide"), this->tray_menu_);

    this->exit_action_ = new QAction(QStringLiteral("Exit"), this->tray_menu_);
    this->exit_action_->setIcon(QIcon(AppResources::Icons::X));

    this->tray_menu_->addAction(this->toggle_visible_action_);
    this->tray_menu_->addSeparator();
    this->tray_menu_->addAction(this->exit_action_);

    this->setContextMenu(this->tray_menu_);
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
