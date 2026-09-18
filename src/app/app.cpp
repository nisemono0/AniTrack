#include "app/app.hpp"
#include "app/app_config.hpp"
#include "app/app_resources.hpp"

#include "utils/log.hpp"

#include <QLocalSocket>
#include <QIcon>


App::App(int &argc, char **argv) : QApplication(argc, argv) {
    this->logger_ = new Logger(this);

    this->setupSingleInstance();

    // Don't setup anything else if another instance is running
    if (this->already_running_) {
        return;
    }
    this->setupApp();

    this->app_settings_ = new AppSettings(this);

    this->network_ = new Network(this);

    this->image_cache_ = new ImageCache(
        this->network_,
        this
    );
    if (!this->image_cache_->init()) {
        this->init_failed_ = true;
        return;
    }

    this->database_ = new Database(this);
    if (!this->database_->init()) {
        this->init_failed_ = true;
        return;
    }

    this->database_controller_ = new DatabaseController(
        this->database_,
        this
    );

    this->anilist_account_ = new AnilistAccount(this);

    this->anilist_api_ = new AnilistApi(
        this->network_,
        this->anilist_account_,
        this
    );

    this->sync_manager_ = new SyncManager(
        this->anilist_api_,
        this->database_,
        this
    );

    this->search_manager_ = new SearchManager(
        this->anilist_api_,
        this
    );

    this->account_manager_ = new AccountManager(
        this->anilist_account_,
        this->anilist_api_,
        this
    );

    this->recognition_manager_ = new RecognitionManager(
        this->database_,
        this
    );

    this->app_controller_ = new AppController(
        this->logger_,
        this->account_manager_,
        this->database_controller_,
        this->sync_manager_,
        this->search_manager_,
        this->recognition_manager_,
        this
    );
}

App* App::instance() {
    return qobject_cast<App*>(QApplication::instance());
}

Logger* App::logger() const {
    return this->logger_;
}

AppSettings* App::settings() const {
    return this->app_settings_;
}

ImageCache* App::imageCache() const {
    return this->image_cache_;
}

AppController* App::controller() const {
    return this->app_controller_;
}

bool App::isAlreadyRunning() const {
    return this->already_running_;
}

bool App::isInitFailed() const {
    return this->init_failed_;
}

void App::setupApp() {
    this->setApplicationName(AppConfig::Name);
    this->setOrganizationName(AppConfig::OrgName);

    this->setApplicationVersion(ANITRACK_VERSION);

    this->setCursorFlashTime(AppConfig::CursorFlashTime);

    this->setWindowIcon(QIcon(AppResources::App::Icon));

    this->setStyle(AppConfig::Style);
    this->setPalette(this->darkPalette());
}

void App::setupSingleInstance() {
    QLocalSocket socket;
    socket.connectToServer(AppConfig::InstanceName);

    if (socket.waitForConnected(AppConfig::InstanceTimeout)) {
        this->already_running_ = true;
        return;
    }

    QLocalServer::removeServer(AppConfig::InstanceName);

    this->local_server_ = new QLocalServer(this);

    if (!this->local_server_->listen(AppConfig::InstanceName)) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Could not create single instance server: %1").arg(
                this->local_server_->errorString()
            )
        );
        return;
    }

    connect(this->local_server_, &QLocalServer::newConnection, this, &App::onNewInstanceConnection);
}

QPalette App::darkPalette() const {
    // Edited default fusion dark
    // https://github.com/qt/qtbase/blob/b2d28ec38cfd104dbd6db4364af65e18d98fa387/src/gui/kernel/qplatformtheme.cpp#L389
    const QColor window_text = QColor(240, 240, 240);
    const QColor background = QColor(35, 35, 35);
    const QColor light = background.lighter(150);
    const QColor mid = background.darker(130);
    const QColor mid_light = mid.lighter(110);
    const QColor base = background.darker(140);
    const QColor disabled_base(background);
    const QColor dark = background.darker(150);
    const QColor dark_disabled = QColor(209, 209, 209).darker(110);
    const QColor text = window_text;
    const QColor link = QColor(48, 140, 198);
    const QColor highlight = QColor(55, 65, 81);
    const QColor hightlighted_text = window_text;
    const QColor disabled_text = QColor(130, 130, 130);
    const QColor button = background;
    const QColor shadow = dark.darker(135);
    const QColor disabled_shadow = shadow.lighter(150);
    const QColor disabled_highlight(145, 145, 145);
    const QColor placeholder = QColor(240, 240, 240, 128);

    QPalette fusion_palette(window_text, background, light, dark, mid, text, base);
    fusion_palette.setBrush(QPalette::Midlight, mid_light);
    fusion_palette.setBrush(QPalette::Button, button);
    fusion_palette.setBrush(QPalette::Shadow, shadow);
    fusion_palette.setBrush(QPalette::HighlightedText, hightlighted_text);

    fusion_palette.setBrush(QPalette::Disabled, QPalette::Text, disabled_text);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::WindowText, disabled_text);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::ButtonText, disabled_text);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::Base, disabled_base);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::Dark, dark_disabled);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::Shadow, disabled_shadow);

    fusion_palette.setBrush(QPalette::Active, QPalette::Highlight, highlight);
    fusion_palette.setBrush(QPalette::Inactive, QPalette::Highlight, highlight);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::Highlight, disabled_highlight);

    fusion_palette.setBrush(QPalette::Active, QPalette::Accent, highlight);
    fusion_palette.setBrush(QPalette::Inactive, QPalette::Accent, highlight);
    fusion_palette.setBrush(QPalette::Disabled, QPalette::Accent, disabled_highlight);

    fusion_palette.setBrush(QPalette::PlaceholderText, placeholder);

    fusion_palette.setBrush(QPalette::Link, link);

    return fusion_palette;
}

void App::onNewInstanceConnection() {
    if (!this->local_server_->hasPendingConnections()) {
        return;
    }

    QLocalSocket *socket = this->local_server_->nextPendingConnection();
    if (!socket) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Could not read from single instance server (no pending connections): %1").arg(
                this->local_server_->errorString()
            )
        );
        return;
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Another instance detected: focusing currently running instance")
    );
    emit newInstanceStarted();

    socket->disconnectFromServer();
    socket->deleteLater();
}

