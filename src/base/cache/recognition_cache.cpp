#include "base/cache/recognition_cache.hpp"

#include "base/recognition/title_normalizer.hpp"
#include "base/recognition/string_similarity.hpp"

#include "utils/log.hpp"

#include <algorithm>


namespace {
QSet<QString> makeTrigrams(const QString &text) {
    QSet<QString> trigrams;

    if (text.isEmpty()) {
        return trigrams;
    }

    if (text.size() <= 3) {
        trigrams.insert(text);
        return trigrams;
    }

    // Reserve number of total trigrams
    trigrams.reserve(text.size() - 2);

    for (int i = 0; i <= text.size() - 3; i++) {
        trigrams.insert(text.sliced(i, 3));
    }

    return trigrams;
}
} // namespace


RecognitionCache::RecognitionCache(QObject *parent) : QObject(parent) {
    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Cache initialized with 0 media entries")
    );
}

RecognitionCache::RecognitionCache(
    const QList<AnilistMedia> &media_list,
    QObject *parent
) : QObject(parent) {

    this->media_id_to_normalized_titles_.reserve(media_list.size());

    for (const auto &media : media_list) {
        this->add(media);
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Cache initialized with %1 media entries").arg(media_list.size())
    );
}

void RecognitionCache::add(const AnilistMedia &media) {
    const int media_id = media.id;

    this->cacheTitle(media_id, media.title.english);
    this->cacheTitle(media_id, media.title.native);
    this->cacheTitle(media_id, media.title.romaji);

    for (const auto &synonym : media.synonyms) {
        this->cacheTitle(media_id, synonym);
    }
}

void RecognitionCache::add(const QString &title, int media_id) {
    this->cacheTitle(media_id, title);
}

void RecognitionCache::remove(int media_id) {
    const auto titles = this->media_id_to_normalized_titles_.take(media_id);

    for (const auto &title : titles) {
        auto title_it = this->normalized_title_to_media_ids_.find(title);
        if (title_it != this->normalized_title_to_media_ids_.end()) {
            title_it->remove(media_id);

            if (title_it->isEmpty()) {
                this->normalized_title_to_media_ids_.erase(title_it);
            }
        }

        const auto trigrams = makeTrigrams(title);
        for (const auto &trigram : trigrams) {
            auto trigram_it = this->trigrams_to_media_ids_.find(trigram);
            if (trigram_it != this->trigrams_to_media_ids_.end()) {
                trigram_it->remove(media_id);

                if (trigram_it->isEmpty()) {
                    this->trigrams_to_media_ids_.erase(trigram_it);
                }
            }
        }
    }
}

QList<RecognitionCache::TitleMatch> RecognitionCache::findMatches(const QString &title,
                                                                  double min_levenshtein_score,
                                                                  int max_matches) {

    if (max_matches <= 0) {
        return {};
    }

    QList<TitleMatch> matches;

    const QString normalized_title = TitleNormalizer::normalize(title);

    if (normalized_title.isEmpty()) {
        return {};
    }

    const auto exact_match = this->exactMatch(normalized_title);
    if (exact_match) {
        matches.append({
            exact_match.value(),
            ExactMatch
        });
        Log::info(
            CONTEXT_CLASS,
            QStringLiteral("Found exact match: %1 -> %2").arg(title).arg(exact_match.value())
        );
        return matches;
    }

    const auto search_candidates = this->findSearchCandidates(normalized_title);
    if (search_candidates.isEmpty()) {
        return {};
    }

    for (const int media_id : search_candidates) {
        const auto title_it = this->media_id_to_normalized_titles_.constFind(media_id);
        if (title_it == this->media_id_to_normalized_titles_.constEnd()) {
            continue;
        }

        double best_score = 0.0;

        for (const auto &candidate_title : title_it.value()) {
            best_score = qMax(
                best_score,
                StringSimilarity::levenshteinSimilarity(normalized_title, candidate_title)
            );
        }

        if (best_score >= min_levenshtein_score) {
            matches.append({
                media_id,
                best_score
            });
        }
    }

    std::sort(
        matches.begin(),
        matches.end(),
        [] (const TitleMatch &l, const TitleMatch &r) {
            return l.score > r.score;
        }
    );

    if (matches.size() > max_matches) {
        matches.resize(max_matches);
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Found %1 partial matches for %2").arg(matches.size()).arg(title)
    );

    return matches;
}

void RecognitionCache::cacheTitle(int media_id, const QString &title) {
    const QString normalized_title = TitleNormalizer::normalize(title);

    if (normalized_title.isEmpty()) {
        return;
    }

    this->media_id_to_normalized_titles_[media_id].insert(normalized_title);
    this->normalized_title_to_media_ids_[normalized_title].insert(media_id);

    const auto trigrams = makeTrigrams(normalized_title);
    for (const auto &trigram : trigrams) {
        this->trigrams_to_media_ids_[trigram].insert(media_id);
    }
}

std::optional<int> RecognitionCache::exactMatch(const QString &normalized_title) {
    const auto title_it = this->normalized_title_to_media_ids_.constFind(normalized_title);
    if (title_it == this->normalized_title_to_media_ids_.constEnd()) {
        return std::nullopt;
    }

    if (title_it->size() == 1) {
        return *title_it->constBegin();
    }
    return std::nullopt;
}

QSet<int> RecognitionCache::findSearchCandidates(const QString &normalized_title) {
    QSet<int> candidates;

    const auto trigrams = makeTrigrams(normalized_title);

    for (const auto &trigram : trigrams) {
        const auto trigram_it = this->trigrams_to_media_ids_.constFind(trigram);
        if (trigram_it == this->trigrams_to_media_ids_.constEnd()) {
            continue;
        }

        candidates.unite(trigram_it.value());
    }

    return candidates;
}

