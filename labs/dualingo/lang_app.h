#ifndef LANG_APP_H
#define LANG_APP_H

// Подключаем модули корректно для Bazel
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QStackedWidget>
#include <QtCore/QVector>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QTimer>

QT_BEGIN_NAMESPACE
class QLabel;
class QTextEdit;
class QPushButton;
class QProgressBar;
class QRadioButton;
class QButtonGroup;
class QVBoxLayout;
QT_END_NAMESPACE

enum class TaskType { Translation, Grammar };

struct Task {
    TaskType type;
    QString question;
    QString correctAnswer;
    QStringList options; // Для Grammar
    QString hint;        // Подсказка для 'H'
};

class LangApp : public QMainWindow {
    Q_OBJECT

public:
    LangApp(QWidget* parent = nullptr);

private slots:
    void openDifficultyDialog();
    
    // Запуск конкретных упражнений
    void startTranslationTask();
    void startGrammarTask();
    
    // Проверка ответов
    void submitTranslation();
    void submitGrammar();
    
    void onTimerTick();
    void showHelp();
    void playAudioMock();

private:
    void setupUI();
    void setupMenu();
    void loadMockData();
    
    void startSession(TaskType type);
    void displayCurrentTask();
    void processAnswer(bool isCorrect);
    void finishExercise(bool success, const QString& message);
    
    // Advanced Feature (Расстояние Левенштейна)
    bool isTranslationAcceptable(const QString& input, const QString& expected);

    // Данные
    QVector<Task> m_allTranslationTasks;
    QVector<Task> m_allGrammarTasks;
    QVector<Task> m_sessionTasks;
    
    int m_currentIndex = 0;
    int m_totalPoints = 0;
    int m_sessionPoints = 0;
    int m_maxMistakes = 3;
    int m_currentMistakes = 0;
    int m_timeLimit = 60;
    int m_timeLeft = 0;

    // UI Левая панель (Меню и статы)
    QLabel* m_scoreLabel;
    QLabel* m_livesLabel;
    QLabel* m_timerLabel;
    QProgressBar* m_progressBar;
    QPushButton* m_btnTranslation;
    QPushButton* m_btnGrammar;
    QTimer* m_timer;

    // UI Правая панель (Динамическая)
    QStackedWidget* m_rightPanel;

    // Страница 1: Перевод
    QLabel* m_transQuestionLabel;
    QTextEdit* m_transTextEdit;
    QPushButton* m_transSubmitBtn;
    QPushButton* m_transAudioBtn;

    // Страница 2: Грамматика
    QLabel* m_gramQuestionLabel;
    QButtonGroup* m_gramRadioGroup;
    QVBoxLayout* m_gramRadioLayout;
    QPushButton* m_gramSubmitBtn;

    // Страница 3: Результат
    QLabel* m_resultLabel;
};

#endif // LANG_APP_H