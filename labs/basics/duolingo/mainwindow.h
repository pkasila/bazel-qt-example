#pragma once

#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QTimer>
#include <QLabel>
#include <QPushButton>
#include <QLineEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QDialog>
#include <QShortcut>
#include <QSoundEffect>
#include <QUrl>

// Тип упражнения
enum class ExerciseType { Translation, Grammar };

// Структура для хранения вопроса
struct Question {
    QString text;
    QString correctAnswer;
    QStringList options; // Варианты ответа (только для Grammar)
    QString hint;        // Подсказка для клавиши H
};

// Диалоговое окно для выбора сложности
class DifficultyDialog : public QDialog {
    Q_OBJECT
public:
    explicit DifficultyDialog(QWidget *parent = nullptr);
    int getSelectedDifficulty() const; // 0 - Easy, 1 - Medium, 2 - Hard
private:
    QButtonGroup *diffGroup;
};

// Главное окно
class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

private slots:
    void onStartTranslation();
    void onStartGrammar();
    void onSubmitAnswer();
    void onTimeOut();
    void onChangeDifficulty();
    void onShowHint();

private:
    // --- Логика игры ---
    void loadQuestions(ExerciseType type);
    void showNextQuestion();
    void finishExercise(bool success);
    bool checkAnswerAdvanced(const QString& user, const QString& target);
    int levenshteinDistance(const QString& s1, const QString& s2);

    int score = 0;
    int lives = 3;
    int currentQuestionIndex = 0;
    int difficulty = 0; // 0 - Easy, 1 - Medium, 2 - Hard
    
    QList<Question> currentQuestions;
    ExerciseType currentType;
    QTimer *timer;
    int timeLeft;

    // --- UI Элементы ---
    QStackedWidget *mainStack;
    
    // Страница меню
    QWidget *menuWidget;
    QLabel *scoreLabel;

    // Страница упражнения
    QWidget *exerciseWidget;
    QProgressBar *progressBar;
    QLabel *timerLabel;
    QLabel *livesLabel;
    QLabel *questionLabel;
    
    // Динамический ввод
    QStackedWidget *inputStack; 
    QLineEdit *translationInput;     // Для ввода текста
    QWidget *grammarWidget;          // Для радио-кнопок
    QButtonGroup *grammarGroup;      // Группа радио-кнопок
    
    QPushButton *submitBtn;

    QSoundEffect *correctSound;
    QSoundEffect *wrongSound;
    QPushButton *helpBtn;
};