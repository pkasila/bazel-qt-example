#include "answerchecker.h"

#include <QRegularExpression>
#include <QVector>
#include <algorithm>

QString AnswerChecker::normalized(QString value) {
    value = value.trimmed().toLower();
    value.replace(QRegularExpression("\\s+"), " ");
    value.remove(QRegularExpression("[.,!?;:]") );
    return value;
}

bool AnswerChecker::isTranslationCorrect(const QString& userAnswer, const QString& expectedAnswer) {
    const QString user = normalized(userAnswer);
    const QString expected = normalized(expectedAnswer);

    if (user.isEmpty()) {
        return false;
    }

    if (user == expected) {
        return true;
    }

    const int distance = levenshteinDistance(user, expected);
    const int allowedMistakes = expected.length() >= 18 ? 3 : (expected.length() >= 8 ? 2 : 1);
    return distance <= allowedMistakes;
}

int AnswerChecker::levenshteinDistance(const QString& left, const QString& right) {
    const int n = left.size();
    const int m = right.size();

    QVector<int> previous(m + 1);
    QVector<int> current(m + 1);

    for (int j = 0; j <= m; ++j) {
        previous[j] = j;
    }

    for (int i = 1; i <= n; ++i) {
        current[0] = i;
        for (int j = 1; j <= m; ++j) {
            const int cost = left[i - 1] == right[j - 1] ? 0 : 1;
            current[j] = std::min({previous[j] + 1, current[j - 1] + 1, previous[j - 1] + cost});
        }
        previous.swap(current);
    }

    return previous[m];
}
