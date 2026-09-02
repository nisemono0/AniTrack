DELETE FROM media
WHERE NOT EXISTS (
    SELECT 1 FROM ENTRY
    WHERE entry.media_id = media.id
);
