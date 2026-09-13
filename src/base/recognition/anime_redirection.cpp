#include "base/recognition/anime_redirection.hpp"

#include "base/recognition/recognition_resources.hpp"

#include "utils/log.hpp"

#include <QFile>
#include <QTextStream>
#include <QRegularExpression>


AnimeRedirection::AnimeRedirection(QObject *parent) : QObject(parent) {
    QFile file(RecognitionResources::AnimeRelations);
    if (!file.open(QFile::ReadOnly | QFile::Text)) {
        Log::error(
            CONTEXT_CLASS,
            QStringLiteral("Failed to open %1 for reading").arg(RecognitionResources::AnimeRelations)
        );
        return;
    }

    QStringList rules;
    while (!file.atEnd()) {
        const QString rule_line = file.readLine().trimmed();

        if (rule_line.isEmpty() ||
            rule_line.startsWith('#') ||
            rule_line.startsWith("::")) {
            continue;
        }

        rules.append(rule_line);
    }

    static const QRegularExpression rule_re(
        R"(^- (?<src_ids>(?:\d+|[?~])(?:\|(?:\d+|[?~]))*):(?<src_start>\d+)(?:-(?<src_end>\d+|\?))? -> )"
        R"((?<dst_ids>(?:\d+|[?~])(?:\|(?:\d+|[?~]))*):(?<dst_start>\d+)(?:-(?<dst_end>\d+|\?))?(?<bang>!)?$)"
    );

    for (const auto &rule : std::as_const(rules)) {
        const auto match = rule_re.match(rule);

        if (!match.hasMatch()) {
            continue;
        }

        const auto src_id = this->parseSourceIds(
            match.captured(QStringLiteral("src_ids"))
        );
        if (!src_id) {
            continue;
        }

        const auto dst_id = this->parseDestinationIds(
            match.captured(QStringLiteral("dst_ids")),
            src_id.value()
        );
        if (!dst_id) {
            continue;
        }

        const auto src_ep_range = this->parseEpisodeRange(
            match.captured(QStringLiteral("src_start")),
            match.captured(QStringLiteral("src_end"))
        );

        const auto dst_ep_range = this->parseEpisodeRange(
            match.captured(QStringLiteral("dst_start")),
            match.captured(QStringLiteral("dst_end"))
        );

        this->redirections_[src_id.value()].append({
            .source_id = src_id.value(),
            .destination_id = dst_id.value(),
            .source_episode_start = src_ep_range.first,
            .source_episode_end = src_ep_range.second,
            .destination_episode_start = dst_ep_range.first,
            .destination_episode_end = dst_ep_range.second
        });

        const bool self_redirect = !match.captured(QStringLiteral("bang")).isEmpty();
        if (self_redirect) {
            this->redirections_[dst_id.value()].append({
                .source_id = dst_id.value(),
                .destination_id = dst_id.value(),
                .source_episode_start = src_ep_range.first,
                .source_episode_end = src_ep_range.second,
                .destination_episode_start = dst_ep_range.first,
                .destination_episode_end = dst_ep_range.second
            });
        }
    }

    Log::info(
        CONTEXT_CLASS,
        QStringLiteral("Loaded redirection rules")
    );
}

AnimeRedirection::Redirection AnimeRedirection::redirect(const int media_id, const int episode) {
    const auto redirections = this->redirections(media_id);

    Redirection anime_redirection{
        .media_id = media_id,
        .episode =episode
    };

    if (redirections.isEmpty()) {
        return anime_redirection;
    }

    for (const auto &rule : redirections) {
        if (episode < rule.source_episode_start ||
            episode > rule.source_episode_end) {
            continue;
        }

        anime_redirection.media_id = rule.destination_id;
        anime_redirection.episode = rule.destination_episode_start + (episode - rule.source_episode_start);
        return anime_redirection;
    }

    return anime_redirection;
}

QList<AnimeRedirection::RedirectionInfo> AnimeRedirection::redirections(int media_id) const {
    return this->redirections_.value(media_id);
}

std::optional<int> AnimeRedirection::parseSourceIds(const QString &src_ids) const {
    const QStringList id_parts = src_ids.split('|');

    if (id_parts.size() != 3) {
        return std::nullopt;
    }

    const QString &anilist_id = id_parts.at(2);

    if (anilist_id == '?' || anilist_id == '~') {
        return std::nullopt;
    }

    bool ok = false;
    const int id = anilist_id.toInt(&ok);

    return ok ? std::optional(id) : std::nullopt;
}

std::optional<int> AnimeRedirection::parseDestinationIds(const QString &dst_ids, int repeat_id) const {
    const QStringList id_parts = dst_ids.split('|');

    if (id_parts.size() != 3) {
        return std::nullopt;
    }

    const QString &anilist_id = id_parts.at(2);

    if (anilist_id == '?') {
        return std::nullopt;
    }

    if (anilist_id == '~') {
        return repeat_id;
    }

    bool ok = false;
    const int id = anilist_id.toInt(&ok);

    return ok ? std::optional(id) : std::nullopt;
}

QPair<int, int> AnimeRedirection::parseEpisodeRange(const QString &start, const QString &end) const {
    // ep_start should always be available
    const int start_ep = start.toInt();

    if (end.isEmpty()) {
        return {start_ep, start_ep};
    }

    bool ok = false;
    const int end_ep = end.toInt(&ok);

    return {
        start_ep,
        ok ? end_ep : INT_MAX
    };
}

