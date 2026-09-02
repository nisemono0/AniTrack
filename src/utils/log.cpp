#include "utils/log.hpp"

#include "app/app.hpp"


void Log::info(const QString &context, const QString &message) {
    App::instance()->logger()->log(context, message, Logger::Level::INFO);
}

void Log::warning(const QString &context, const QString &message) {
    App::instance()->logger()->log(context, message, Logger::Level::WARNING);
}

void Log::error(const QString &context, const QString &message) {
    App::instance()->logger()->log(context, message, Logger::Level::ERROR);
}

