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
    void exitTriggered();

private:
    QMenu *tray_menu_;
    QAction *toggle_visible_action_;
    QAction *exit_action_;

    void setupTrayMenu();

private slots:
    void onTrayActivated(QSystemTrayIcon::ActivationReason reason);
};

