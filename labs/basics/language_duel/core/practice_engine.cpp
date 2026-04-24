#include "core/practice_engine.h"

#include <algorithm>
#include <random>

#include <QRandomGenerator>

namespace {
template <typename Task>
std::vector<Task> pickRandomTasks(std::vector<Task> tasks, int count) {
    std::mt19937 rng(QRandomGenerator::global()->generate());
    std::shuffle(tasks.begin(), tasks.end(), rng);

    if (count < static_cast<int>(tasks.size())) {
        tasks.resize(count);
    }

    return tasks;
}
}  // namespace

PracticeEngine::PracticeEngine() = default;

void PracticeEngine::setDifficulty(Difficulty difficulty) {
    difficulty_ = difficulty;
}

Difficulty PracticeEngine::difficulty() const {
    return difficulty_;
}

QString PracticeEngine::difficultyToString(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Easy:
            return "Easy";
        case Difficulty::Medium:
            return "Medium";
        case Difficulty::Hard:
            return "Hard";
    }
    return "Unknown";
}

SessionConfig PracticeEngine::makeConfig(ExerciseMode mode) const {
    SessionConfig config;
    config.difficulty = difficulty_;
    config.mode = mode;

    switch (difficulty_) {
        case Difficulty::Easy:
            config.taskCount = 5;
            config.timeLimitSeconds = 90;
            config.maxMistakes = 2;
            break;
        case Difficulty::Medium:
            config.taskCount = 6;
            config.timeLimitSeconds = 75;
            config.maxMistakes = 2;
            break;
        case Difficulty::Hard:
            config.taskCount = 7;
            config.timeLimitSeconds = 60;
            config.maxMistakes = 1;
            break;
    }

    return config;
}

SessionData PracticeEngine::createSession(ExerciseMode mode) const {
    SessionData data;
    data.config = makeConfig(mode);

    if (mode == ExerciseMode::Translation) {
        std::vector<TranslationTask> easy = {
            {"Translate: Good morning", "Доброе утро", "Use a common polite greeting."},
            {"Translate: I read books every day", "Я читаю книги каждый день", "Present simple often maps to a habitual action."},
            {"Translate: The cat is under the table", "Кот под столом", "Pay attention to place prepositions."},
            {"Translate: We are learning English", "Мы учим английский", "Continuous idea can be translated naturally."},
            {"Translate: She likes green tea", "Ей нравится зеленый чай", "Remember natural word order."},
            {"Translate: My friend lives in Minsk", "Мой друг живет в Минске", "City name should stay capitalized."},
            {"Translate: They arrived very late", "Они приехали очень поздно", "An adverb usually goes near the verb or after it."}
        };

        std::vector<TranslationTask> medium = {
            {"Translate: If it rains tomorrow, we will stay home", "Если завтра пойдет дождь, мы останемся дома", "It is a real future condition."},
            {"Translate: I have never seen this movie before", "Я никогда раньше не видел этот фильм", "Use a natural translation of present perfect."},
            {"Translate: She was cooking when I called her", "Она готовила, когда я ей позвонил", "Two past actions with one in progress."},
            {"Translate: Could you open the window, please?", "Не мог бы ты открыть окно, пожалуйста", "Polite request, not strict command."},
            {"Translate: The lesson was more difficult than I expected", "Урок был сложнее, чем я ожидал", "Comparative construction matters."},
            {"Translate: We should finish the project this week", "Мы должны закончить проект на этой неделе", "Modal meaning: obligation or plan."},
            {"Translate: He forgot to send the email yesterday", "Он забыл отправить письмо вчера", "Infinitive after forgot."},
            {"Translate: I am proud of your progress", "Я горжусь твоим прогрессом", "Use the correct preposition in Russian."}
        };

        std::vector<TranslationTask> hard = {
            {"Translate: Although the task looked simple, it required careful reasoning", "Хотя задача выглядела простой, она требовала тщательного рассуждения", "Contrast clause + formal wording."},
            {"Translate: By the time we arrived, the lecture had already started", "К тому времени как мы пришли, лекция уже началась", "Past perfect idea."},
            {"Translate: I would rather revise the theory before solving the exercises", "Я бы предпочел повторить теорию перед решением упражнений", "Would rather = preference."},
            {"Translate: The interface remains intuitive even when the window is resized", "Интерфейс остается интуитивно понятным даже при изменении размера окна", "Good sentence for UI vocabulary."},
            {"Translate: Her explanation was concise, yet surprisingly insightful", "Ее объяснение было кратким, но удивительно содержательным", "Watch conjunction nuance."},
            {"Translate: Unless you practice regularly, progress will be slow", "Если ты не будешь регулярно практиковаться, прогресс будет медленным", "Unless = if not."},
            {"Translate: This approach is robust enough to handle minor typing mistakes", "Этот подход достаточно надежен, чтобы обрабатывать небольшие опечатки", "Infinitive of purpose."},
            {"Translate: The final result exceeded all expectations", "Итоговый результат превзошел все ожидания", "Formal but compact phrase."},
            {"Translate: He insisted that every detail should be checked twice", "Он настаивал на том, что каждую деталь нужно проверить дважды", "Insisted that + should."}
        };

        switch (difficulty_) {
            case Difficulty::Easy:
                data.translationTasks = pickRandomTasks(easy, data.config.taskCount);
                break;
            case Difficulty::Medium:
                data.translationTasks = pickRandomTasks(medium, data.config.taskCount);
                break;
            case Difficulty::Hard:
                data.translationTasks = pickRandomTasks(hard, data.config.taskCount);
                break;
        }
    } else {
        std::vector<GrammarTask> easy = {
            {"Choose the correct form: She ___ to school every day.", {"go", "goes", "going"}, 1, "Third person singular in Present Simple usually needs -s."},
            {"Choose the correct form: We ___ dinner now.", {"have", "are having", "had"}, 1, "Look for the signal word 'now'."},
            {"Choose the correct form: They ___ in the park yesterday.", {"walk", "walked", "walking"}, 1, "Yesterday usually signals Past Simple."},
            {"Choose the correct form: I ___ a student.", {"am", "is", "are"}, 0, "Match the verb 'to be' with the subject."},
            {"Choose the correct form: There ___ two books on the table.", {"is", "are", "was"}, 1, "Plural noun needs plural verb."},
            {"Choose the correct form: He can ___ very fast.", {"runs", "run", "ran"}, 1, "After modal verbs use the base form."}
        };

        std::vector<GrammarTask> medium = {
            {"Choose the correct form: If I ___ more time, I would travel more.", {"have", "had", "will have"}, 1, "Second conditional uses Past Simple in the if-clause."},
            {"Choose the correct form: By next year, she ___ her degree.", {"finishes", "will finish", "will have finished"}, 2, "Future perfect points to completion before a future moment."},
            {"Choose the correct form: The book ___ by my older brother.", {"was written", "wrote", "has writing"}, 0, "Passive voice is needed."},
            {"Choose the correct form: I prefer tea ___ coffee.", {"than", "to", "from"}, 1, "Fixed collocation."},
            {"Choose the correct form: He asked me where I ___ from.", {"am", "was", "were"}, 1, "Backshift in reported speech."},
            {"Choose the correct form: This is the most interesting lecture I have ever ___.", {"see", "saw", "seen"}, 2, "Present perfect requires the past participle."},
            {"Choose the correct form: Neither of the answers ___ correct.", {"are", "is", "be"}, 1, "Neither is grammatically singular here."}
        };

        std::vector<GrammarTask> hard = {
            {"Choose the correct form: Hardly ___ the meeting started when the fire alarm rang.", {"had", "has", "did"}, 0, "After 'hardly' with inversion use Past Perfect auxiliary."},
            {"Choose the correct form: She acts as if she ___ everything already.", {"knows", "knew", "had known"}, 1, "Unreal comparison often uses past form."},
            {"Choose the correct form: No sooner ___ home than he remembered the forgotten notes.", {"he reached", "had he reached", "he had reached"}, 1, "Negative adverbial at the beginning triggers inversion."},
            {"Choose the correct form: It is essential that every student ___ on time.", {"arrives", "arrive", "arrived"}, 1, "Mandative subjunctive uses base form."},
            {"Choose the correct form: Not only ___ elegant, but it is also maintainable.", {"the solution is", "is the solution", "the solution"}, 1, "Inversion after 'not only' at sentence start."},
            {"Choose the correct form: Had I known earlier, I ___ the design.", {"changed", "would change", "would have changed"}, 2, "Third conditional."},
            {"Choose the correct form: The data, together with the summary, ___ ready.", {"are", "is", "were"}, 1, "The core subject is singular."},
            {"Choose the correct form: Scarcely had we sat down when the speaker ___.", {"arrive", "arrived", "was arriving"}, 1, "Past Simple in the second clause."}
        };

        switch (difficulty_) {
            case Difficulty::Easy:
                data.grammarTasks = pickRandomTasks(easy, data.config.taskCount);
                break;
            case Difficulty::Medium:
                data.grammarTasks = pickRandomTasks(medium, data.config.taskCount);
                break;
            case Difficulty::Hard:
                data.grammarTasks = pickRandomTasks(hard, data.config.taskCount);
                break;
        }
    }

    return data;
}
