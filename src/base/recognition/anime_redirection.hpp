#pragma once

#include <QObject>
#include <QString>
#include <QHash>

#include <optional>


class AnimeRedirection final : public QObject {
    Q_OBJECT;
public:
    struct Redirection {
        int media_id;
        int episode;
    };
    explicit AnimeRedirection(QObject *parent = nullptr);
    ~AnimeRedirection() = default;

    Redirection redirect(const int media_id, const int episode);

private:
    struct RedirectionInfo {
        int source_id = -1;
        int destination_id = -1;
        int source_episode_start = -1;
        int source_episode_end = -1;
        int destination_episode_start = -1;
        int destination_episode_end = -1;
    };
    QHash<int, QList<RedirectionInfo>> redirections_;

    QList<RedirectionInfo> redirections(int media_id) const;

    std::optional<int> parseSourceIds(const QString &src_ids) const;
    std::optional<int> parseDestinationIds(const QString &dst_ids, int repeat_id) const;

    QPair<int, int> parseEpisodeRange(const QString &start, const QString &end) const;

};

