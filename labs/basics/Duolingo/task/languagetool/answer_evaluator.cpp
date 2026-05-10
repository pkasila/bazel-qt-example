#include "answer_evaluator.h"

#include <QDebug>

EvaluationConfig AnswerEvaluator::globalConfig;

AnswerEvaluator::AnswerEvaluator(QObject* parent)
    : QObject(parent)
    , lt_client_(new LanguageToolClient(this))
    , request_token_(0)
    , current_token_(0) {
    connect(
        lt_client_, &LanguageToolClient::checkFinished, this, &AnswerEvaluator::onLTCheckFinished);
}

void AnswerEvaluator::evaluate(const QString& user_text, const QString& expected_text) {
    user_text_ = user_text;
    expected_text_ = expected_text;
    current_token_ = ++request_token_;

    float lev = levenshteinSimilarity(user_text.trimmed(), expected_text.trimmed());
    if (lev >= 0.98f) {
        AnswerEvaluation eval;
        eval.grade = AnswerGrade::CORRECT;
        eval.similarity = lev;
        eval.feedback = QString::fromUtf8("\u2705 Correct!");
        emit evaluationFinished(eval);
        return;
    }

    lt_client_->check(user_text);
}

void AnswerEvaluator::onLTCheckFinished(const LTCheckResult& lt_result) {
    if (current_token_ != request_token_) {
        return;
    }

    AnswerEvaluation eval;
    eval.similarity = wordOverlap(user_text_, expected_text_);

    float lev = levenshteinSimilarity(user_text_.toLower().trimmed(),
                                       expected_text_.toLower().trimmed());

    if (!lt_result.success) {
        if (lev >= 0.70f) {
            eval.grade = AnswerGrade::CORRECT;
            eval.feedback = QString::fromUtf8("\u2705 Accepted (offline check).");
        } else {
            eval.grade = AnswerGrade::ERROR;
            eval.feedback = QString::fromUtf8("\u274c Incorrect.");
        }
        emit evaluationFinished(eval);
        return;
    }

    QStringList typo_messages;
    QStringList error_messages;
    QStringList all_suggestions;

    for (const auto& match : lt_result.matches) {
        bool is_typo_cat =
            (match.ruleCategory == "TYPOS" || match.ruleCategory == "COMPOUNDING" ||
             match.ruleId.contains("SPELL", Qt::CaseInsensitive) ||
             match.ruleId.contains("MORFOLOGIK", Qt::CaseInsensitive));

        bool is_punctuation =
            (match.ruleCategory == "PUNCTUATION" ||
             match.ruleId.contains("PUNCTUATION", Qt::CaseInsensitive));
        bool is_style =
            (match.ruleCategory == "STYLE" || match.ruleCategory == "SEMANTICS" ||
             match.ruleCategory == "TYPOGRAPHY");
        bool is_syntax =
            (match.ruleCategory == "CASING" ||
             match.ruleId.contains("CASING", Qt::CaseInsensitive));
        bool is_grammar = (!is_typo_cat && !is_punctuation && !is_style && !is_syntax);

        bool is_strict = false;
        if (is_grammar && globalConfig.strictGrammar) {
            is_strict = true;
        }
        if (is_syntax && globalConfig.strictSyntax) {
            is_strict = true;
        }
        if (is_typo_cat && globalConfig.strictTypos) {
            is_strict = true;
        }
        if (is_punctuation && globalConfig.strictPunctuation) {
            is_strict = true;
        }
        if (is_style && globalConfig.strictStyle) {
            is_strict = true;
        }

        QString desc = match.message;
        if (!match.replacements.isEmpty()) {
            QStringList top = match.replacements.mid(0, 3);
            desc += QString::fromUtf8("  \u2192 ") + top.join(", ");
            all_suggestions.append(top);
        }

        if (is_strict) {
            error_messages.append(desc);
        } else {
            typo_messages.append(desc);
        }
    }

    eval.typos = typo_messages;
    eval.errors = error_messages;
    eval.suggestions = all_suggestions;

    bool has_typos = !typo_messages.isEmpty();
    bool has_errors = !error_messages.isEmpty();
    bool is_clean_text = lt_result.matches.isEmpty();

    if (is_clean_text) {
        int user_len = user_text_.trimmed().length();
        int expected_len = expected_text_.trimmed().length();
        float length_ratio = (expected_len > 0)
                                 ? static_cast<float>(std::min(user_len, expected_len)) /
                                       std::max(user_len, expected_len)
                                 : 0.0f;
        bool similar_length = length_ratio >= 0.4f && user_len >= 3;

        if ((eval.similarity >= 0.3f || lev >= 0.35f) && similar_length) {
            eval.grade = AnswerGrade::SYNONYM;
            eval.feedback = QString::fromUtf8("\u2705 Accepted as a valid synonym translation!");
        } else {
            eval.grade = AnswerGrade::ERROR;
            eval.feedback = QString::fromUtf8(
                                "\u274c Grammatically correct, but not the right meaning.\n"
                                "Expected: ") +
                            expected_text_;
        }
    } else if (has_typos && !has_errors) {
        eval.grade = AnswerGrade::TYPO;
        QString fb = QString::fromUtf8("\u270f\ufe0f Typos found — fix and try again:\n");
        for (const auto& t : typo_messages) {
            fb += QString::fromUtf8("  \u2022 ") + t + "\n";
        }
        eval.feedback = fb.trimmed();
    } else {
        eval.grade = AnswerGrade::ERROR;
        QString fb;
        if (has_errors) {
            fb += QString::fromUtf8("\u274c Grammar errors:\n");
            for (const auto& e : error_messages) {
                fb += QString::fromUtf8("  \u2022 ") + e + "\n";
            }
        }
        if (has_typos) {
            fb += QString::fromUtf8("\u270f\ufe0f Typos:\n");
            for (const auto& t : typo_messages) {
                fb += QString::fromUtf8("  \u2022 ") + t + "\n";
            }
        }
        eval.feedback = fb.trimmed();
    }

    emit evaluationFinished(eval);
}

int AnswerEvaluator::levenshteinDistance(const QString& s1, const QString& s2) {
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

float AnswerEvaluator::levenshteinSimilarity(const QString& s1, const QString& s2) {
    if (s1.isEmpty() && s2.isEmpty()) {
        return 1.0f;
    }
    int dist = levenshteinDistance(s1, s2);
    int maxLen = std::max(s1.length(), s2.length());
    return 1.0f - (static_cast<float>(dist) / maxLen);
}

float AnswerEvaluator::wordOverlap(const QString& a, const QString& b) {
    QStringList wordsA = a.toLower().simplified().split(' ', Qt::SkipEmptyParts);
    QStringList wordsB = b.toLower().simplified().split(' ', Qt::SkipEmptyParts);

    QSet<QString> setA(wordsA.begin(), wordsA.end());
    QSet<QString> setB(wordsB.begin(), wordsB.end());

    int common = 0;
    for (const auto& w : setA) {
        if (setB.contains(w)) {
            common++;
        }
    }

    int total = std::max(setA.size(), setB.size());
    return total > 0 ? static_cast<float>(common) / total : 0.0f;
}
