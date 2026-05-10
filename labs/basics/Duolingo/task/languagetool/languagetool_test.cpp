#include "../languagetool/answer_evaluator.h"
#include "../languagetool/languagetool_client.h"

#include <QCoreApplication>
#include <QDebug>
#include <QTextStream>
#include <QTimer>

static int passed_count = 0;
static int failed_count = 0;
static int pending_count = 0;
static QTextStream out_stream(stdout);

void checkDone() {
    if (pending_count == 0) {
        out_stream << "\n========================================\n";
        out_stream << "Results: " << passed_count << " passed, " << failed_count << " failed\n";
        out_stream << "========================================\n";
        out_stream.flush();
        QCoreApplication::exit(failed_count > 0 ? 1 : 0);
    }
}

void testLTMatches(
    LanguageToolClient* client, const QString& text, const QString& lang, int min_matches,
    const QString& test_name) {
    pending_count++;
    QObject::connect(
        client, &LanguageToolClient::checkFinished,
        [test_name, min_matches](const LTCheckResult& result) {
            if (!result.success) {
                out_stream << "  FAIL [" << test_name << "] API error: " << result.errorString << "\n";
                failed_count++;
            } else if (result.matches.size() >= min_matches) {
                out_stream << "  PASS [" << test_name << "] got " << result.matches.size()
                           << " matches (>= " << min_matches << ")\n";
                for (const auto& m : result.matches) {
                    out_stream << "       -> " << m.message << " [cat: " << m.ruleCategory << "]\n";
                }
                passed_count++;
            } else {
                out_stream << "  FAIL [" << test_name << "] got " << result.matches.size()
                           << " matches, expected >= " << min_matches << "\n";
                failed_count++;
            }
            pending_count--;
            checkDone();
        });
    client->check(text, lang);
}

void testEvaluation(
    AnswerEvaluator* eval, const QString& user_text, const QString& expected_text,
    AnswerGrade expected_grade, const QString& test_name) {
    pending_count++;
    QObject::connect(
        eval, &AnswerEvaluator::evaluationFinished,
        [test_name, expected_grade](const AnswerEvaluation& result) {
            const char* grade_names[] = {"CORRECT", "SYNONYM", "TYPO", "ERROR"};
            int got = static_cast<int>(result.grade);
            int exp = static_cast<int>(expected_grade);

            if (result.grade == expected_grade) {
                out_stream << "  PASS [" << test_name << "] grade=" << grade_names[got] << "\n";
                out_stream << "       feedback: " << result.feedback.left(120) << "\n";
                passed_count++;
            } else {
                out_stream << "  FAIL [" << test_name << "] expected " << grade_names[exp] << ", got "
                           << grade_names[got] << "\n";
                out_stream << "       feedback: " << result.feedback << "\n";
                failed_count++;
            }
            pending_count--;
            checkDone();
        });
    eval->evaluate(user_text, expected_text);
}

int main(int argc, char* argv[]) {
    QCoreApplication app(argc, argv);

    out_stream << "========================================\n";
    out_stream << " LanguageTool & Evaluator Tests\n";
    out_stream << "========================================\n\n";
    out_stream << "Running tests (requires internet)...\n\n";

    auto* lt1 = new LanguageToolClient(&app);
    testLTMatches(lt1, "This is a speling mistake.", "en-US", 1, "LT: English typo");

    auto* lt2 = new LanguageToolClient(&app);
    testLTMatches(lt2, "She go to school yesterday.", "en-US", 1, "LT: English grammar");

    auto* lt3 = new LanguageToolClient(&app);
    testLTMatches(lt3, "Превет мир", "ru-RU", 1, "LT: Russian typo");

    auto* lt4 = new LanguageToolClient(&app);
    testLTMatches(lt4, "this is a test.", "en-US", 1, "LT: Capitalization");

    out_stream << "\n--- AnswerEvaluator ---\n";

    auto* ev1 = new AnswerEvaluator(&app);
    testEvaluation(ev1, "Кот на столе", "Кот на столе", AnswerGrade::CORRECT, "Eval: exact match");

    auto* ev2 = new AnswerEvaluator(&app);
    testEvaluation(
        ev2, "кот на столе", "Кот на столе", AnswerGrade::ERROR,
        "Eval: case-insensitive (Syntax Error)");

    auto* ev3 = new AnswerEvaluator(&app);
    testEvaluation(
        ev3, "Кошка на столе", "Кот на столе", AnswerGrade::SYNONYM,
        "Eval: synonym (Кошка vs Кот)");

    auto* ev4 = new AnswerEvaluator(&app);
    testEvaluation(
        ev4, "Кот на стале", "Кот на столе", AnswerGrade::SYNONYM,
        "Eval: Russian near-miss (accepted as synonym)");

    auto* ev5 = new AnswerEvaluator(&app);
    testEvaluation(
        ev5, "I like pizza", "Кот на столе", AnswerGrade::ERROR, "Eval: completely wrong");

    auto* ev6 = new AnswerEvaluator(&app);
    testEvaluation(
        ev6, "Thanks a lot", "Thank you very much", AnswerGrade::SYNONYM, "Eval: English synonym");

    auto* ev8 = new AnswerEvaluator(&app);
    testEvaluation(
        ev8, "this is a test", "This is a test", AnswerGrade::ERROR,
        "Eval: Capitalization (Syntax Error)");

    QTimer::singleShot(45'000, &app, [&]() {
        out_stream << "\n  TIMEOUT: Not all tests completed in 45s.\n";
        out_stream << "  (Remaining pending: " << pending_count << ")\n";
        out_stream.flush();
        QCoreApplication::exit(1);
    });

    return app.exec();
}
