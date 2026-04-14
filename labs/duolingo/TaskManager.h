#ifndef TASKMANAGER_H
#define TASKMANAGER_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <vector>

struct TranslationTask {
    QString original;
    QString translation;
    QString hint;
};

struct GrammarTask {
    QString question;
    QStringList options;
    QString answer;
    QString hint;
};

struct MathTask {
    QString expression;
    int answer;
    QString hint;
};

class TaskManager {
   public:
    bool loadFromFile(const QString& path);
    std::vector<TranslationTask> translationTasks;
    std::vector<GrammarTask> grammarTasks;
    std::vector<MathTask> mathHardTasks;
};

#endif  // TASKMANAGER_H