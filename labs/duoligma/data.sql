CREATE TABLE TaskSets (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    name TEXT NOT NULL,
    help TEXT NOT NULL DEFAULT '',
    level INTEGER NOT NULL,
    time_limit INTEGER NOT NULL DEFAULT 0,
    error_limit INTEGER NOT NULL DEFAULT 0
);

CREATE TABLE Tasks (
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    type INTEGER NOT NULL,
    prompt TEXT NOT NULL,
    prompt_audio BLOB NOT NULL DEFAULT X'',
    variants TEXT NOT NULL DEFAULT '',
    correct_variant INTEGER NOT NULL DEFAULT 0,
    correct_resp_normalize BOOLEAN NOT NULL DEFAULT TRUE,
    correct_resp TEXT NOT NULL DEFAULT '',
    task_set_id INTEGER NOT NULL,
    FOREIGN KEY (task_set_id) REFERENCES TaskSet(id) ON DELETE CASCADE
);

INSERT INTO TaskSets (name, level)
VALUES ('German Vocabulary (nouns)', 1);

INSERT INTO Tasks (task_set_id, type, prompt, correct_resp, variants, correct_variant)
VALUES 
    (1, 0, 'Translate "apple"', '', 'Birne;;Apfel;;Kirsche', 1),
    (1, 0, 'Translate "house"', '', 'Gebäude;;Wohnung;;Haus;;Schloss', 2),
    (1, 0, 'Translate "dog"', '', 'Hund;;Katze;;Wolf;;Fuchs', 0),
    (1, 0, 'Translate "cat"', '', 'Hund;;Maus;;Vogel;;Katze', 3),
    (1, 1, 'Translate "water"', 'Wasser', '', 0),
    (1, 1, 'Translate "book"', 'Buch', '', 0),
    (1, 1, 'Translate "friend"', 'Freund', '', 0),
    (1, 1, 'Translate "city"', 'Stadt', '', 0),
    (1, 1, 'Translate "country"', 'Land', '', 0);

INSERT INTO TaskSets (name, level)
VALUES ('German Vocabulary (verbs)', 1);

INSERT INTO Tasks (task_set_id, type, prompt, correct_variant, correct_resp)
VALUES 
    (2, 1, 'Translate "to eat"', 0, 'essen'),
    (2, 1, 'Translate "to drink"', 0, 'trinken'),
    (2, 1, 'Translate "to sleep"', 0, 'schlafen'),
    (2, 1, 'Translate "to work"', 0, 'arbeiten'),
    (2, 1, 'Translate "to learn"', 0, 'lernen'),
    (2, 1, 'Translate "to understand"', 0, 'verstehen'),
    (2, 1, 'Translate "to speak"', 0, 'sprechen'),
    (2, 1, 'Translate "to go"', 0, 'gehen'),
    (2, 1, 'Translate "to see"', 0, 'sehen');

INSERT INTO TaskSets (name, level, time_limit)
VALUES ('Time limit set to 5s', 2, 5);

INSERT INTO Tasks (task_set_id, type, prompt, correct_resp)
VALUES 
    (3, 1, 'Fuck the limits (verbatim)', 'Fuck the limits');

INSERT INTO TaskSets (name, level, error_limit)
VALUES ('Error limit set to 2', 2, 2);

INSERT INTO Tasks (task_set_id, type, prompt, correct_resp)
VALUES 
    (4, 1, 'Nah (verbatim)', 'Nah ha');

INSERT INTO TaskSets (name, level)
VALUES ('Audio', 2);

INSERT INTO Tasks (task_set_id, type, prompt, prompt_audio, correct_resp)
VALUES 
    (5, 2, 'Listen and answer', readfile('labs/duoligma/blob.bin'), 'Bad Apple!!');
