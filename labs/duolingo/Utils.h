#ifndef UTILS_H
#define UTILS_H

#include <QtCore/QRegularExpression>
#include <QtCore/QString>
#include <algorithm>
#include <vector>

inline int calculateLevenshtein(const QString& s1, const QString& s2) {
    QRegularExpression re("[^\\p{L}\\p{N}\\s]");
    QString str1 = s1.toLower().remove(re).trimmed();
    QString str2 = s2.toLower().remove(re).trimmed();

    str1.replace(QRegularExpression("\\s+"), " ");
    str2.replace(QRegularExpression("\\s+"), " ");

    int n = str1.length();
    int m = str2.length();
    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }

    std::vector<std::vector<int>> d(n + 1, std::vector<int>(m + 1));
    for (int i = 0; i <= n; ++i) {
        d[i][0] = i;
    }
    for (int j = 0; j <= m; ++j) {
        d[0][j] = j;
    }

    for (int i = 1; i <= n; ++i) {
        for (int j = 1; j <= m; ++j) {
            int cost = (str1[i - 1] == str2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost});
        }
    }
    return d[n][m];
}

#endif