#pragma once

#include <QObject>
#include <QDBusServiceWatcher>
#include <QDBusConnection>


class MprisWatcher final : public QObject {
    Q_OBJECT;
public:
    explicit MprisWatcher(QObject *parent = nullptr);
    ~MprisWatcher() = default;

    // Register any whitelisted already running players
    void registerRunningPlayers();

signals:
    void mediaFileChanged(const QString &file_name);

private:
    QDBusServiceWatcher *dbus_watcher_;
    QDBusConnection session_bus_ = QDBusConnection::sessionBus();

    QString current_file_url_;
    QSet<QString> active_services_;

    void readRunningPlayersMetadata(const QString &service_name);
    void processMetadataAndNotify(const QVariantMap &metadata_vmap);

private slots:
    void onServiceRegistered(const QString &service_name);
    void onServiceUnregistered(const QString &service_name);

    void onPropertiesChanged(const QString &interface_name,
                             const QVariantMap &changed_properties,
                             const QStringList &invalidated_properties);
};

