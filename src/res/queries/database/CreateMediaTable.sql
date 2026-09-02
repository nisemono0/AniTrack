CREATE TABLE IF NOT EXISTS media (
    -- Anilist media id
    id INTEGER PRIMARY KEY,
    is_adult INTEGER NOT NULL DEFAULT FALSE,
    media_status INTEGER NOT NULL DEFAULT 0,
    season INTEGER NOT NULL DEFAULT 0,
    season_year INTEGER NOT NULL DEFAULT 0,
    average_score INTEGER NOT NULL DEFAULT 0,
    site_url TEXT,
    title_english TEXT,
    title_native TEXT,
    title_romaji TEXT,
    -- JSON array
    -- Example: ["Hyakkano 3", "รักรักรักรักรักเธอหมดหัวใจจากแฟนสาว 100 คน ซีซั่น 3"]
    synonyms TEXT,
    format INTEGER NOT NULL DEFAULT 0,
    description TEXT,
    -- Qt::ISODate
    start_date TEXT,
    end_date TEXT,
    -- Unix time
    updated_at INTEGER NOT NULL DEFAULT 0,
    episodes INTEGER NOT NULL DEFAULT 0,
    country_of_origin TEXT,
    cover_url TEXT,
    -- JSON array
    -- Example: [ "Action", "Drama" ]
    genres TEXT,
    -- JSON array of objects
    -- Example:
    -- [
    --   {
    --     "name": "Kyoto Animation",
    --     "site_url": "https://www.kyotoanimation.co.jp"
    --   }
    -- ]
    studios TEXT,
    -- Same as above
    producers TEXT
);
