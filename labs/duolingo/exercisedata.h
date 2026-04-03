#ifndef EXERCISEDATA_H
#define EXERCISEDATA_H

#include <QString>
#include <QVector>
#include <QStringList>


enum class Difficulty { Easy, Medium, Hard };

enum class ExerciseType { None, Translation, Grammar };

struct TranslationQuestion {
    QString original_sentence;
    QString correct_answer;
    QString help_text;
};

struct GrammarQuestion {
    QString sentence_p1;
    QString sentence_p2;
    QStringList options;
    int correct_option_index;
    QString help_text;
};

#endif // EXERCISEDATA_H
