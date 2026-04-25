#include "string_utils.h"

#include <QChar>
#include <QVector>
#include <algorithm>

namespace StringUtils {

QString normalizeForComparison(const QString &input) {
    QString decomposed = input.normalized(QString::NormalizationForm_D);
    QString out;
    out.reserve(decomposed.size());

    for (const QChar ch : decomposed) {
        const auto category = ch.category();
        if (category == QChar::Mark_NonSpacing ||
            category == QChar::Mark_SpacingCombining ||
            category == QChar::Mark_Enclosing) {
            continue;
        }

        if (ch.isLetterOrNumber()) {
            out.append(ch.toCaseFolded());
        } else if (ch.isSpace() || ch.isPunct() || ch.isSymbol()) {
            out.append(' ');
        }
    }

    return out.simplified();
}

int levenshteinDistance(const QString &a, const QString &b) {
    const int n = a.size();
    const int m = b.size();
    if (n == 0) return m;
    if (m == 0) return n;

    QVector<int> prev(m + 1);
    QVector<int> curr(m + 1);
    for (int j = 0; j <= m; ++j) prev[j] = j;

    for (int i = 1; i <= n; ++i) {
        curr[0] = i;
        for (int j = 1; j <= m; ++j) {
            const int cost = (a[i - 1] == b[j - 1]) ? 0 : 1;
            curr[j] = std::min({prev[j] + 1, curr[j - 1] + 1, prev[j - 1] + cost});
        }
        std::swap(prev, curr);
    }

    return prev[m];
}

bool fuzzyEquivalent(const QString &a, const QString &b) {
    const QString na = normalizeForComparison(a);
    const QString nb = normalizeForComparison(b);

    if (na == nb) {
        return true;
    }

    const QStringList ta = na.split(' ', Qt::SkipEmptyParts);
    const QStringList tb = nb.split(' ', Qt::SkipEmptyParts);

    if (!ta.isEmpty() && ta.size() == tb.size()) {
        QStringList sa = ta;
        QStringList sb = tb;
        std::sort(sa.begin(), sa.end());
        std::sort(sb.begin(), sb.end());
        if (sa == sb) {
            return true;
        }
    }

    const int dist = levenshteinDistance(na, nb);
    const int maxLen = std::max(na.size(), nb.size());
    if (maxLen == 0) {
        return true;
    }

    const double similarity = 1.0 - (static_cast<double>(dist) / static_cast<double>(maxLen));
    return similarity >= 0.86;
}

} // namespace StringUtils
