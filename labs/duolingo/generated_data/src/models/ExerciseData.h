#ifndef EXERCISEDATA_H
#define EXERCISEDATA_H

#include <QString>
#include <QStringList>
#include <QVariantMap>

enum class ExerciseType {
    Translation,
    Grammar
    // Add more types if needed
};

struct ExerciseData {
    int id = -1;
    ExerciseType type;
    QString languageCode; // e.g., "en", "fr"
    int difficultyId = -1;

    QString originalText;       // Sentence to translate, or sentence with a blank
    QString correctAnswerText;  // Correct translation or word for the blank
    QStringList grammarOptions; // Options for grammar exercises
    QString hintText;

    QVariantMap additionalData; // For any other specific data

    ExerciseData() = default;
};

#endif // EXERCISEDATA_H