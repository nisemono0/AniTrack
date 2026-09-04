#include "base/recognition/mpris_watcher.hpp"
#include "base/recognition/mpris_config.hpp"
#include "base/recognition/mpris_keys.hpp"

#include "utils/log.hpp"
#include "utils/file.hpp"

#include <QDBusArgument>


MprisWatcher::MprisWatcher(QObject *parent) : QObject(parent) {
    this->dbus_watcher_ = new QDBusServiceWatcher(
        MprisConfig::Service,
        this->session_bus_,
        QDBusServiceWatcher::WatchForRegistration | QDBusServiceWatcher::WatchForUnregistration,
        this
    );

    connect(this->dbus_watcher_, &QDBusServiceWatcher::serviceRegistered, this, &MprisWatcher::onServiceRegistered);
    connect(this->dbus_watcher_, &QDBusServiceWatcher::serviceUnregistered, this, &MprisWatcher::onServiceUnregistered);
}

void MprisWatcher::onServiceRegistered(const QString &service_name) {
    // Only allow whitelisted mpris services
    if (!MprisConfig::AllowedServices.contains(service_name)) {
        return;
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("MPRIS service registered: %1").arg(service_name)
    );

    // Don't allow duplicate connections for the same service
    if (this->active_services_.contains(service_name)) {
        return;
    }

    bool connected = this->session_bus_.connect(
        service_name,
        MprisConfig::ObjectPath,
        MprisConfig::PropertiesInterface,
        MprisConfig::PropertiesChangedSignal,
        this,
        SLOT(onPropertiesChanged(QString,QVariantMap,QStringList))
    );

    if (!connected) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Failed to connect to: %1").arg(service_name)
        );
        return;
    }

    this->active_services_.insert(service_name);
}

void MprisWatcher::onServiceUnregistered(const QString &service_name) {
    // Only allow whitelisted mpris services
    if (!MprisConfig::AllowedServices.contains(service_name)) {
        return;
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("MPRIS service unregistered: %1").arg(service_name)
    );

    // Ignore services we're not connected to
    if (!this->active_services_.contains(service_name)) {
        return;
    }

    bool disconnected = this->session_bus_.disconnect(
        service_name,
        MprisConfig::ObjectPath,
        MprisConfig::PropertiesInterface,
        MprisConfig::PropertiesChangedSignal,
        this,
        SLOT(onPropertiesChanged(QString,QVariantMap,QStringList))
    );

    if (!disconnected) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Failed to disconnect: %1").arg(service_name)
        );
        return;
    }

    this->active_services_.remove(service_name);
}

void MprisWatcher::onPropertiesChanged(const QString &interface_name,
                                       const QVariantMap &changed_properties,
                                       const QStringList &invalidated_properties) {

    if (interface_name != MprisConfig::PlayerInterface) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Interface is not %1: %2").arg(MprisConfig::PlayerInterface, interface_name)
        );
        return;
    }

    if (!changed_properties.contains(MprisKeys::Metadata)) {
        QStringList keys = changed_properties.keys();
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("No Metadata in PropertiesChanged signal. Changed: %1").arg(
                keys.join(QChar::Space)
            )
        );
        return;
    }

    QVariantMap metadata;
    changed_properties.value(MprisKeys::Metadata).value<QDBusArgument>() >> metadata;

    if (metadata.isEmpty()) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Metadata is empty")
        );
        return;
    }

    if (!metadata.contains(MprisKeys::Url)) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Metadata does not contain: %1").arg(MprisKeys::Url)
        );
        return;
    }

    QString file_url = metadata.value(MprisKeys::Url).toString().trimmed();
    if (file_url == this->current_file_url_ || file_url.isEmpty()) {
        return;
    }

    auto file_name = FileUtils::fileNameFromUrl(file_url);
    if (!file_name) {
        Log::warning(CONTEXT_CLASS, file_name.error());
        return;
    }

    this->current_file_url_ = file_url;

    emit mediaFileChanged(file_name.value());
}

