#include "gui/pages/statistics_page.hpp"

#include "utils/cache.hpp"

#include <QFileInfo>
#include <QDirIterator>
#include <QtConcurrentRun>


StatisticsPage::StatisticsPage(QWidget *parent) :
    QWidget(parent),
    ui_(new Ui::StatisticsWidget) {
    this->ui_->setupUi(this);

    this->fs_watcher_ = new QFileSystemWatcher(this);
    connect(this->fs_watcher_, &QFileSystemWatcher::fileChanged, this, &StatisticsPage::databaseFileChanged);
    connect(this->fs_watcher_, &QFileSystemWatcher::directoryChanged, this, &StatisticsPage::cacheDirChanged);

    this->dirsize_watcher_ = new QFutureWatcher<qint64>(this);
    this->dirsize_interrupt_.storeRelaxed(false);
    connect(this->dirsize_watcher_, &QFutureWatcher<qint64>::finished, this, &StatisticsPage::onDirSizeWatcherFinished);

    connect(this->ui_->pushButtonUpdateUser, &QPushButton::clicked, this, &StatisticsPage::requestUserUpdate);

    this->updateCachePath();
}

StatisticsPage::~StatisticsPage() {
    this->dirsize_interrupt_.storeRelaxed(true);

    if (this->dirsize_watcher_->isRunning()) {
        this->dirsize_watcher_->waitForFinished();
    }

    delete this->ui_;
}

void StatisticsPage::onDatabasePathChanged(const QString &database_path) {
    this->fs_watcher_->addPath(database_path);

    this->ui_->labelDatabaseLocation->setText(database_path);
    this->ui_->labelDatabaseSize->setText(
        this->sizeToPrettyFormat(QFileInfo(database_path).size())
    );
}


void StatisticsPage::onUserUpdated(const AnilistAccount::User &user) {
    this->ui_->labelUsername->setText(user.username);
    this->ui_->labelUserId->setText(QString::number(user.user_id));
    this->ui_->labelAnimeCount->setText(QString::number(user.anime_count));
    this->ui_->labelEpisodeCount->setText(QString::number(user.episodes_watched));
    this->ui_->labelTimeWatched->setText(
        this->minutesToPrettyFormat(user.minutes_watched)
    );
    this->ui_->labelMeanScore->setText(QString::number(user.mean_score));
    this->ui_->labelStandardDeviation->setText(QString::number(user.std_deviation));
}

void StatisticsPage::onAnimeCountChanged(int count) {
    this->ui_->labelDatabaseCount->setText(QString::number(count));
}

void StatisticsPage::updateCachePath() {
    this->fs_watcher_->addPath(Cache::cachePath());

    this->ui_->labelCacheLocation->setText(Cache::cachePath());
    this->ui_->labelCacheCount->setText(
        QString::number(Cache::cacheCount())
    );

    this->cacheDirChanged(Cache::cachePath());
}

QString StatisticsPage::minutesToPrettyFormat(int minutes) {
    int days = minutes / 1440; // 24h * 60min
    int hours = (minutes % 1440) / 60; // leftover min to hours
    int remainder_minutes = minutes % 60; // leftover min

    return QStringLiteral("%1 days %2 hours %3 minutes").arg(days).arg(hours).arg(remainder_minutes);
}

QString StatisticsPage::sizeToPrettyFormat(qint64 size) {
    QLocale locale;
    return locale.formattedDataSize(size, 1);
}

qint64 StatisticsPage::getDirSize(const QString &dir_path) {
    qint64 total_size = 0;

    QDirIterator dir_it(dir_path, QDir::Files | QDir::NoDotAndDotDot);
    while (dir_it.hasNext() && !this->dirsize_interrupt_.loadRelaxed()) {
        total_size += dir_it.nextFileInfo().size();
    }

    return total_size;
}

void StatisticsPage::databaseFileChanged(const QString &database_path) {
    this->ui_->labelDatabaseSize->setText(
        this->sizeToPrettyFormat(QFileInfo(database_path).size())
    );
}

void StatisticsPage::cacheDirChanged(const QString &cache_path) {
    this->ui_->labelCacheCount->setText(
        QString::number(Cache::cacheCount())
    );
    this->dirsize_watcher_->setFuture(
        QtConcurrent::run([this, cache_path] -> qint64 {
            return this->getDirSize(cache_path);
        })
    );
}

void StatisticsPage::onDirSizeWatcherFinished() {
    this->ui_->labelCacheSize->setText(
        this->sizeToPrettyFormat(this->dirsize_watcher_->result())
    );
}
