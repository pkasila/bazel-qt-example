#pragma once

#include <QString>
#include <vector>

class StringMatcher {
public:
    static bool isMatch(const QString& input, const std::vector<QString>& expectedList);
    static int levenshteinDistance(const QString& s1, const QString& s2);
    static QString fixKeyboardLayout(const QString& input);
};
