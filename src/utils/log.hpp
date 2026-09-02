#pragma once

#include <QString>


// Wrappers around App::instance()->logger()
namespace Log {
// Helper function to log an info message
void info(const QString &context, const QString &message);

// Helper function to log a warning message
void warning(const QString &context, const QString &message);

// Helper function to log an error message
void error(const QString &context, const QString &message);

} // namespace Log


// Log context helpers
#define CONTEXT_CLASS QString::fromUtf8(this->metaObject()->className())
#define CONTEXT_FUNC QString::fromUtf8(Q_FUNC_INFO)
#define CONTEXT_FULL QStringLiteral("%1::%2").arg(LOG_CONTEXT_CLASS, LOG_CONTEXT_FUNC)

