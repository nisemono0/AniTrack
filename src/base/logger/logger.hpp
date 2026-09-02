#pragma once

#include <QObject>
#include <QMutex>
#include <QString>
#include <QTextStream>


class Logger final : public QObject {
    Q_OBJECT;
public:
    enum class Level { INFO, WARNING, ERROR };

    explicit Logger(QObject *parent = nullptr);
    ~Logger() = default;

    void log(const QString &context, const QString &message, Logger::Level level);

signals:
    void messageLogged(const QString &message);

private:
    QMutex logger_mutex_;
    QTextStream std_out_;
};

