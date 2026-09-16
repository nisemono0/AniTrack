#pragma once

#include <QSystemTrayIcon>
#include <QMenu>
#include <QAction>


class SystemTrayIcon final : public QSystemTrayIcon {
    Q_OBJECT;
public:
    explicit SystemTrayIcon(QObject *parent = nullptr);
    ~SystemTrayIcon();

public slots:
    void onWindowVisibilityChanged(bool visible);

signals:
    void toggleVisibleTriggered();

    void accountSetupTriggered();
    void userUpdateTriggered();

    void syncTriggered();
    void syncAnimeInfoTriggered();

    void settingsTriggered();

    void exitTriggered();

private:
    QMenu *tray_menu_;
    QAction *toggle_visible_action_;

    void setupTrayMenu();

    void addToggleVisibleAction();
    void addAccountActions();
    void addSyncActions();
    void addSettingsAction();
    void addExitAction();

private slots:
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
};

