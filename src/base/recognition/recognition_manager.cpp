#include "base/recognition/recognition_manager.hpp"

#include "utils/log.hpp"

#include "base/recognition/title_normalizer.hpp"

#include <algorithm>


RecognitionManager::RecognitionManager(
    Database *database,
    QObject *parent
) : QObject(parent),
    database_(database) {

    this->initCache();
    this->initWatcher();

    this->anime_redirection_ = new AnimeRedirection(this);
}

void RecognitionManager::registerRunningPlayers() {
    this->mpris_watcher_->registerRunningPlayers();
}

void RecognitionManager::onIdSearchFinished(const QList<AnilistMedia> &media_list) {
    for (const auto &media : media_list) {
        if (!this->missing_ids_to_redirection_episode_.contains(media.id)) {
            Log::warning(
                CONTEXT_CLASS,
                QStringLiteral("No redirection for id: %1").arg(media.id)
            );
            continue;
        }

        RecognizedAnime recognized;
        recognized.episode = this->missing_ids_to_redirection_episode_.value(media.id);
        recognized.media = media;
        recognized.entry = std::nullopt;

        this->recognized_anime_.append(std::move(recognized));
    }

    if (this->recognized_anime_.isEmpty()) {
        emit showNoMatchPage(this->recognized_title_);
        return;
    }

    if (this->recognized_anime_.size() == 1) {
        emit showNowPlaying(
            this->recognized_anime_.constFirst(),
            this->recognized_title_
        );
        return;
    }

    emit showSelectAnimePage(this->recognized_anime_, this->recognized_title_);
}

void RecognitionManager::onIdSearchFailed(const QString &message) {
    this->failRecognition(
        QStringLiteral("Failed to search missing ids: %1").arg(message)
    );
}

void RecognitionManager::onAnimeSelected(const AnilistMedia &media) {
    // user selected an unkown anime. cache it
    this->recognition_cache_->add(media);
}

void RecognitionManager::resetRecognition() {
    this->recognized_title_.clear();
    this->recognized_anime_.clear();
    this->missing_ids_to_redirection_episode_.clear();
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
    emit showErrorPage(message);
}

void RecognitionManager::handleExactMatch(const int media_id, const AnimeFileParser::AnimeFileInfo &file_info) {
    const auto redirection = this->anime_redirection_->redirect(media_id, file_info.episode);

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
        this->missing_ids_to_redirection_episode_.insert(
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
    recognized.episode = redirection.episode;
    recognized.entry = anime.entry;
    recognized.media = anime.media;

    this->recognized_anime_.append(recognized);

    // recognized item in database, request to show it
    emit showNowPlaying(recognized, this->recognized_title_);
}

void RecognitionManager::handlePartialMatches(const QList<int> &media_ids, const AnimeFileParser::AnimeFileInfo &file_info) {
    QList<AnimeRedirection::Redirection> redirections;
    QList<int> redirected_media_ids;
    for (const auto &id : media_ids) {
        const auto redirection = this->anime_redirection_->redirect(id, file_info.episode);

        redirected_media_ids.append(redirection.media_id);
        redirections.append(std::move(redirection));
    }

    const auto local_anime = this->database_->selectAnimeByMediaIds(redirected_media_ids);
    if (!local_anime) {
        this->failRecognition(
            QStringLiteral("Failed to select partial matches: %1").arg(local_anime.error())
        );
        return;
    }

    // no local anime with recognized ids, search them all
    if (local_anime->isEmpty()) {
        emit requestAnimeSearchById(redirected_media_ids);
        return;
    }

    // add the local only ids in a set and add the matching anime
    // to the recognized anime list
    QSet<int> local_ids;
    for (const auto &anime : local_anime.value()) {
        local_ids.insert(anime.media.id);

        const auto it = std::ranges::find_if(
            redirections,
            [&anime] (const AnimeRedirection::Redirection &redirection) {
                return anime.media.id == redirection.media_id;
            }
        );

        if (it == redirections.end()) {
            continue;
        }

        RecognizedAnime recognized;
        recognized.episode = it->episode;
        recognized.entry = anime.entry;
        recognized.media = anime.media;

        this->recognized_anime_.append(std::move(recognized));
    }

    // get a list of missing redirected ids to search on anilist
    QList<int> missing_ids;
    for(const auto &redirection : std::as_const(redirections)) {
        if (!local_ids.contains(redirection.media_id)) {
            missing_ids.append(redirection.media_id);
            // store the missing ids redirection episode
            this->missing_ids_to_redirection_episode_.insert(
                redirection.media_id,
                redirection.episode
            );
        }
    }

    // no missing ids, show the recognized anime list
    if (missing_ids.isEmpty()) {
        emit showSelectAnimePage(this->recognized_anime_, this->recognized_title_);
        return;
    }

    // search the missing local ids
    emit requestAnimeSearchById(missing_ids);
}

void RecognitionManager::onMediaFileChanged(const QString &filename) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Now playing: %1").arg(filename)
    );

    this->resetRecognition();

    const auto file_info = AnimeFileParser::parse(filename);
    if (file_info.title.isEmpty() || file_info.episode == AnimeFileParser::InvalidEpisode) {
        this->failRecognition(
            QStringLiteral("Failed to retrieve filename title: %1").arg(filename)
        );
        return;
    }

    this->recognized_title_ = file_info.title;

    const auto matches = this->recognition_cache_->findMatches(
        file_info.title,
        this->min_score_,
        this->max_matches_
    );

    // No recognition matches, show search page
    if (matches.isEmpty()) {
        emit showSearchPage(file_info.title);
        return;
    }

    // Matches are sorted based on score, so first one has the
    // the best score or is an exact match
    const auto &first_match = matches.constFirst();
    if (first_match.score == RecognitionCache::ExactMatch) {
        this->handleExactMatch(first_match.media_id, file_info);
        return;
    }

    // Partial matches
    QList<int> media_ids;
    for (const auto &match : matches) {
        media_ids.append(match.media_id);
    }
    this->handlePartialMatches(media_ids, file_info);
}

void RecognitionManager::onMediaPlayerClosed() {
    this->resetRecognition();
    emit showIdlePage();
}

