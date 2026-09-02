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
RETURNING local_id as entry_local_id;
