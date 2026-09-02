CREATE TABLE IF NOT EXISTS entry (
    -- Local id
    local_id INTEGER PRIMARY KEY AUTOINCREMENT,
    -- Anilist entry id
    id INTEGER UNIQUE,
    -- Media id
    media_id INTEGER NOT NULL UNIQUE,
    is_private INTEGER NOT NULL DEFAULT FALSE,
    status INTEGER NOT NULL DEFAULT 0,
    progress INTEGER NOT NULL DEFAULT 0,
    repeat INTEGER NOT NULL DEFAULT 0,
    notes TEXT,
    score INTEGER NOT NULL DEFAULT 0,
    -- Qt::ISODate
    started_at TEXT,
    completed_at TEXT,
    -- Unix time
    updated_at INTEGER NOT NULL DEFAULT 0,
    pending_operation INTEGER NOT NULL DEFAULT 0,

    FOREIGN KEY (media_id) REFERENCES media (id) ON DELETE CASCADE
);
