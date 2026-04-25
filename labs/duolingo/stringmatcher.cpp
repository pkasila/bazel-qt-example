#include "stringmatcher.h"
#include <algorithm>
#include <QMap>

int StringMatcher::levenshteinDistance(const QString& s1, const QString& s2) {
    int len1 = s1.length();
    int len2 = s2.length();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1].toLower() == s2[j - 1].toLower()) ? 0 : 1;
            d[i][j] = std::min({
                d[i - 1][j] + 1,       // deletion
                d[i][j - 1] + 1,       // insertion
                d[i - 1][j - 1] + cost // substitution
            });
        }
    }
    return d[len1][len2];
}

QString StringMatcher::fixKeyboardLayout(const QString& input) {
    static const QMap<QChar, QChar> enToRu = {
        {'q', QChar(0x0439)}, {'w', QChar(0x0446)}, {'e', QChar(0x0443)}, {'r', QChar(0x043A)},
        {'t', QChar(0x0435)}, {'y', QChar(0x043D)}, {'u', QChar(0x0433)}, {'i', QChar(0x0448)},
        {'o', QChar(0x0449)}, {'p', QChar(0x0437)}, {'[', QChar(0x0445)}, {']', QChar(0x044A)},
        {'a', QChar(0x0444)}, {'s', QChar(0x044B)}, {'d', QChar(0x0432)}, {'f', QChar(0x0430)},
        {'g', QChar(0x043F)}, {'h', QChar(0x0440)}, {'j', QChar(0x043E)}, {'k', QChar(0x043B)},
        {'l', QChar(0x0434)}, {';', QChar(0x0436)}, {'\'', QChar(0x044D)},{'z', QChar(0x044F)},
        {'x', QChar(0x0447)}, {'c', QChar(0x0441)}, {'v', QChar(0x043C)}, {'b', QChar(0x0438)},
        {'n', QChar(0x0442)}, {'m', QChar(0x044C)}, {',', QChar(0x0431)}, {'.', QChar(0x044E)},
        {'/', '.'}
    };
    
    static QMap<QChar, QChar> ruToEn;
    if (ruToEn.isEmpty()) {
        for (auto it = enToRu.constBegin(); it != enToRu.constEnd(); ++it) {
            ruToEn[it.value()] = it.key();
        }
    }

    QString fixed;
    for (QChar c : input) {
        QChar lowerC = c.toLower();
        if (enToRu.contains(lowerC)) {
            fixed.append(c.isUpper() ? enToRu[lowerC].toUpper() : enToRu[lowerC]);
        } else if (ruToEn.contains(lowerC)) {
            fixed.append(c.isUpper() ? ruToEn[lowerC].toUpper() : ruToEn[lowerC]);
        } else {
            fixed.append(c);
        }
    }
    return fixed;
}

bool StringMatcher::isMatch(const QString& input, const std::vector<QString>& expectedList) {
    if (input.isEmpty() || expectedList.empty()) return false;
    
    QString cleanInput = input.trimmed().simplified();
    if (cleanInput.isEmpty()) return false;

    QString fixedInput = fixKeyboardLayout(cleanInput);

    for (const QString& expected : expectedList) {
        QString cleanExpected = expected.trimmed().simplified();
        
        // Direct
        if (cleanInput.compare(cleanExpected, Qt::CaseInsensitive) == 0 || 
            fixedInput.compare(cleanExpected, Qt::CaseInsensitive) == 0) {
            return true;
        }

        // Distance
        int distOrig = levenshteinDistance(cleanInput, cleanExpected);
        int distFixed = levenshteinDistance(fixedInput, cleanExpected);
        int bestDist = std::min(distOrig, distFixed);
        
        int allowedTypos = 0;
        int len = cleanExpected.length();
        if (len >= 4 && len <= 7) allowedTypos = 1;
        else if (len > 7) allowedTypos = 2;

        if (bestDist <= allowedTypos) {
            return true;
        }
    }
    
    return false;
}
