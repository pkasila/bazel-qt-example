CREATE TABLE IF NOT EXISTS translation_questions_easy
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    english_word TEXT NOT NULL,
    german_word  TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS translation_questions_medium
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    english_word TEXT NOT NULL,
    german_word  TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS translation_questions_hard
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    english_word TEXT NOT NULL,
    german_word  TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS user
(
    user_id    INTEGER PRIMARY KEY,
    experience INTEGER NOT NULL DEFAULT 0,
    level      INTEGER NOT NULL DEFAULT 1
);

INSERT INTO user DEFAULT
VALUES;

CREATE TABLE IF NOT EXISTS unanswered_translation_easy_questions
(
    question_id INTEGER PRIMARY KEY
);

INSERT INTO unanswered_translation_easy_questions (question_id)
WITH RECURSIVE cnt(x) AS (
    SELECT 1
    UNION ALL
    SELECT x + 1 FROM cnt WHERE x < 243
)
SELECT x FROM cnt;

CREATE TABLE IF NOT EXISTS unanswered_translation_medium_questions
(
    question_id INTEGER PRIMARY KEY
);

INSERT INTO unanswered_translation_medium_questions (question_id)
WITH RECURSIVE cnt(x) AS (
    SELECT 1
    UNION ALL
    SELECT x + 1 FROM cnt WHERE x < 317
)
SELECT x FROM cnt;

CREATE TABLE IF NOT EXISTS unanswered_translation_hard_questions
(
    question_id INTEGER PRIMARY KEY
);

INSERT INTO unanswered_translation_hard_questions (question_id)
WITH RECURSIVE cnt(x) AS (
    SELECT 1
    UNION ALL
    SELECT x + 1 FROM cnt WHERE x < 172
)
SELECT x FROM cnt;

CREATE TABLE IF NOT EXISTS grammar_questions_easy
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    task TEXT NOT NULL,
    options1  TEXT NOT NULL,
    options2  TEXT NOT NULL,
    options3  TEXT NOT NULL,
    options4  TEXT NOT NULL,
    answer INTEGER NOT NULL,
    rule TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS grammar_questions_medium
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    task TEXT NOT NULL,
    options1  TEXT NOT NULL,
    options2  TEXT NOT NULL,
    options3  TEXT NOT NULL,
    options4  TEXT NOT NULL,
    answer INTEGER NOT NULL,
    rule TEXT NOT NULL
);


CREATE TABLE IF NOT EXISTS grammar_questions_hard
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    task TEXT NOT NULL,
    options1  TEXT NOT NULL,
    options2  TEXT NOT NULL,
    options3  TEXT NOT NULL,
    options4  TEXT NOT NULL,
    answer INTEGER NOT NULL,
    rule TEXT NOT NULL
);

CREATE TABLE IF NOT EXISTS unanswered_grammar_easy_questions
(
    question_id INTEGER PRIMARY KEY
);

INSERT INTO unanswered_grammar_easy_questions (question_id)
WITH RECURSIVE cnt(x) AS (
    SELECT 1
    UNION ALL
    SELECT x + 1 FROM cnt WHERE x < 150
)
SELECT x FROM cnt;

CREATE TABLE IF NOT EXISTS unanswered_grammar_medium_questions
(
    question_id INTEGER PRIMARY KEY
);

INSERT INTO unanswered_grammar_medium_questions (question_id)
WITH RECURSIVE cnt(x) AS (
    SELECT 1
    UNION ALL
    SELECT x + 1 FROM cnt WHERE x < 100
)
SELECT x FROM cnt;

CREATE TABLE IF NOT EXISTS unanswered_grammar_hard_questions
(
    question_id INTEGER PRIMARY KEY
);

INSERT INTO unanswered_grammar_hard_questions (question_id)
WITH RECURSIVE cnt(x) AS (
    SELECT 1
    UNION ALL
    SELECT x + 1 FROM cnt WHERE x < 90
)
SELECT x FROM cnt;

CREATE TABLE IF NOT EXISTS rules
(
    id           INTEGER PRIMARY KEY AUTOINCREMENT,
    topic TEXT NOT NULL,
    rule TEXT NOT NULL
)