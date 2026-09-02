#pragma once

#include "ui_statistics_widget.h"

#include "base/anilist/anilist_account.hpp"

#include <QWidget>
#include <QFileSystemWatcher>
#include <QFutureWatcher>
#include <QAtomicInt>


namespace Ui {
class StatisticsWidget;
}

class StatisticsPage final : public QWidget {
    Q_OBJECT;
public:
    explicit StatisticsPage(QWidget *parent = nullptr);
    ~StatisticsPage();

public slots:
    void onDatabasePathChanged(const QString &database_path);

    void onUserUpdated(const AnilistAccount::User &user);
    void onAnimeCountChanged(int count);

signals:
    void requestUserUpdate();

private:
    Ui::StatisticsWidget *ui_;

    QFileSystemWatcher *fs_watcher_;
    QFutureWatcher<qint64> *dirsize_watcher_;
    QAtomicInt dirsize_interrupt_;

    void updateCachePath();

    QString minutesToPrettyFormat(int minutes);
    QString sizeToPrettyFormat(qint64 size);

    qint64 getDirSize(const QString &dir_path);

private slots:
    void databaseFileChanged(const QString &database_path);
    void cacheDirChanged(const QString &cache_path);

    void onDirSizeWatcherFinished();
};

