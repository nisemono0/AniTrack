#pragma once

#include <QString>
#include <QMetaEnum>


namespace EnumsUtils {
// Template that returns an Enum type from QString
// The enum MUST be registered to the Qt meta object system (w/ Q_ENUM();)
// Returns the enum type for string or fallback if the string key is not defined
template<typename Enum>
inline Enum enumFromString(const QString &string, Enum fallback) {
    QMetaEnum meta_enum = QMetaEnum::fromType<Enum>();

    bool ok;
    int enum_value = meta_enum.keyToValue(
        string.toLatin1().constData(),
        &ok
    );

    return ok ? static_cast<Enum>(enum_value) : fallback;
}

// Template that returns a string from an Enum type
// The enum MUST be registered to the Qt meta object syste (w/ Q_ENUM();)
// Returns the string for the enum of fallback if enum is not defined
template<typename Enum>
inline QString stringFromEnum(Enum value, const QString &fallback) {
    QMetaEnum meta_enum = QMetaEnum::fromType<Enum>();

    const char *key = meta_enum.valueToKey(
        static_cast<int>(value)
    );

    return key ? QString::fromLatin1(key) : fallback;
}
} // namespace EnumsUtils

