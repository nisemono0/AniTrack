#pragma once

#include "base/logger/logger.hpp"
#include "base/settings/app_settings.hpp"
#include "base/network/network.hpp"
#include "base/cache/image_cache.hpp"
#include "base/database/database.hpp"
#include "base/database/database_controller.hpp"
#include "base/sync/sync_manager.hpp"
#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_api.hpp"
#include "base/search/search_manager.hpp"
#include "base/account/account_manager.hpp"

#include "app/app_controller.hpp"

#include <QApplication>
#include <QLocalServer>


class App final : public QApplication {
    Q_OBJECT;
public:
    explicit App(int &argc, char **argv);
    ~App() = default;

    static App* instance();

    Logger* logger() const;
    AppSettings* settings() const;
    ImageCache* imageCache() const;
    AppController* controller() const;

    bool isAlreadyRunning() const;
    bool isInitFailed() const;

signals:
    void newInstanceStarted();

private:
    QLocalServer *local_server_ = nullptr;

    Logger *logger_ = nullptr;
    AppSettings *app_settings_ = nullptr;
    Network *network_ = nullptr;
    ImageCache *image_cache_ = nullptr;
    Database *database_ = nullptr;
    DatabaseController *database_controller_ = nullptr;
    AnilistAccount *anilist_account_ = nullptr;
    AnilistApi *anilist_api_ = nullptr;
    SyncManager *sync_manager_ = nullptr;
    SearchManager *search_manager_ = nullptr;
    AccountManager *account_manager_ = nullptr;
    AppController *app_controller_ = nullptr;

    bool already_running_ = false;
    bool init_failed_ = false;

    void setupApp();
    void setupSingleInstance();

private slots:
    void onNewInstanceConnection();

};

