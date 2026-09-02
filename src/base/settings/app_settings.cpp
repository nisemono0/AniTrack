#include "base/settings/app_settings.hpp"

#include "utils/log.hpp"

#include <QSettings>


AppSettings::AppSettings(QObject *parent) : QObject(parent) {
    QSettings::setDefaultFormat(QSettings::IniFormat);

    QSettings settings;
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Using settings: %1").arg(settings.fileName())
    );

    // Load all settings
    for (const auto &key : settings.allKeys()) {
        this->settings_variantmap_[key] = settings.value(key);
    }
}

void AppSettings::syncToDisk() const {
    QSettings settings;

    for (const auto &[key, value] : this->settings_variantmap_.asKeyValueRange()) {
        if (value.metaType().flags().testFlag(QMetaType::IsEnumeration)) {
            settings.setValue(
                key,
                value.toInt()
            );
        } else {
            settings.setValue(key, value);
        }
    }

    settings.sync();
}
