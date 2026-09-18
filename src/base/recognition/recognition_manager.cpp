#include "base/recognition/recognition_manager.hpp"

#include "base/settings/app_settings.hpp"

#include "utils/log.hpp"
#include "utils/settings.hpp"

#include "base/recognition/title_normalizer.hpp"


RecognitionManager::RecognitionManager(
    Database *database,
    QObject *parent
) : QObject(parent),
    database_(database) {

    this->initCache();
    this->initWatcher();

    this->anime_redirection_ = new AnimeRedirection(this);

    this->applySettings();
    connect(App::instance()->settings(), &AppSettings::settingsChanged, this, &RecognitionManager::applySettings);
}

void RecognitionManager::registerRunningPlayers() {
    this->mpris_watcher_->registerRunningPlayers();
}

void RecognitionManager::onAnimeAddFinished(const QList<AnilistAnime> &anime_list) {
    for (const auto &anime : anime_list) {
        this->recognition_cache_->addMedia(anime.media);
    }
}

void RecognitionManager::onQuietSearchFinished(const QList<AnilistMedia> &media_list) {
    // quiet search is a new standalone result set
    this->recognized_anime_.clear();
    this->missing_ids_to_redirected_episode_.clear();

    QSet<int> redirected_ids;
    QHash<int, int> id_to_redirected_id;
    QHash<int, int> redirected_id_to_episode;
    for (const auto &media : media_list) {
        const auto redirection = this->anime_redirection_->redirect(media.id, this->recognized_episode_);
        id_to_redirected_id.insert(media.id, redirection.media_id);
        redirected_id_to_episode.insert(redirection.media_id, redirection.episode);

        redirected_ids.insert(redirection.media_id);
    }

    const auto local_anime = this->database_->selectAnimeByMediaIds(
        {redirected_ids.constBegin(), redirected_ids.constEnd()}
    );
    if (!local_anime) {
        this->failRecognition(
            QStringLiteral("Failed to select searched matches: %1").arg(local_anime.error())
        );
        return;
    }

    // handle local anime
    for (const auto &anime : local_anime.value()) {
        RecognizedAnime recognized;
        recognized.media = anime.media;
        recognized.entry = anime.entry;
        recognized.release_group = this->recognized_release_group_;
        recognized.episode = redirected_id_to_episode.value(
            anime.media.id,
            this->recognized_episode_
        );

        this->recognized_anime_.append(std::move(recognized));

        redirected_ids.remove(anime.media.id);
    }

    // handle non local anime
    for (const auto &media : media_list) {
        const auto redirected_id = id_to_redirected_id.value(media.id);

        if (!redirected_ids.contains(redirected_id)) {
            continue;
        }

        RecognizedAnime recognized;
        recognized.media = media;
        recognized.entry = std::nullopt;
        recognized.release_group = this->recognized_release_group_;
        recognized.episode = redirected_id_to_episode.value(
            redirected_id,
            this->recognized_episode_
        );

        this->recognized_anime_.append(std::move(recognized));
    }

    if (this->recognized_anime_.isEmpty()) {
        emit requestShowSearchPage(this->recognized_title_, this->recognized_episode_);
        return;
    }

    if (this->recognized_anime_.size() == 1) {
        const auto recognized = this->recognized_anime_.constFirst();
        this->recognition_cache_->add(this->recognized_title_, recognized.media.id);
        emit requestShowNowPlayingPage(
            recognized,
            this->recognized_title_
        );
        return;
    }

    emit requestShowSelectAnimePage(
        this->recognized_anime_,
        this->recognized_title_,
        this->recognized_episode_,
        this->recognized_release_group_
    );
}

void RecognitionManager::onIdSearchFinished(const QList<AnilistMedia> &media_list) {
    for (const auto &media : media_list) {
        if (!this->missing_ids_to_redirected_episode_.contains(media.id)) {
            Log::warning(
                CONTEXT_CLASS,
                QStringLiteral("No redirection for id: %1").arg(media.id)
            );
            continue;
        }

        RecognizedAnime recognized;
        recognized.media = media;
        recognized.entry = std::nullopt;
        recognized.release_group = this->recognized_release_group_;
        recognized.episode = this->missing_ids_to_redirected_episode_.value(media.id);

        this->recognized_anime_.append(std::move(recognized));
    }

    if (this->recognized_anime_.isEmpty()) {
        emit requestShowSearchPage(this->recognized_title_, this->recognized_episode_);
        return;
    }

    // single entry recognized, cache it and show it
    if (this->recognized_anime_.size() == 1) {
        const auto recognized = this->recognized_anime_.constFirst();
        this->recognition_cache_->add(this->recognized_title_, recognized.media.id);

        emit requestShowNowPlayingPage(
            recognized,
            this->recognized_title_
        );
        return;
    }

    // multiple entries recognized, ask to select the correct one
    emit requestShowSelectAnimePage(
        this->recognized_anime_,
        this->recognized_title_,
        this->recognized_episode_,
        this->recognized_release_group_
    );
}

void RecognitionManager::onQuietSearchFailed(const QString &message) {
    this->failRecognition(
        QStringLiteral("Failed to search recognition titles: %1").arg(message)
    );
}

void RecognitionManager::onIdSearchFailed(const QString &message) {
    this->failRecognition(
        QStringLiteral("Failed to search recognition ids: %1").arg(message)
    );
}

void RecognitionManager::onRecognizedAnimeSelected(const RecognizedAnime &selected_anime) {
    // cache and show the selected anime
    this->recognition_cache_->add(this->recognized_title_, selected_anime.media.id);
    emit requestShowNowPlayingPage(selected_anime, this->recognized_title_);
}

void RecognitionManager::resetRecognition() {
    this->recognized_episode_ = AnimeFileParser::InvalidEpisode;
    this->recognized_title_.clear();
    this->recognized_anime_.clear();
    this->missing_ids_to_redirected_episode_.clear();
}

void RecognitionManager::initCache() {
    const auto media_list = this->database_->selectAllMedia();
    if (!media_list) {
        Log::warning(
            CONTEXT_CLASS,
            QStringLiteral("Failed to initialize recognition cache: %1").arg(
                media_list.error()
            )
        );
        this->recognition_cache_ = new RecognitionCache(this);
        return;
    }

    this->recognition_cache_ = new RecognitionCache(media_list.value(), this);
}

void RecognitionManager::initWatcher() {
    this->mpris_watcher_ = new MprisWatcher(this);

    connect(this->mpris_watcher_, &MprisWatcher::mediaFileChanged, this, &RecognitionManager::onMediaFileChanged);
    connect(this->mpris_watcher_, &MprisWatcher::mediaPlayerClosed, this, &RecognitionManager::onMediaPlayerClosed);
}

void RecognitionManager::failRecognition(const QString &message) {
    this->resetRecognition();
    Log::warning(CONTEXT_CLASS, message);
    emit requestShowNoMatchPage(message);
}

void RecognitionManager::handleExactMatch(const int media_id) {
    const auto redirection = this->anime_redirection_->redirect(media_id, this->recognized_episode_);

    // recognized media_id after redirection
    const auto local_anime = this->database_->selectAnimeByMediaIds({redirection.media_id});
    if (!local_anime) {
        this->failRecognition(
            QStringLiteral("Failed to select single match: %1").arg(local_anime.error())
        );
        return;
    }

    // Recognized anime not in database, request a search on
    // the recognized id to get it from anilist
    if (local_anime->isEmpty()) {
        // store the missing id redirectioned episode
        this->missing_ids_to_redirected_episode_.insert(
            redirection.media_id,
            redirection.episode
        );
        emit requestAnimeSearchById({redirection.media_id});
        return;
    }

    // db media_id is unique so this should always return a single
    // anime for 1 id query
    const auto &anime = local_anime->constFirst();

    RecognizedAnime recognized;
    recognized.entry = anime.entry;
    recognized.media = anime.media;
    recognized.release_group = this->recognized_release_group_;
    recognized.episode = redirection.episode;

    this->recognized_anime_.append(recognized);

    // recognized item in database, request to show it
    emit requestShowNowPlayingPage(recognized, this->recognized_title_);
}

void RecognitionManager::handlePartialMatches(const QList<int> &media_ids) {
    QSet<int> redirected_ids;
    QHash<int, int> redirected_id_to_episode;
    for (const auto &id : media_ids) {
        const auto redirection = this->anime_redirection_->redirect(id, this->recognized_episode_);
        redirected_id_to_episode.insert(redirection.media_id, redirection.episode);

        redirected_ids.insert(redirection.media_id);
    }

    QList<int> redirected_ids_list{redirected_ids.constBegin(), redirected_ids.constEnd()};
    const auto local_anime = this->database_->selectAnimeByMediaIds(redirected_ids_list);
    if (!local_anime) {
        this->failRecognition(
            QStringLiteral("Failed to select partial matches: %1").arg(local_anime.error())
        );
        return;
    }

    // no local anime with recognized ids, search them all
    if (local_anime->isEmpty()) {
        emit requestAnimeSearchById(redirected_ids_list);
        return;
    }

    // handle local anime
    for (const auto &anime : local_anime.value()) {
        RecognizedAnime recognized;
        recognized.media = anime.media;
        recognized.entry = anime.entry;
        recognized.release_group = this->recognized_release_group_;
        recognized.episode = redirected_id_to_episode.value(
            anime.media.id,
            this->recognized_episode_
        );

        this->recognized_anime_.append(std::move(recognized));

        redirected_ids.remove(anime.media.id);
    }

    // remaining redirected ids are the missing ones
    QList<int> missing_ids;
    for(const auto &redirected_id : std::as_const(redirected_ids)) {
        missing_ids.append(redirected_id);
        this->missing_ids_to_redirected_episode_.insert(
            redirected_id,
            redirected_id_to_episode.value(
                redirected_id,
                this->recognized_episode_
            )
        );
    }

    // no missing ids, show the recognized anime list or
    // the now playing if only 1 exists
    if (missing_ids.isEmpty()) {
        if (this->recognized_anime_.size() == 1) {
            const auto recognized = this->recognized_anime_.constFirst();
            this->recognition_cache_->add(this->recognized_title_, recognized.media.id);

            emit requestShowNowPlayingPage(recognized, this->recognized_title_);
            return;
        }

        emit requestShowSelectAnimePage(
            this->recognized_anime_,
            this->recognized_title_,
            this->recognized_episode_,
            this->recognized_release_group_
        );
        return;
    }

    // search for the missing ids
    emit requestAnimeSearchById(missing_ids);
}

void RecognitionManager::onMediaFileChanged(const QString &filename) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Now playing: %1").arg(filename)
    );

    this->resetRecognition();

    if (!this->recognition_enabled_) {
        emit requestShowNoMatchPage(
            QStringLiteral("Recognition disabled")
        );
        return;
    }

    const auto file_info = AnimeFileParser::parse(filename);
    if (file_info.title.isEmpty() || file_info.episode == AnimeFileParser::InvalidEpisode) {
        this->failRecognition(
            QStringLiteral("Failed to retrieve filename title: %1").arg(filename)
        );
        return;
    }

    this->recognized_title_ = file_info.title;
    this->recognized_episode_ = file_info.episode;
    this->recognized_release_group_ = file_info.release_group;

    const auto matches = this->recognition_cache_->findMatches(
        file_info.title,
        this->min_score_,
        this->max_matches_
    );

    // No recognition matches, show search page
    if (matches.isEmpty()) {
        emit requestShowSearchPage(file_info.title, file_info.episode);
        return;
    }

    // Matches are sorted based on score, so first one has the
    // the best score or is an exact match
    const auto &first_match = matches.constFirst();
    if (first_match.score == RecognitionCache::ExactMatch) {
        this->handleExactMatch(first_match.media_id);
        return;
    }

    // Partial matches
    QList<int> media_ids;
    for (const auto &match : matches) {
        media_ids.append(match.media_id);
    }
    this->handlePartialMatches(media_ids);
}

void RecognitionManager::onMediaPlayerClosed() {
    this->resetRecognition();
    emit requestShowIdlePage();
}

void RecognitionManager::applySettings() {
    this->recognition_enabled_ = Settings::get(Settings::Recognition::EnableAnimeRecognition, true);
    this->min_score_ = Settings::get(Settings::Recognition::MinMatchScore, 0.5);
    this->max_matches_ = Settings::get(Settings::Recognition::MaxCacheMatches, 10);

    if (!this->recognition_enabled_) {
        this->resetRecognition();
        emit requestShowIdlePage();
    }
}

