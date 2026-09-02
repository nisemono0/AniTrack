#include "base/account/account_manager.hpp"


AccountManager::AccountManager(
    AnilistAccount *anilist_account,
    AnilistApi *anilist_api,
    QObject *parent
) : QObject(parent),
    anilist_api_(anilist_api),
    anilist_account_(anilist_account) {

    connect(this->anilist_api_, &AnilistApi::userFetchFinished, this, &AccountManager::onUserFetchFinished);
    connect(this->anilist_api_, &AnilistApi::userFetchFailed, this, &AccountManager::onUserFetchFailed);
}

void AccountManager::ensureLoggedIn() {
    if (!this->anilist_account_->isValid()) {
        emit warningOccurred(
            QStringLiteral("Login"),
            QStringLiteral("Please login by setting an auth token")
        );
        return;
    }

    emit userUpdated(this->anilist_account_->user());
}

void AccountManager::requestAuthUrl() {
    emit authUrlReady(
        this->anilist_account_->authUrl()
    );
}

void AccountManager::requestProfileUrl() {
    auto profile_url = this->anilist_account_->profileUrl();
    if (!profile_url) {
        emit errorOccurred(
            QStringLiteral("Profile"),
            profile_url.error()
        );
        return;
    }
    emit profileUrlReady(profile_url.value());
}

void AccountManager::requestListUrl() {
    auto list_url = this->anilist_account_->listUrl();
    if (!list_url) {
        emit errorOccurred(
            QStringLiteral("Profile"),
            list_url.error()
        );
        return;
    }
    emit listUrlReady(list_url.value());
}

void AccountManager::requestStatsUrl() {
    auto stats_url = this->anilist_account_->statsUrl();
    if (!stats_url) {
        emit errorOccurred(
            QStringLiteral("Profile"),
            stats_url.error()
        );
        return;
    }
    emit statsUrlReady(stats_url.value());
}

void AccountManager::requestUserUpdate() {
    this->fetchUser();
}

void AccountManager::setAuthToken(const QString &auth_token) {
    if (auth_token.isEmpty()) {
        emit errorOccurred(
            QStringLiteral("Login"),
            QStringLiteral("Empty auth token")
        );
        return;
    }

    this->anilist_account_->setAuthToken(auth_token);
    this->fetchUser();
}

void AccountManager::fetchUser() {
    if (this->fetch_in_progress_) {
        return;
    }

    emit fetchStarted(
        QStringLiteral("Anilist"),
        QStringLiteral("Fetching user...")
    );

    this->fetch_in_progress_ = true;
    this->anilist_api_->fetchUser();
}

void AccountManager::onUserFetchFailed(const QString &message) {
    this->fetch_in_progress_ = false;
    emit fetchFinished();
    emit errorOccurred(
        QStringLiteral("Login"),
        message
    );
}

void AccountManager::onUserFetchFinished(const AnilistAccount::User &user) {
    this->fetch_in_progress_ = false;
    emit fetchFinished();

    bool new_user_added = user.user_id != this->anilist_account_->user().user_id;

    qDebug() << new_user_added;

    this->anilist_account_->setUser(user);

    if (!this->anilist_account_->isValid()) {
        emit errorOccurred(
            QStringLiteral("Login"),
            QStringLiteral("Invalid user received from Anilist")
        );
        return;
    }

    if (new_user_added) {
        emit requestRecreateDatabase();
    }

    emit userUpdated(this->anilist_account_->user());
}

