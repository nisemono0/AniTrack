
SELECT
    e.local_id AS entry_local_id,
    e.id AS entry_id,
    e.media_id AS entry_media_id,
    e.is_private AS entry_is_private,
    e.status AS entry_status,
    e.progress AS entry_progress,
    e.repeat AS entry_repeat,
    e.notes AS entry_notes,
    e.score AS entry_score,
    e.started_at AS entry_started_at,
    e.completed_at AS entry_completed_at,
    e.updated_at AS entry_updated_at,
    e.pending_operation AS entry_pending_operation,

    m.id AS media_id,
    m.is_adult AS media_is_adult,
    m.media_status AS media_status,
    m.season AS media_season,
    m.season_year AS media_season_year,
    m.average_score AS media_average_score,
    m.site_url AS media_site_url,
    m.title_english AS media_title_english,
    m.title_native AS media_title_native,
    m.title_romaji AS media_title_romaji,
    m.synonyms AS media_synonyms,
    m.format AS media_format,
    m.description AS media_description,
    m.start_date AS media_start_date,
    m.end_date AS media_end_date,
    m.updated_at AS media_updated_at,
    m.episodes AS media_episodes,
    m.country_of_origin AS media_country_of_origin,
    m.cover_url AS media_cover_url,
    m.genres AS media_genres,
    m.studios AS media_studios,
    m.producers AS media_producers
FROM entry e
INNER JOIN media m ON e.media_id = m.id
WHERE e.pending_operation <> 0;

