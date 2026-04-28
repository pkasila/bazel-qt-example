#pragma once

#include <QButtonGroup>
#include <QDialog>
#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QShortcut>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QUrl>

class QVBoxLayout;

enum class ExerciseType {
    Translation,
    Grammar
};

struct Question {
    QString text;
    QString correctAnswer;
    QStringList options;
    QString hint;
};

class DifficultyDialog : public QDialog {
    Q_OBJECT

public:
    explicit DifficultyDialog(int currentDifficulty, QWidget *parent = nullptr);
    int selectedDifficulty() const;

private:
    QButtonGroup *difficultyGroup;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override = default;

private slots:
    void startTranslation();
    void startGrammar();
    void submitAnswer();
    void tickTimer();
    void changeDifficulty();
    void showHint();

private:
    void buildInterface();
    void buildMenuBar();
    void buildSounds();
    void applyTheme();
    void loadQuestions(ExerciseType type);
    void showQuestion();
    void finishExercise(bool success, const QString &message);
    void resetGrammarOptions(const Question &question);
    void updateStatus();
    QString resourcePath(const QString &fileName) const;
    bool isTranslationCorrect(const QString &userAnswer, const QString &correctAnswer) const;
    QString normalizeAnswer(const QString &text) const;
    int levenshteinDistance(const QString &left, const QString &right) const;
    QList<Question> translationBank() const;
    QList<Question> grammarBank() const;
    int exerciseSize() const;
    int initialLives() const;
    int initialTime() const;
    int reward() const;
    QString difficultyName() const;

    int score = 0;
    int lives = 3;
    int currentQuestionIndex = 0;
    int difficulty = 0;
    int timeLeft = 60;
    ExerciseType currentType = ExerciseType::Translation;
    QList<Question> currentQuestions;

    QWidget *rootWidget = nullptr;
    QLabel *scoreLabel = nullptr;
    QLabel *difficultyLabel = nullptr;
    QLabel *titleLabel = nullptr;
    QLabel *subtitleLabel = nullptr;
    QLabel *timerLabel = nullptr;
    QLabel *livesLabel = nullptr;
    QLabel *questionLabel = nullptr;
    QLabel *statusLabel = nullptr;
    QProgressBar *progressBar = nullptr;
    QStackedWidget *exerciseStack = nullptr;
    QTextEdit *translationInput = nullptr;
    QWidget *grammarWidget = nullptr;
    QVBoxLayout *grammarLayout = nullptr;
    QButtonGroup *grammarGroup = nullptr;
    QPushButton *translationButton = nullptr;
    QPushButton *grammarButton = nullptr;
    QPushButton *submitButton = nullptr;
    QPushButton *helpButton = nullptr;
    QTimer *timer = nullptr;
    QSoundEffect *correctSound = nullptr;
    QSoundEffect *wrongSound = nullptr;
};
