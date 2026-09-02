INSERT INTO entry (
    id,
    media_id,
    is_private,
    status,
    progress,
    repeat,
    notes,
    score,
    started_at,
    completed_at,
    updated_at,
    pending_operation
)
VALUES (
    :id,
    :media_id,
    :is_private,
    :status,
    :progress,
    :repeat,
    :notes,
    :score,
    :started_at,
    :completed_at,
    :updated_at,
    :pending_operation
)
ON CONFLICT(media_id) DO UPDATE SET
    id = excluded.id,
    status = excluded.status,
    progress = excluded.progress,
    repeat = excluded.repeat,
    notes = excluded.notes,
    score = excluded.score,
    started_at = excluded.started_at,
    completed_at = excluded.completed_at,
    updated_at = excluded.updated_at,
    pending_operation = excluded.pending_operation;
