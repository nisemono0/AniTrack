#pragma once

#include <expected>

#include <QString>
#include <QJsonObject>


class AnilistAccount final : public QObject {
    Q_OBJECT;
public:
    explicit AnilistAccount(QObject *parent = nullptr);
    ~AnilistAccount() = default;

    // Account score format
    enum class ScoreFormat {
        INVALID = 0,
        POINT_100,
        POINT_10_DECIMAL,
        POINT_10,
        POINT_5,
        POINT_3
    };
    Q_ENUM(ScoreFormat);

    // Account title display language
    enum class TitleLanguage {
        INVALID = 0,
        ROMAJI,
        ENGLISH,
        NATIVE
    };
    Q_ENUM(TitleLanguage);

    // Anilist user
    static constexpr int InvalidId = -1;
    struct User final {
        int user_id = InvalidId;
        QString username;
        QString avatar_url;

        ScoreFormat score_format = ScoreFormat::INVALID;
        TitleLanguage title_language = TitleLanguage::INVALID;

        int anime_count = 0;
        int episodes_watched = 0;
        int minutes_watched = 0;
        double mean_score = 0.0;
        double std_deviation = 0.0;
    };

    // Return an anilist user from an anilist Viewer json object
    static AnilistAccount::User userFromViewerJson(const QJsonObject &json_object);

    // Set a new auth token for account
    void setAuthToken(const QString &auth_token);
    // Set a new anilist user
    void setUser(const AnilistAccount::User &user);

    // Check if the AnilistAccount is valid.
    // Valid means it has an auth token, user id and username
    bool isValid() const;

    // Return the anilist account auth token
    const QString& authToken() const;
    // Return the anilist user for the account
    const AnilistAccount::User& user() const;
    // Get the account's profile url or an error string
    std::expected<QUrl, QString> profileUrl() const;
    // Get the account's anime list url or an error string
    std::expected<QUrl, QString> listUrl() const;
    // Get the account's stats url or an error string
    std::expected<QUrl, QString> statsUrl() const;
    // Get the anilist auth url
    QUrl authUrl() const;

    bool operator==(const AnilistAccount &anilist_account) const;
    bool operator!=(const AnilistAccount &anilist_account) const;

private:
    QString auth_token_;
    User anilist_user_;

};

