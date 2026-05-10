#include <QString>
#include <algorithm>
#include <vector>

namespace Utils {

int levenshteinDistance(const QString& s1, const QString& s2) {
    int n = s1.length();
    int m = s2.length();

    if (n == 0) {
        return m;
    }
    if (m == 0) {
        return n;
    }

    std::vector<std::vector<int>> d(n + 1, std::vector<int>(m + 1));

    for (int i = 0; i <= n; i++) {
        d[i][0] = i;
    }
    for (int j = 0; j <= m; j++) {
        d[0][j] = j;
    }

    for (int i = 1; i <= n; i++) {
        for (int j = 1; j <= m; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost});
        }
    }
    return d[n][m];
}

bool isCloseEnough(const QString& s1, const QString& s2, float tolerance = 0.2f) {
    if (s1.isEmpty() && s2.isEmpty()) {
        return true;
    }
    int dist = levenshteinDistance(s1.toLower(), s2.toLower());
    int max_len = std::max(s1.length(), s2.length());
    return (static_cast<float>(dist) / max_len) <= tolerance;
}

}  // namespace Utils
