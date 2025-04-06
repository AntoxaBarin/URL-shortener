DROP SCHEMA IF EXISTS shortener_schema CASCADE;

CREATE SCHEMA IF NOT EXISTS shortener_schema;

CREATE TABLE IF NOT EXISTS shortener_schema.urls (
    url TEXT NOT NULL PRIMARY KEY,
    short_uri TEXT UNIQUE,
    created_at TIMESTAMP
);
