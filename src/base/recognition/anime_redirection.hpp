#pragma once

#include <QObject>
#include <QString>
#include <QHash>

#include <optional>


class AnimeRedirection final : public QObject {
    Q_OBJECT;
public:
    struct EpisodeRedirection {
        int source_id = -1;
        int destination_id = -1;
        int source_episode_start = -1;
        int source_episode_end = -1;
        int destination_episode_start = -1;
        int destination_episode_end = -1;
    };
    explicit AnimeRedirection(QObject *parent = nullptr);
    ~AnimeRedirection() = default;

    QList<EpisodeRedirection> redirections(int media_id) const;

private:
    QHash<int, QList<EpisodeRedirection>> redirections_;

    std::optional<int> parseSourceIds(const QString &src_ids) const;
    std::optional<int> parseDestinationIds(const QString &dst_ids, int repeat_id) const;

    QPair<int, int> parseEpisodeRange(const QString &start, const QString &end) const;

};

