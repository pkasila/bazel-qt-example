#ifndef STRING_UTILS_H
#define STRING_UTILS_H

#include <QString>

namespace StringUtils {
QString normalizeForComparison(const QString &input);
bool fuzzyEquivalent(const QString &a, const QString &b);
int levenshteinDistance(const QString &a, const QString &b);
}

#endif // STRING_UTILS_H
