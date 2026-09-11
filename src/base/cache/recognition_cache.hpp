#pragma once

#include "base/anilist/anilist_media.hpp"

#include <QObject>
#include <QHash>


class RecognitionCache final : public QObject {
    Q_OBJECT;
public:
    struct TitleMatch {
        int media_id;
        double score;
    };

    explicit RecognitionCache(QObject *parent = nullptr);
    explicit RecognitionCache(
        const QList<AnilistMedia> &media_list,
        QObject *parent = nullptr
    );
    ~RecognitionCache() = default;

    void add(const AnilistMedia &media);
    void add(const QString &title, int media_id);

    void remove(int media_id);

    QList<TitleMatch> findMatches(const QString &title,
                                  double min_levenshtein_score = 0.5,
                                  int max_matches = 10);

private:
    QHash<int, QSet<QString>> media_id_to_normalized_titles_;
    QHash<QString, QSet<int>> normalized_title_to_media_ids_;
    QHash<QString, QSet<int>> trigrams_to_media_ids_;

    void cacheTitle(int media_id, const QString &title);

    std::optional<int> exactMatch(const QString &normalized_title);
    QSet<int> findSearchCandidates(const QString &normalized_title);
};

