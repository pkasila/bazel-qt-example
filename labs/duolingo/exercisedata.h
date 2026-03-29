#ifndef EXERCISEDATA_H
#define EXERCISEDATA_H

#include <QMap>
#include <QSet>
#include <QString>
#include <QVector>

enum class Difficulty { Easy, Medium, Hard };

struct TranslationTask {
    QString sourceSentence;
    QString correctAnswer;
};

struct GrammarTask {
    QString question;
    QStringList options;
    int correctOptionIndex;
};

struct MultiCorrectGrammarTask {
    QString question;
    QStringList options;
    QSet<int> correctOptionIndices;
};

namespace ExerciseData {
inline QMap<Difficulty, QVector<TranslationTask>> translationTasks;
inline QMap<Difficulty, QVector<GrammarTask>> grammarTasks;
inline QMap<Difficulty, QVector<MultiCorrectGrammarTask>> multiGrammarTasks;

inline void initializeData() {
    translationTasks[Difficulty::Easy] = {
      {.sourceSentence = "Hello", .correctAnswer = "Привет"},
      {.sourceSentence = "Goodbye", .correctAnswer = "Пока"},
      {.sourceSentence = "Thank you", .correctAnswer = "Спасибо"},
      {.sourceSentence = "Yes", .correctAnswer = "Да"},
      {.sourceSentence = "No", .correctAnswer = "Нет"},
      {.sourceSentence = "Please", .correctAnswer = "Пожалуйста"},
      {.sourceSentence = "Good morning", .correctAnswer = "Доброе утро"},
      {.sourceSentence = "Water", .correctAnswer = "Вода"}};

    translationTasks[Difficulty::Medium] = {
      {.sourceSentence = "My name is John.", .correctAnswer = "Меня зовут Джон."},
      {.sourceSentence = "How are you?", .correctAnswer = "Как дела?"},
      {.sourceSentence = "I am learning Russian.", .correctAnswer = "Я учу русский язык."},
      {.sourceSentence = "This is a book.", .correctAnswer = "Это книга."},
      {.sourceSentence = "Where is the library?", .correctAnswer = "Где библиотека?"},
      {.sourceSentence = "What time is it?", .correctAnswer = "Который час?"},
      {.sourceSentence = "I don't understand.", .correctAnswer = "Я не понимаю."}};

    translationTasks[Difficulty::Hard] = {
      {.sourceSentence = "The weather is beautiful today.",
       .correctAnswer = "Сегодня прекрасная погода."},
      {.sourceSentence = "Could you please help me?",
       .correctAnswer = "Не могли бы вы мне помочь?"},
      {.sourceSentence = "I would like to order a coffee.",
       .correctAnswer = "Я хотел бы заказать кофе."},
      {.sourceSentence = "This university has a rich history.",
       .correctAnswer = "У этого университета богатая история."},
      {.sourceSentence = "Practice makes perfect.", .correctAnswer = "Повторение - мать учения."},
      {.sourceSentence = "To kill two birds with one stone.",
       .correctAnswer = "Убить двух зайцев одним выстрелом."},
      {.sourceSentence = "Never judge a book by its cover.",
       .correctAnswer = "Не суди книгу по обложке."}};

    grammarTasks[Difficulty::Easy] = {
      {.question = "I ___ a student. (am/is/are)",
       .options = {"am", "is", "are"},
       .correctOptionIndex = 0},
      {.question = "She ___ reading a book. (am/is/are)",
       .options = {"am", "is", "are"},
       .correctOptionIndex = 1},
      {.question = "They ___ playing. (am/is/are)",
       .options = {"am", "is", "are"},
       .correctOptionIndex = 2},
      {.question = "This ___ an apple. (is/are)",
       .options = {"is", "are"},
       .correctOptionIndex = 0},
      {.question = "These ___ apples. (is/are)", .options = {"is", "are"}, .correctOptionIndex = 1},
      {.question = "He ___ a car. (has/have)",
       .options = {"has", "have"},
       .correctOptionIndex = 0}};

    grammarTasks[Difficulty::Medium] = {
      {.question = "He ___ (go) to school yesterday. (went/goes/gone)",
       .options = {"went", "goes", "gone"},
       .correctOptionIndex = 0},
      {.question = "We ___ (study) English now. (study/are studying/studied)",
       .options = {"study", "are studying", "studied"},
       .correctOptionIndex = 1},
      {.question = "I have ___ (see) this movie. (see/saw/seen)",
       .options = {"see", "saw", "seen"},
       .correctOptionIndex = 2},
      {.question = "The cat sleeps ___ the table. (on/in/under)",
       .options = {"on", "in", "under"},
       .correctOptionIndex = 0},
      {.question = "She is ___ than her brother. (tall/taller/tallest)",
       .options = {"tall", "taller", "tallest"},
       .correctOptionIndex = 1}};

    grammarTasks[Difficulty::Hard] = {
      {.question = "If I ___ (be) you, I would study more. (am/were/was)",
       .options = {"am", "were", "was"},
       .correctOptionIndex = 1},
      {.question = "By the time he arrived, she ___ (leave). (left/had left/leaves)",
       .options = {"left", "had left", "leaves"},
       .correctOptionIndex = 1},
      {.question =
           "This book, ___ I borrowed from the library, is very interesting. (who/which/whom)",
       .options = {"who", "which", "whom"},
       .correctOptionIndex = 1},
      {.question = "Neither John ___ Mary likes classical music. (or/nor/and)",
       .options = {"or", "nor", "and"},
       .correctOptionIndex = 1},
      {.question = "The report must ___ (submit) by tomorrow. (be submitted/submit/submitting)",
       .options = {"be submitted", "submit", "submitting"},
       .correctOptionIndex = 0}};

    multiGrammarTasks[Difficulty::Easy] = {
      {.question = "Which of these are fruits? (Select all that apply)",
       .options = {"Apple", "Carrot", "Banana", "Broccoli"},
       .correctOptionIndices = {0, 2}},
      {.question = "Select the primary colors:",
       .options = {"Red", "Green", "Blue", "Yellow (mixed)"},
       .correctOptionIndices = {0, 1, 2}},
      {.question = "Which words are nouns?",
       .options = {"Quickly", "Table", "Jump", "Happiness"},
       .correctOptionIndices = {1, 3}}};

    multiGrammarTasks[Difficulty::Medium] = {
      {.question = "Select the words that can be adjectives:",
       .options = {"Beauty", "Beautiful", "Beautifully", "Beautify"},
       .correctOptionIndices = {1}},
      {.question = "Which sentences are in the Past Simple tense?",
       .options = {"She eats an apple.", "He walked home.", "They were playing.", "I saw a bird."},
       .correctOptionIndices = {1, 3}},
      {.question = "Select all synonyms for 'happy':",
       .options = {"Joyful", "Sad", "Content", "Angry", "Pleased"},
       .correctOptionIndices = {0, 2, 4}}};

    multiGrammarTasks[Difficulty::Hard] = {
      {.question = "Which of these are renewable energy sources? (Select all that apply)",
       .options = {"Solar", "Coal", "Wind", "Natural Gas", "Geothermal"},
       .correctOptionIndices = {0, 2, 4}},
      {.question = "Select all modal verbs from the list:",
       .options = {"Can", "Must", "Eat", "Should", "Sleep", "Might"},
       .correctOptionIndices = {0, 1, 3, 5}},
      {.question = "Identify all sentences written in the passive voice:",
       .options =
           {"The cake was eaten by the dog.", "The dog ate the cake.",
            "A decision will be made soon.", "She is writing a letter."},
       .correctOptionIndices = {0, 2}}};
}
}  // namespace ExerciseData
#endif