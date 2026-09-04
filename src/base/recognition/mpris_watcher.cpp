#include "base/recognition/mpris_watcher.hpp"
#include "base/recognition/mpris_config.hpp"
#include "base/recognition/mpris_keys.hpp"

#include "utils/log.hpp"
#include "utils/file.hpp"

#include <QDBusArgument>
#include <QDBusConnectionInterface>
#include <QDBusVariant>


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

void MprisWatcher::registerRunningPlayers() {
    auto *interface = this->session_bus_.interface();
    if (!interface) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Failed to get D-Bus interface")
        );
        return;
    }

    auto registered_services = interface->registeredServiceNames();

    if (!registered_services.isValid()) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Failed to get registered services: %1").arg(
                registered_services.error().message()
            )
        );
        return;
    }

    auto services = registered_services.value();
    for (const auto &service : std::as_const(services)) {
        if (MprisConfig::AllowedServices.contains(service)) {
            // Register the player for PropertiesChanged signal
            this->onServiceRegistered(service);
            // Get the running's player metadata since PropertiesChanged signal
            // might not be emitted for already running players on startup
            this->readRunningPlayersMetadata(service);
        }
    }
}

void MprisWatcher::processMetadataAndNotify(const QVariantMap &metadata_vmap) {
    if (metadata_vmap.isEmpty()) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Metadata is empty")
        );
        return;
    }

    if (!metadata_vmap.contains(MprisKeys::Url)) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Metadata does not contain: %1").arg(MprisKeys::Url)
        );
        return;
    }

    QString file_url = metadata_vmap.value(MprisKeys::Url).toString().trimmed();
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

void MprisWatcher::readRunningPlayersMetadata(const QString &service_name) {
    QDBusMessage metadata_method = QDBusMessage::createMethodCall(
        service_name,
        MprisConfig::ObjectPath,
        MprisConfig::PropertiesInterface,
        MprisConfig::PropertiesGet
    );
    metadata_method.setArguments({
        MprisConfig::PlayerInterface,
        MprisKeys::Metadata
    });

    auto reply = this->session_bus_.call(metadata_method);

    if (reply.type() == QDBusMessage::ErrorMessage) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Metadata method call failed for %1: %2").arg(service_name, reply.errorMessage())
        );
        return;
    }

    if (reply.arguments().isEmpty()) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("No metadata returned by: %1").arg(service_name)
        );
        return;
    }

    QVariant metadata_variant = reply.arguments().at(0).value<QDBusVariant>().variant();

    QVariantMap metadata;
    metadata_variant.value<QDBusArgument>() >> metadata;

    this->processMetadataAndNotify(metadata);
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

    if (this->active_services_.isEmpty()) {
        this->current_file_url_.clear();
    }
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

    this->processMetadataAndNotify(metadata);
}

