#pragma once

#include <QString>

namespace AnilistKeys {
namespace Variables {
inline const QString UserId                 = QStringLiteral("userId");
inline const QString Search                 = QStringLiteral("search");
inline const QString DeleteMediaListEntryId = QStringLiteral("deleteMediaListEntryId");
inline const QString SaveMediaListEntryId   = QStringLiteral("saveMediaListEntryId");
inline const QString MediaId                = QStringLiteral("mediaId");
inline const QString Status                 = QStringLiteral("status");
inline const QString ScoreRaw               = QStringLiteral("scoreRaw");
inline const QString Progress               = QStringLiteral("progress");
inline const QString Repeat                 = QStringLiteral("repeat");
inline const QString Notes                  = QStringLiteral("notes");
inline const QString StartedAt              = QStringLiteral("startedAt");
inline const QString CompletedAt            = QStringLiteral("completedAt");
} // namespace Variables

namespace Common {
inline const QString Data       = QStringLiteral("data");
inline const QString Errors     = QStringLiteral("errors");
inline const QString Message    = QStringLiteral("message");
inline const QString Status     = QStringLiteral("status");
inline const QString Validation = QStringLiteral("validation");
}; // namespace Common

namespace Query {
inline const QString Viewer              = QStringLiteral("Viewer");
inline const QString MediaListCollection = QStringLiteral("MediaListCollection");
inline const QString Media               = QStringLiteral("Media");
inline const QString Page                = QStringLiteral("Page");
} // namespace Query

namespace Mutation {
inline const QString DeleteMediaListEntry = QStringLiteral("DeleteMediaListEntry");
inline const QString SaveMediaListEntry   = QStringLiteral("SaveMediaListEntry");
} // namespace Mutation

namespace DeleteMediaListEntry {
inline const QString Deleted = QStringLiteral("deleted");
}

namespace User {
inline const QString Id               = QStringLiteral("id");
inline const QString Name             = QStringLiteral("name");
inline const QString Avatar           = QStringLiteral("avatar");
inline const QString MediaListOptions = QStringLiteral("mediaListOptions");
inline const QString Options          = QStringLiteral("options");
inline const QString Statistics       = QStringLiteral("statistics");
}; // namespace User

namespace MediaListOptions {
inline const QString ScoreFormat = QStringLiteral("scoreFormat");
}

namespace UserOptions {
inline const QString UserTitleLanguage = QStringLiteral("titleLanguage");
}

namespace UserStatisticTypes {
inline const QString Anime = QStringLiteral("anime");
}

namespace UserStatistics {
inline const QString Count             = QStringLiteral("count");
inline const QString EpisodesWatched   = QStringLiteral("episodesWatched");
inline const QString MeanScore         = QStringLiteral("meanScore");
inline const QString MinutesWatched    = QStringLiteral("minutesWatched");
inline const QString StandardDeviation = QStringLiteral("standardDeviation");
} // namespace UserStatistics

namespace UserAvatar {
inline const QString Medium = QStringLiteral("medium");
}

namespace MediaListCollection {
inline const QString Lists = QStringLiteral("lists");
}

namespace MediaListGroup {
inline const QString Entries = QStringLiteral("entries");
}

namespace MediaList {
inline const QString CompletedAt = QStringLiteral("completedAt");
inline const QString Id          = QStringLiteral("id");
inline const QString Media       = QStringLiteral("media");
inline const QString Private     = QStringLiteral("private");
inline const QString Progress    = QStringLiteral("progress");
inline const QString Repeat      = QStringLiteral("repeat");
inline const QString Score       = QStringLiteral("score");
inline const QString StartedAt   = QStringLiteral("startedAt");
inline const QString Status      = QStringLiteral("status");
inline const QString UpdatedAt   = QStringLiteral("updatedAt");
inline const QString Notes       = QStringLiteral("notes");
} // namespace MediaList

namespace FuzzyDate {
inline const QString Day   = QStringLiteral("day");
inline const QString Month = QStringLiteral("month");
inline const QString Year  = QStringLiteral("year");
} // namespace FuzzyDate

namespace Media {
inline const QString Id              = QStringLiteral("id");
inline const QString IsAdult         = QStringLiteral("isAdult");
inline const QString Status          = QStringLiteral("status");
inline const QString Season          = QStringLiteral("season");
inline const QString SeasonYear      = QStringLiteral("seasonYear");
inline const QString SiteUrl         = QStringLiteral("siteUrl");
inline const QString Title           = QStringLiteral("title");
inline const QString Synonyms        = QStringLiteral("synonyms");
inline const QString Format          = QStringLiteral("format");
inline const QString Description     = QStringLiteral("description");
inline const QString StartDate       = QStringLiteral("startDate");
inline const QString EndDate         = QStringLiteral("endDate");
inline const QString UpdatedAt       = QStringLiteral("updatedAt");
inline const QString Episodes        = QStringLiteral("episodes");
inline const QString CountryOfOrigin = QStringLiteral("countryOfOrigin");
inline const QString CoverImage      = QStringLiteral("coverImage");
inline const QString Genres          = QStringLiteral("genres");
inline const QString Studios         = QStringLiteral("studios");
inline const QString AverageScore    = QStringLiteral("averageScore");
} // namespace Media

namespace MediaCoverImage {
inline const QString Large = QStringLiteral("large");
};

namespace MediaTitle {
inline const QString English = QStringLiteral("english");
inline const QString Native  = QStringLiteral("native");
inline const QString Romaji  = QStringLiteral("romaji");
}; // namespace MediaTitle

namespace StudioConnection {
inline const QString Edges = QStringLiteral("edges");
}

namespace StudioEdge {
inline const QString IsMain = QStringLiteral("isMain");
inline const QString Node   = QStringLiteral("node");
} // namespace StudioEdge

namespace Studio {
inline const QString Name    = QStringLiteral("name");
inline const QString SiteUrl = QStringLiteral("siteUrl");
} // namespace Studio

namespace Page {
inline const QString Media = QStringLiteral("media");
}
}; // namespace AnilistKeys

