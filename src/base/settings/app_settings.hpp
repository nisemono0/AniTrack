#pragma once

#include <type_traits>

#include <QObject>
#include <QVariant>
#include <QVariantMap>


class AppSettings final : public QObject {
    Q_OBJECT;
public:
    explicit AppSettings(QObject *parent = nullptr);
    ~AppSettings() = default;

    // Template that returns a type T setting or default_value if key does not exist
    // If the type T is an enum, it will static_cast the value to said enum type
    template<typename T>
    inline T get(const QString &key, const T &default_value) const {
        const QVariant value = this->settings_variantmap_.value(
            key,
            QVariant::fromValue(default_value)
        );

        if constexpr (std::is_enum_v<T>) {
            return static_cast<T>(value.toInt());
        }

        return value.value<T>();
    }

    // Template that sets key setting to T type value
    // Value is set using QVariant::fromValue()
    template<typename T>
    inline void set(const QString &key, const T &value) {
        this->settings_variantmap_[key] = QVariant::fromValue(value);
    }

    // Save stored settings to disk
    // Save enum types as their underlying int value
    void syncToDisk() const;

private:
    QVariantMap settings_variantmap_;

};

