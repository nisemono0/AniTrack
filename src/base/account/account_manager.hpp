#pragma once

#include "base/anilist/anilist_account.hpp"
#include "base/anilist/anilist_api.hpp"

class AccountManager final : public QObject {
    Q_OBJECT;
public:
    explicit AccountManager(
        AnilistAccount *anilist_account,
        AnilistApi *anilist_api,
        QObject *parent = nullptr
    );
    ~AccountManager() = default;

    void ensureLoggedIn();

public slots:
    void requestAuthUrl();

    void requestProfileUrl();
    void requestListUrl();
    void requestStatsUrl();

    void requestUserUpdate();

    void setAuthToken(const QString &auth_token);

    void onUserFetchFailed(const QString &message);
    void onUserFetchFinished(const AnilistAccount::User &user);

signals:
    void authUrlReady(const QUrl &auth_url);

    void profileUrlReady(const QUrl &profile_url);
    void listUrlReady(const QUrl &list_url);
    void statsUrlReady(const QUrl &stats_url);

    void fetchStarted(const QString &title, const QString &message);
    void fetchFinished();

    void userUpdated(const AnilistAccount::User &user);

    void requestRecreateDatabase();

    void warningOccurred(const QString &context, const QString &message);
    void errorOccurred(const QString &context, const QString &message);

private:
    AnilistAccount *anilist_account_;
    AnilistApi *anilist_api_;

    bool fetch_in_progress_ = false;

    void fetchUser();

private slots:
};

