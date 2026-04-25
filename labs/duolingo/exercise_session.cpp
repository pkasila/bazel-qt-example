#include "exercise_session.h"
#include "string_utils.h"

#include <QRandomGenerator>
#include <algorithm>

namespace {

int tasksForLevel(DifficultyLevel level) {
    switch (level) {
    case DifficultyLevel::Beginner: return 8;
    case DifficultyLevel::Intermediate: return 10;
    case DifficultyLevel::Advanced: return 12;
    }
    return 8;
}

int durationForLevel(DifficultyLevel level) {
    switch (level) {
    case DifficultyLevel::Beginner: return 120;
    case DifficultyLevel::Intermediate: return 90;
    case DifficultyLevel::Advanced: return 75;
    }
    return 120;
}

int maxWrongForLevel(DifficultyLevel level) {
    switch (level) {
    case DifficultyLevel::Beginner: return 4;
    case DifficultyLevel::Intermediate: return 3;
    case DifficultyLevel::Advanced: return 3;
    }
    return 4;
}

QVector<int> shuffledIndices(int n) {
    QVector<int> indices;
    indices.reserve(n);
    for (int i = 0; i < n; ++i) {
        indices.push_back(i);
    }
    std::shuffle(indices.begin(), indices.end(), *QRandomGenerator::global());
    return indices;
}

} // namespace

void ExerciseSession::reset() {
    m_tasks.clear();
    m_index = 0;
    m_wrongAttempts = 0;
    m_score = 0;
    m_active = false;
    m_finished = false;
    m_succeeded = false;
    m_timedOut = false;
}

void ExerciseSession::start(ExerciseMode mode, DifficultyLevel level) {
    reset();
    m_config.mode = mode;
    m_config.level = level;
    m_config.taskCount = tasksForLevel(level);
    m_config.durationSeconds = durationForLevel(level);
    m_config.maxWrongAttempts = maxWrongForLevel(level);
    m_config.scorePerTask = 10;

    if (mode == ExerciseMode::Translation) {
        m_tasks = makeTranslationTasks(level, m_config.taskCount);
    } else {
        m_tasks = makeGrammarTasks(level, m_config.taskCount);
    }

    m_active = true;
}

ExerciseSession::SubmitResult ExerciseSession::submitTranslationAnswer(const QString &answer) {
    if (!m_active || m_finished || m_config.mode != ExerciseMode::Translation) {
        return SubmitResult::NotStarted;
    }

    const QString expected = currentTask().answer;
    if (StringUtils::fuzzyEquivalent(answer, expected)) {
        ++m_index;
        if (m_index >= m_tasks.size()) {
            m_score = m_tasks.size() * m_config.scorePerTask;
            m_finished = true;
            m_succeeded = true;
            m_active = false;
            return SubmitResult::Finished;
        }
        return SubmitResult::Correct;
    }

    ++m_wrongAttempts;
    if (m_wrongAttempts >= m_config.maxWrongAttempts) {
        m_finished = true;
        m_active = false;
        m_succeeded = false;
        return SubmitResult::Failed;
    }
    return SubmitResult::Wrong;
}

ExerciseSession::SubmitResult ExerciseSession::submitGrammarAnswer(const QString &answer) {
    if (!m_active || m_finished || m_config.mode != ExerciseMode::Grammar) {
        return SubmitResult::NotStarted;
    }

    const QString expected = currentTask().answer;
    if (StringUtils::normalizeForComparison(answer) == StringUtils::normalizeForComparison(expected)) {
        ++m_index;
        if (m_index >= m_tasks.size()) {
            m_score = m_tasks.size() * m_config.scorePerTask;
            m_finished = true;
            m_succeeded = true;
            m_active = false;
            return SubmitResult::Finished;
        }
        return SubmitResult::Correct;
    }

    ++m_wrongAttempts;
    if (m_wrongAttempts >= m_config.maxWrongAttempts) {
        m_finished = true;
        m_active = false;
        m_succeeded = false;
        return SubmitResult::Failed;
    }
    return SubmitResult::Wrong;
}

void ExerciseSession::markTimedOut() {
    if (!m_active || m_finished) {
        return;
    }
    m_timedOut = true;
    m_finished = true;
    m_active = false;
    m_succeeded = false;
}

QVector<Task> ExerciseSession::makeTranslationTasks(DifficultyLevel level, int count) const {
    QVector<Task> bank;
    if (level == DifficultyLevel::Beginner) {
        bank = {
            {Task::Type::Translation, "Good morning", "Bonjour", {}, "Greeting used before noon."},
            {Task::Type::Translation, "Thank you", "Merci", {}, "Polite expression of gratitude."},
            {Task::Type::Translation, "My name is Anna", "Je m'appelle Anna", {}, "Use the reflexive form in French."},
            {Task::Type::Translation, "I am learning French", "J'apprends le français", {}, "Present tense is enough here."},
            {Task::Type::Translation, "Where is the station?", "Où est la gare ?", {}, "Question word 'where'."},
            {Task::Type::Translation, "I like coffee", "J'aime le café", {}, "Simple present with an article."},
            {Task::Type::Translation, "I have a dog", "J'ai un chien", {}, "Use 'avoir'."},
            {Task::Type::Translation, "We are friends", "Nous sommes amis", {}, "Plural form of 'to be'."},
        };
    } else if (level == DifficultyLevel::Intermediate) {
        bank = {
            {Task::Type::Translation, "We went to the museum yesterday", "Nous sommes allés au musée hier", {}, "Past tense with agreement."},
            {Task::Type::Translation, "She will call you tomorrow", "Elle t'appellera demain", {}, "Future tense."},
            {Task::Type::Translation, "They are studying in the library", "Ils étudient à la bibliothèque", {}, "Watch the preposition."},
            {Task::Type::Translation, "Could you repeat that, please?", "Pouvez-vous répéter, s'il vous plaît ?", {}, "Polite request."},
            {Task::Type::Translation, "I have been here for two hours", "Je suis ici depuis deux heures", {}, "Duration since a point in time."},
            {Task::Type::Translation, "This book is more interesting than that one", "Ce livre est plus intéressant que celui-là", {}, "Comparative form."},
            {Task::Type::Translation, "He forgot his keys at home", "Il a oublié ses clés à la maison", {}, "Past participle."},
            {Task::Type::Translation, "I need to finish my homework before dinner", "Je dois finir mes devoirs avant le dîner", {}, "Necessity + infinitive."},
            {Task::Type::Translation, "The weather became colder in the evening", "Le temps est devenu plus froid dans la soirée", {}, "Comparative adjective."},
            {Task::Type::Translation, "She has already seen that film", "Elle a déjà vu ce film", {}, "Present perfect."},
        };
    } else {
        bank = {
            {Task::Type::Translation, "Although it was raining, we decided to go out", "Bien qu'il pleuvait, nous avons décidé de sortir", {}, "Subordinate clause with concessive meaning."},
            {Task::Type::Translation, "The report will have been finished by Monday", "Le rapport aura été terminé d'ici lundi", {}, "Future perfect passive."},
            {Task::Type::Translation, "Had I known, I would have called you earlier", "Si j'avais su, je t'aurais appelé plus tôt", {}, "Counterfactual conditional."},
            {Task::Type::Translation, "They asked whether the project could be delayed", "Ils ont demandé si le projet pouvait être retardé", {}, "Indirect question."},
            {Task::Type::Translation, "The more you practice, the better you get", "Plus tu pratiques, mieux tu progresses", {}, "Correlative comparative."},
            {Task::Type::Translation, "No matter how difficult it seems, keep going", "Peu importe à quel point cela semble difficile, continue", {}, "Use a natural idiomatic version."},
            {Task::Type::Translation, "She insisted that the documents be signed immediately", "Elle a insisté pour que les documents soient signés immédiatement", {}, "Subjunctive in dependent clause."},
            {Task::Type::Translation, "If the weather had improved, the event would have started outdoors", "Si le temps s'était amélioré, l'événement aurait commencé dehors", {}, "Third conditional."},
            {Task::Type::Translation, "By the time we arrived, the train had already left", "Quand nous sommes arrivés, le train était déjà parti", {}, "Past perfect idea."},
            {Task::Type::Translation, "Her explanation was so clear that everyone understood", "Son explication était si claire que tout le monde a compris", {}, "Result clause."},
            {Task::Type::Translation, "Rarely have we seen such dedication", "Nous avons rarement vu une telle détermination", {}, "Inversion with adverb."},
            {Task::Type::Translation, "Not until the last chapter did I understand the ending", "Ce n'est qu'au dernier chapitre que j'ai compris la fin", {}, "Emphatic inversion."},
        };
    }

    QVector<int> indices = shuffledIndices(bank.size());
    QVector<Task> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        result.push_back(bank[indices[i % indices.size()]]);
    }
    return result;
}

QVector<Task> ExerciseSession::makeGrammarTasks(DifficultyLevel level, int count) const {
    QVector<Task> bank;
    if (level == DifficultyLevel::Beginner) {
        bank = {
            {Task::Type::Grammar, "Choose the correct form: She ___ to school every day.", "goes", {"go", "goes", "going", "gone"}, "Third-person singular present simple."},
            {Task::Type::Grammar, "Choose the correct form: I ___ a sandwich now.", "am eating", {"eat", "am eating", "eats", "ate"}, "Present continuous."},
            {Task::Type::Grammar, "Choose the correct form: They ___ happy.", "are", {"is", "are", "am", "be"}, "Verb to be in plural."},
            {Task::Type::Grammar, "Choose the correct form: He ___ a car last year.", "bought", {"buy", "buys", "bought", "buying"}, "Simple past."},
            {Task::Type::Grammar, "Choose the correct form: We ___ tired after class.", "are", {"is", "are", "was", "be"}, "Verb to be with plural subject."},
            {Task::Type::Grammar, "Choose the correct form: My brother ___ tennis on Saturdays.", "plays", {"play", "plays", "playing", "played"}, "Third-person singular present simple."},
            {Task::Type::Grammar, "Choose the correct form: The cat ___ on the sofa.", "is sleeping", {"sleep", "is sleeping", "sleeps", "slept"}, "Present continuous."},
            {Task::Type::Grammar, "Choose the correct form: They ___ like cold weather.", "don't", {"doesn't", "don't", "isn't", "aren't"}, "Negative with plural subject."},
        };
    } else if (level == DifficultyLevel::Intermediate) {
        bank = {
            {Task::Type::Grammar, "Choose the correct form: If I ___ time, I would travel more.", "had", {"have", "had", "will have", "has"}, "Second conditional."},
            {Task::Type::Grammar, "Choose the correct form: She said she ___ late.", "was", {"is", "was", "will be", "has been"}, "Reported speech."},
            {Task::Type::Grammar, "Choose the correct form: By next week, they ___ the project.", "will have finished", {"finish", "will finish", "will have finished", "finished"}, "Future perfect."},
            {Task::Type::Grammar, "Choose the correct form: We have lived here ___ 2020.", "since", {"for", "since", "during", "from"}, "Point in time."},
            {Task::Type::Grammar, "Choose the correct form: Neither the teacher nor the students ___ ready.", "are", {"is", "are", "was", "be"}, "Nearest subject governs agreement here."},
            {Task::Type::Grammar, "Choose the correct form: I wish I ___ about the meeting earlier.", "had known", {"know", "knew", "had known", "would know"}, "Wish with past perfect."},
            {Task::Type::Grammar, "Choose the correct form: He recommended that I ___ a break.", "take", {"take", "takes", "took", "taken"}, "Mandative subjunctive."},
            {Task::Type::Grammar, "Choose the correct form: The book that I ___ yesterday is on the table.", "bought", {"buy", "buys", "bought", "buying"}, "Past simple in relative clause."},
            {Task::Type::Grammar, "Choose the correct form: Although she was tired, she ___ working.", "kept", {"keeps", "kept", "keep", "keeping"}, "Past simple."},
            {Task::Type::Grammar, "Choose the correct form: The more carefully you listen, the ___ you understand.", "better", {"good", "better", "best", "well"}, "Correlative comparative."},
        };
    } else {
        bank = {
            {Task::Type::Grammar, "Choose the correct form: Hardly ___ the show started when the phone rang.", "had", {"had", "has", "did", "was"}, "Inversion after a negative adverbial."},
            {Task::Type::Grammar, "Choose the correct form: Not only ___ late, but he also forgot the tickets.", "was he", {"he was", "was he", "he is", "is he"}, "Inversion after 'not only'."},
            {Task::Type::Grammar, "Choose the correct form: I suggest that she ___ present at the meeting.", "be", {"is", "was", "be", "been"}, "Subjunctive form."},
            {Task::Type::Grammar, "Choose the correct form: Had we known about the delay, we ___ earlier.", "would have left", {"left", "would leave", "would have left", "had left"}, "Third conditional inversion."},
            {Task::Type::Grammar, "Choose the correct form: The data ___ showing a steady increase.", "are", {"is", "are", "was", "has"}, "Plural noun treated as plural."},
            {Task::Type::Grammar, "Choose the correct form: No sooner ___ home than it began to snow.", "had I reached", {"I reached", "had I reached", "did I reach", "I had reached"}, "Inversion with 'no sooner'."},
            {Task::Type::Grammar, "Choose the correct form: Seldom ___ such precision in a beginner's essay.", "do we see", {"we see", "do we see", "we saw", "have we saw"}, "Inversion with 'seldom'."},
            {Task::Type::Grammar, "Choose the correct form: Little ___ about the plan before the briefing.", "did I know", {"I knew", "did I know", "I know", "have I known"}, "Negative adverbial inversion."},
            {Task::Type::Grammar, "Choose the correct form: Were I in your position, I ___ the offer.", "would accept", {"would accept", "accept", "accepted", "will accept"}, "Formal conditional inversion."},
            {Task::Type::Grammar, "Choose the correct form: Only after the meeting ___ the issue.", "did we discuss", {"we discussed", "did we discuss", "we do discuss", "have we discussed"}, "Inversion with 'only after'."},
            {Task::Type::Grammar, "Choose the correct form: Never ___ such a beautiful voice.", "have I heard", {"I heard", "have I heard", "I have heard", "did I hear"}, "Inversion with 'never'."},
            {Task::Type::Grammar, "Choose the correct form: So intense ___ that the windows shook.", "was the storm", {"the storm was", "was the storm", "the storm is", "is the storm"}, "Inversion with 'so'."},
        };
    }

    QVector<int> indices = shuffledIndices(bank.size());
    QVector<Task> result;
    result.reserve(count);
    for (int i = 0; i < count; ++i) {
        result.push_back(bank[indices[i % indices.size()]]);
    }
    return result;
}
