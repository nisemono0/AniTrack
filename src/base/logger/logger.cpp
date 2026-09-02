#include "base/logger/logger.hpp"

namespace Format::Terminal {
namespace {
QString info(const QString &context, const QString &message) {
    return QStringLiteral("\033[38;5;245m[INFO]\033[0m [%1]: %2").arg(context, message);
}
QString warning(const QString &context, const QString &message) {
    return QStringLiteral("\033[38;5;172m[WARN]\033[0m [%1]: %2").arg(context, message);
}
QString error(const QString &context, const QString &message) {
    return QStringLiteral("\033[38;5;124m[CRIT]\033[0m [%1]: %2").arg(context, message);
}
} // namespace
} // namespace Format::Terminal

namespace Format::GUI {
namespace {
QString info(const QString &context, const QString &message) {
    return QStringLiteral("<font color=\"gray\">[INFO]</font> [%1]: %2").arg(context, message);
}
QString warning(const QString &context, const QString &message) {
    return QStringLiteral("<font color=\"orange\">[WARN]</font> [%1]: %2").arg(context, message);
}
QString error(const QString &context, const QString &message) {
    return QStringLiteral("<font color=\"red\">[CRIT]</font> [%1]: %2").arg(context, message);
}
} // namespace
} // namespace Format::GUI


Logger::Logger(QObject *parent) :
    QObject(parent),
    std_out_(stdout) {}

void Logger::log(const QString &context, const QString &message, Logger::Level level) {
    QString terminal_message;
    QString gui_message;

    switch (level) {
        case Level::INFO: {
            terminal_message = Format::Terminal::info(context, message);
            gui_message = Format::GUI::info(context, message);
            break;
        }
        case Level::WARNING: {
            terminal_message = Format::Terminal::warning(context, message);
            gui_message = Format::GUI::warning(context, message);
            break;
        }
        case Level::ERROR: {
            terminal_message = Format::Terminal::error(context, message);
            gui_message = Format::GUI::error(context, message);
            break;
        }
        default:
            break;
    }

    {
        QMutexLocker locker(&this->logger_mutex_);
        std_out_ << terminal_message << Qt::endl;
    }

    emit messageLogged(gui_message);
}

