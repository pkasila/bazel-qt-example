#include "exercise_data.h"

#include <random>

namespace ExerciseGenerator {

Question makeTranslation(
    const QString& prompt, const QString& answer, const QString& hint, bool reverse) {
    return {prompt, answer, QStringList(), -1, hint, false, reverse};
}

Question makeGrammar(
    const QString& prompt, const QStringList& opts, int correctIdx, const QString& hint) {
    return {prompt, QString(), opts, correctIdx, hint, true, false};
}

QList<Question> generate(int count, bool isGrammar) {
    QList<Question> questions;
    if (!isGrammar) {
        static const QList<QPair<QString, QString>> vocab = {
          {"Кот", "Cat"},    {"Книга", "Book"},  {"Вода", "Water"},
          {"Солнце", "Sun"}, {"Друг", "Friend"}, {"Дерево", "Tree"}};
        static const QList<QString> hints = {
          "Hint: Domestic animal that says meow",
          "Hint: Object for reading",
          "Hint: Transparent drinkable liquid",
          "Hint: Star that gives light during day",
          "Hint: Person close to you",
          "Hint: Plant with trunk and leaves"};

        std::random_device rd;
        std::mt19937 gen(rd());
        std::uniform_int_distribution<> dirDist(0, 1);

        for (int i = 0; i < count; ++i) {
            int idx = i % vocab.size();
            bool reverse = dirDist(gen) == 1;
            if (reverse) {
                questions << makeTranslation(vocab[idx].second, vocab[idx].first, hints[idx], true);
            } else {
                questions << makeTranslation(
                    vocab[idx].first, vocab[idx].second, hints[idx], false);
            }
        }
    } else {
        static const QList<Question> pool = {
          makeGrammar(
              "I ___ a student.", QStringList() << "am" << "is" << "are", 0,
              "Hint: you don`t need it"),
          makeGrammar(
              "She ___ happy.", QStringList() << "am" << "is" << "are", 1,
              "Hint: you don`t need it"),
          makeGrammar(
              "They ___ playing.", QStringList() << "am" << "is" << "are", 2,
              "Hint: you don`t need it"),
          makeGrammar(
              "We ___ friends.", QStringList() << "am" << "is" << "are", 2,
              "Hint: you don`t need it"),
          makeGrammar(
              "It ___ cold.", QStringList() << "am" << "is" << "are", 1, "Hint: you don`t need it"),
          makeGrammar(
              "You ___ late.", QStringList() << "am" << "is" << "are", 2,
              "Hint: you don`t need it")};
        for (int i = 0; i < count; ++i) {
            questions << pool[i % pool.size()];
        }
    }
    return questions;
}
}  // namespace ExerciseGenerator