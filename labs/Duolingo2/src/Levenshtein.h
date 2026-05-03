#pragma once
#include <QString>
#include <vector>
#include <algorithm>
#include <QRegularExpression>

class StringComparator {
public:
    // Алгоритм Дамерау-Левенштейна (учитывает перестановку соседних букв как 1 ошибку)
    static bool isFuzzyMatch(const QString& input, const QString& expected, int maxTypos = 2) {
        QString s1 = cleanString(input);
        QString s2 = cleanString(expected);

        int len1 = s1.length();
        int len2 = s2.length();
        std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

        for (int i = 0; i <= len1; ++i) d[i][0] = i;
        for (int j = 0; j <= len2; ++j) d[0][j] = j;

        for (int i = 1; i <= len1; ++i) {
            for (int j = 1; j <= len2; ++j) {
                int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
                d[i][j] = std::min({
                    d[i - 1][j] + 1,       // Удаление
                    d[i][j - 1] + 1,       // Вставка
                    d[i - 1][j - 1] + cost // Замена
                });
                
                // Транспозиция (перестановка)
                if (i > 1 && j > 1 && s1[i - 1] == s2[j - 2] && s1[i - 2] == s2[j - 1]) {
                    d[i][j] = std::min(d[i][j], d[i - 2][j - 2] + cost);
                }
            }
        }
        
        // Для длинных слов прощаем больше ошибок (до 20% от длины)
        int allowedTypos = std::max(maxTypos, len2 / 5);
        return d[len1][len2] <= allowedTypos;
    }

private:
    static QString cleanString(const QString& str) {
        QString res = str.toLower().trimmed();
        res.remove(QRegularExpression("[.,!?\"'-]")); // Игнорируем любую пунктуацию
        return res;
    }
};
