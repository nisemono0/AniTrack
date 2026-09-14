#pragma once

#include "base/database/database.hpp"

#include "base/cache/recognition_cache.hpp"

#include "base/recognition/mpris_watcher.hpp"
#include "base/recognition/anime_redirection.hpp"

#include "base/recognition/recognized_anime.hpp"

#include <QObject>
#include <QString>


class RecognitionManager final : public QObject {
    Q_OBJECT;
public:
    explicit RecognitionManager(
        Database *database,
        QObject *parent = nullptr
    );
    ~RecognitionManager() = default;

    void registerRunningPlayers();

public slots:
    void onIdSearchFinished(const QList<AnilistMedia> &media_list);
    void onIdSearchFailed(const QString &message);

    void onAnimeSelected(const AnilistMedia &media);

signals:
    void requestShowNoMatchPage(const QString &message);
    void requestShowErrorPage(const QString &message);
    void requestShowIdlePage();
    void requestShowSelectAnimePage(const QList<RecognizedAnime> &recognized_anime, const QString &title);
    void requestShowNowPlayingPage(const RecognizedAnime &recognized_anime, const QString &title);

    void requestShowSearchPage(const QString &title);

    void requestAnimeSearchById(const QList<int> &media_ids);

private:
    Database *database_;

    QString recognized_title_;
    QList<RecognizedAnime> recognized_anime_;
    QHash<int, int> missing_ids_to_redirection_episode_;

    double min_score_ = 0.5;
    int max_matches_ = 5;

    MprisWatcher *mpris_watcher_;
    RecognitionCache *recognition_cache_;
    AnimeRedirection *anime_redirection_;

    void resetRecognition();

    void initCache();
    void initWatcher();

    void failRecognition(const QString &message);

    void handleExactMatch(const int media_id, const AnimeFileParser::AnimeFileInfo &file_info);
    void handlePartialMatches(const QList<int> &media_ids, const AnimeFileParser::AnimeFileInfo &file_info);

private slots:
    void onMediaFileChanged(const QString &filename);
    void onMediaPlayerClosed();

};

