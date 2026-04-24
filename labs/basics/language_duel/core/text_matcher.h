#pragma once

#include <QString>

struct MatchResult {
    bool accepted = false;
    bool exactMatch = false;
    bool acceptedByWordOrder = false;
    bool acceptedBySemantic = false;
    int distance = 0;
    double similarity = 0.0;
    double wordOrderSimilarity = 0.0;
    double tokenCoverage = 0.0;
    double semanticSimilarity = 0.0;
    double semanticCoverage = 0.0;
    QString semanticConfidence;
    QString normalizedInput;
    QString normalizedExpected;
    QString feedback;
};

class TextMatcher {
public:
    static MatchResult compare(const QString& userInput, const QString& expected);
    static QString normalize(const QString& text);

private:
    static int levenshteinDistance(const QString& a, const QString& b);
    static QString sortedTokenKey(const QString& text);
    static double tokenCoverage(const QString& userInput, const QString& expected);
};
