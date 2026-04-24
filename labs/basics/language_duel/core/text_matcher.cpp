#include "core/text_matcher.h"

#include <algorithm>
#include <QChar>
#include <QSet>
#include <QStringList>
#include <vector>

#include "core/semantic_similarity_model.h"

QString TextMatcher::normalize(const QString& text) {
    QString normalized = text.normalized(QString::NormalizationForm_D);
    QString stripped;
    stripped.reserve(normalized.size());

    for (const QChar ch : normalized) {
        if (ch.category() == QChar::Mark_NonSpacing ||
            ch.category() == QChar::Mark_SpacingCombining ||
            ch.category() == QChar::Mark_Enclosing) {
            continue;
        }

        if (ch.isLetterOrNumber() || ch.isSpace()) {
            stripped.push_back(ch.toLower());
            continue;
        }

        if (QString("'-").contains(ch)) {
            stripped.push_back(' ');
        }
    }

    stripped.replace(QChar(0x0451), QChar(0x0435));
    stripped.replace(QChar(0x0401), QChar(0x0435));

    return stripped.simplified();
}

int TextMatcher::levenshteinDistance(const QString& a, const QString& b) {
    const int n = a.size();
    const int m = b.size();

    std::vector<int> prev(m + 1);
    std::vector<int> cur(m + 1);

    for (int j = 0; j <= m; ++j) {
        prev[j] = j;
    }

    for (int i = 1; i <= n; ++i) {
        cur[0] = i;
        for (int j = 1; j <= m; ++j) {
            const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            cur[j] = std::min({
                prev[j] + 1,
                cur[j - 1] + 1,
                prev[j - 1] + cost
            });
        }
        prev.swap(cur);
    }

    return prev[m];
}

QString TextMatcher::sortedTokenKey(const QString& text) {
    QStringList tokens = text.split(' ', Qt::SkipEmptyParts);
    tokens.removeAll("a");
    tokens.removeAll("an");
    tokens.removeAll("the");
    std::sort(tokens.begin(), tokens.end());
    return tokens.join(' ');
}

double TextMatcher::tokenCoverage(const QString& userInput, const QString& expected) {
    const QStringList userTokens = userInput.split(' ', Qt::SkipEmptyParts);
    const QStringList expectedTokens = expected.split(' ', Qt::SkipEmptyParts);
    if (expectedTokens.isEmpty()) {
        return userTokens.isEmpty() ? 1.0 : 0.0;
    }

    QSet<QString> userSet;
    for (const QString& token : userTokens) {
        userSet.insert(token);
    }

    int covered = 0;
    for (const QString& token : expectedTokens) {
        if (userSet.contains(token)) {
            ++covered;
        }
    }

    return static_cast<double>(covered) / static_cast<double>(expectedTokens.size());
}

MatchResult TextMatcher::compare(const QString& userInput, const QString& expected) {
    MatchResult result;
    result.normalizedInput = normalize(userInput);
    result.normalizedExpected = normalize(expected);
    result.distance = levenshteinDistance(result.normalizedInput, result.normalizedExpected);

    const int maxLen = std::max(result.normalizedInput.size(), result.normalizedExpected.size());
    result.similarity = (maxLen == 0) ? 1.0 : 1.0 - static_cast<double>(result.distance) / static_cast<double>(maxLen);

    if (result.normalizedInput == result.normalizedExpected) {
        result.accepted = true;
        result.exactMatch = true;
        result.wordOrderSimilarity = 1.0;
        result.tokenCoverage = 1.0;
        result.semanticSimilarity = 1.0;
        result.semanticCoverage = 1.0;
        result.semanticConfidence = "high";
        result.feedback = "Exact match after normalization.";
        return result;
    }

    const QString sortedInput = sortedTokenKey(result.normalizedInput);
    const QString sortedExpected = sortedTokenKey(result.normalizedExpected);
    const int sortedDistance = levenshteinDistance(sortedInput, sortedExpected);
    const int sortedMaxLen = std::max(sortedInput.size(), sortedExpected.size());
    result.wordOrderSimilarity =
        (sortedMaxLen == 0) ? 1.0 : 1.0 - static_cast<double>(sortedDistance) / static_cast<double>(sortedMaxLen);
    result.tokenCoverage = tokenCoverage(result.normalizedInput, result.normalizedExpected);

    if (maxLen <= 10) {
        result.accepted = result.distance <= 1;
    } else if (maxLen <= 25) {
        result.accepted = result.distance <= 2;
    } else {
        result.accepted = result.similarity >= 0.88;
    }

    if (!result.accepted &&
        result.tokenCoverage >= 0.82 &&
        result.wordOrderSimilarity >= 0.86 &&
        result.similarity >= 0.68) {
        result.accepted = true;
        result.acceptedByWordOrder = true;
    }

    const SemanticMatchResult semantic = SemanticSimilarityModel::compare(userInput, expected);
    result.semanticSimilarity = semantic.similarity;
    result.semanticCoverage = semantic.conceptCoverage;
    result.semanticConfidence = semantic.confidence;
    if (!result.accepted && semantic.accepted) {
        result.accepted = true;
        result.acceptedBySemantic = true;
    }

    if (result.acceptedBySemantic) {
        result.feedback = "Accepted semantically: " + semantic.explanation + ".";
    } else if (result.acceptedByWordOrder) {
        result.feedback = "Accepted: the words match well enough even with a different word order.";
    } else if (result.accepted) {
        result.feedback = "Accepted: minor typos or punctuation differences were ignored.";
    } else {
        result.feedback = "Not accepted: too many important words differ. " + semantic.explanation + ".";
    }

    return result;
}
