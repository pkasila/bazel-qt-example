#ifndef EXERCISEDATA_H
#define EXERCISEDATA_H

#include "exercisestructures.h"
#include <QMap>
#include <QList>
#include <QPair>

class ExerciseData {
public:
    ExerciseData();

    const QList<TranslationTask>& getTranslationTasks(const QString& difficulty) const;
    const QList<GrammarTask>& getGrammarTasks(const QString& difficulty) const;
    int getTimeLimit(const QString& difficulty) const;

    const QStringList& getDifficultyLevels() const;

private:
    QMap<QString, QList<TranslationTask>> translationTasksMap;
    QMap<QString, QList<GrammarTask>> grammarTasksMap;
    QMap<QString, int> timeLimits;
    QStringList difficultyLevels;

    QList<TranslationTask> emptyTranslationTasks;
    QList<GrammarTask> emptyGrammarTasks;

    void initializeData();
};

#endif // EXERCISEDATA_H
