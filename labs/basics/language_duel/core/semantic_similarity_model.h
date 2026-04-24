#pragma once

#include <QString>
#include <QStringList>

struct SemanticMatchResult {
    bool accepted = false;
    bool blockedByGuard = false;
    double similarity = 0.0;
    double conceptCoverage = 0.0;
    double precision = 0.0;
    QString confidence;
    QStringList inputConcepts;
    QStringList expectedConcepts;
    QStringList matchedConcepts;
    QStringList missingConcepts;
    QStringList extraConcepts;
    QString explanation;
};

class SemanticSimilarityModel {
public:
    static SemanticMatchResult compare(const QString& userInput, const QString& expected);
    static QString modelVersion();
    static QStringList capabilities();

private:
    static QString normalize(const QString& text);
    static QStringList conceptsForText(const QString& text);
    static QString conceptForToken(const QString& token);
    static bool isStopWord(const QString& token);
};
