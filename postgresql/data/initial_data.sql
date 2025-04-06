INSERT INTO shortener_schema.users(url, short_uri, created_at)
VALUES ('TODO', 'TODO', 'TODO')
ON CONFLICT (url)
DO NOTHING;
