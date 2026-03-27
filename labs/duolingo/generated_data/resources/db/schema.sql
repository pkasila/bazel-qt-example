-- Enable foreign key support for SQLite if not enabled by PRAGMA in C++
PRAGMA foreign_keys = ON;

CREATE TABLE IF NOT EXISTS Languages (
    language_id INTEGER PRIMARY KEY AUTOINCREMENT,
    language_code TEXT UNIQUE NOT NULL, -- 'en', 'fr', 'de'
    language_name TEXT NOT NULL       -- 'English', 'French', 'German'
);

CREATE TABLE IF NOT EXISTS DifficultyLevels (
    difficulty_id INTEGER PRIMARY KEY AUTOINCREMENT, -- 1:Easy, 2:Medium, 3:Hard
    level_name TEXT UNIQUE NOT NULL,        -- 'Easy', 'Medium', 'Hard'
    points_base INTEGER NOT NULL DEFAULT 1, -- Base points for a correct task at this level
    error_limit INTEGER NOT NULL DEFAULT 3, -- Max errors allowed for a session at this level
    session_duration_seconds INTEGER NOT NULL DEFAULT 180 -- Default time for N tasks
);

CREATE TABLE IF NOT EXISTS ExerciseTypes (
    type_id INTEGER PRIMARY KEY AUTOINCREMENT, -- 1:Translation, 2:Grammar
    type_name TEXT UNIQUE NOT NULL -- 'Translation', 'Grammar'
);

CREATE TABLE IF NOT EXISTS Exercises (
    exercise_id INTEGER PRIMARY KEY AUTOINCREMENT,
    type_id INTEGER NOT NULL,
    language_id INTEGER NOT NULL,
    difficulty_id INTEGER NOT NULL, -- Refers to DifficultyLevels.difficulty_id
    original_text TEXT NOT NULL,
    correct_answer_text TEXT, 
    hint_text TEXT,
    additional_data_json TEXT, -- For grammar options: {"options": ["opt1", "opt2", "opt3"]}
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    FOREIGN KEY (type_id) REFERENCES ExerciseTypes(type_id),
    FOREIGN KEY (language_id) REFERENCES Languages(language_id),
    FOREIGN KEY (difficulty_id) REFERENCES DifficultyLevels(difficulty_id)
);

CREATE TABLE IF NOT EXISTS Users (
    user_id INTEGER PRIMARY KEY AUTOINCREMENT,
    username TEXT UNIQUE NOT NULL DEFAULT 'default_user', -- For now, only one user
    created_at TIMESTAMP DEFAULT CURRENT_TIMESTAMP
);

CREATE TABLE IF NOT EXISTS UserExerciseSessions (
    session_id INTEGER PRIMARY KEY AUTOINCREMENT,
    user_id INTEGER NOT NULL,
    difficulty_id INTEGER NOT NULL, -- Refers to DifficultyLevels.difficulty_id
    session_start_time TIMESTAMP NOT NULL,
    session_end_time TIMESTAMP,
    total_tasks_in_session INTEGER NOT NULL, -- The N value for this session
    tasks_correctly_completed INTEGER DEFAULT 0,
    errors_made INTEGER DEFAULT 0,
    final_score INTEGER DEFAULT 0,
    was_successful BOOLEAN DEFAULT FALSE, -- True if all N tasks done, within M errors, and within time
    time_spent_seconds INTEGER,
    FOREIGN KEY (user_id) REFERENCES Users(user_id),
    FOREIGN KEY (difficulty_id) REFERENCES DifficultyLevels(difficulty_id)
);

CREATE TABLE IF NOT EXISTS UserSolvedExercises ( -- To avoid repeating exercises for a user
    user_id INTEGER NOT NULL,
    exercise_id INTEGER NOT NULL,
    solved_timestamp TIMESTAMP DEFAULT CURRENT_TIMESTAMP,
    PRIMARY KEY (user_id, exercise_id),
    FOREIGN KEY (user_id) REFERENCES Users(user_id),
    FOREIGN KEY (exercise_id) REFERENCES Exercises(exercise_id) ON DELETE CASCADE
);

-- Initial Data
INSERT OR IGNORE INTO Languages (language_id, language_code, language_name) VALUES (1, 'en', 'English');
INSERT OR IGNORE INTO Languages (language_id, language_code, language_name) VALUES (2, 'fr', 'French');
INSERT OR IGNORE INTO Languages (language_id, language_code, language_name) VALUES (3, 'de', 'German');

INSERT OR IGNORE INTO DifficultyLevels (difficulty_id, level_name, points_base, error_limit, session_duration_seconds) VALUES (1, 'Easy', 1, 5, 300);
INSERT OR IGNORE INTO DifficultyLevels (difficulty_id, level_name, points_base, error_limit, session_duration_seconds) VALUES (2, 'Medium', 2, 3, 180);
INSERT OR IGNORE INTO DifficultyLevels (difficulty_id, level_name, points_base, error_limit, session_duration_seconds) VALUES (3, 'Hard', 3, 2, 120);

INSERT OR IGNORE INTO ExerciseTypes (type_id, type_name) VALUES (1, 'Translation');
INSERT OR IGNORE INTO ExerciseTypes (type_id, type_name) VALUES (2, 'Grammar');

INSERT OR IGNORE INTO Users (user_id, username) VALUES (0, 'default_user');

-- Example Exercises (English, Easy - Translation)
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Hello', 'Привет', 'A common greeting.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Goodbye', 'Пока', 'Said when leaving.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Thank you', 'Спасибо', 'Expressing gratitude.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Please', 'Пожалуйста', 'A polite request.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Yes', 'Да', 'Affirmative answer.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'No', 'Нет', 'Negative answer.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'My name is...', 'Меня зовут...', 'Introducing yourself.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'How are you?', 'Как дела?', 'Asking about well-being.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Good morning', 'Доброе утро', 'Morning greeting.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 1, 'Water', 'Вода', 'A vital liquid.');

-- Example Exercises (English, Easy - Grammar)
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 1, 'I ___ a student.', 'am', 'Verb "to be", present tense, first person.', '{"options": ["is", "am", "are", "be"]}');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 1, 'She ___ a doctor.', 'is', 'Verb "to be", present tense, third person singular.', '{"options": ["is", "am", "are", "be"]}');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 1, 'They ___ happy.', 'are', 'Verb "to be", present tense, plural.', '{"options": ["is", "am", "are", "be"]}');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 1, 'This book is ___ .', 'mine', 'Possessive pronoun.', '{"options": ["my", "mine", "me", "I"]}');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 1, 'He ___ to the park yesterday.', 'went', 'Past tense of "go".', '{"options": ["go", "goes", "went", "gone"]}');

-- Example Exercises (English, Medium - Translation)
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 2, 'The weather is beautiful today.', 'Сегодня прекрасная погода.', 'Describing current weather conditions.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 2, 'I would like to order a coffee.', 'Я хотел бы заказать кофе.', 'Making a request at a cafe.');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text) VALUES (1, 1, 2, 'Can you help me with this task?', 'Вы можете помочь мне с этой задачей?', 'Asking for assistance.');

-- Example Exercises (English, Medium - Grammar)
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 2, 'If I ___ you, I would study more.', 'were', 'Subjunctive mood for hypothetical situations.', '{"options": ["was", "were", "am", "be"]}');
INSERT OR IGNORE INTO Exercises (type_id, language_id, difficulty_id, original_text, correct_answer_text, hint_text, additional_data_json) VALUES (2, 1, 2, 'This is the house ___ I was born.', 'where', 'Relative pronoun for place.', '{"options": ["which", "who", "where", "when"]}');