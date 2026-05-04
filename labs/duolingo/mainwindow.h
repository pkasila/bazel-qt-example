#pragma once
#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>
#include <QSoundEffect>

struct Question {
    QString text;         // Текст вопроса или фраза для перевода
    QString correctAnswer; // Правильный ответ
    QStringList options;  // Варианты (только для грамматики)
    QString hint;         // Та самая подсказка по клавише 'H'
};

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override; // Для перехвата клавиши 'H'

private slots:
    void changeDifficulty();
    void startTranslation();
    void startGrammar();
    void checkTranslation();
    void checkGrammar();
    void timeOut();

private:
    void setupUi();
    void applyTheme();
    void endExercise(bool success, const QString &reason);
    void updateTranslationUI();
    int calculateLevenshteinDistance(const QString &s1, const QString &s2); 

    QList<Question> translationTasks;
    QList<Question> grammarTasks;
    void setupTasks(); // Метод для заполнения списков

    // Компоненты UI
    QStackedWidget *stackedWidget;
    QWidget *menuPage, *translationPage, *grammarPage;
    
    QProgressBar *progressBar;
    QLabel *scoreLabel;
    QLabel *timerLabel;
    
    // Элементы упражнения Translation
    QLabel *transWordLabel;
    QTextEdit *transInput;
    
    // Элементы упражнения Grammar
    QLabel *grammQuestionLabel;
    QButtonGroup *radioGroup;
    QVBoxLayout *radioLayout;
    
    QTimer *exerciseTimer;
    QSoundEffect *soundEffect;

    // Состояние игры
    int difficulty = 1;
    int currentTask = 0;
    int totalTasks = 5;
    int strikes = 0;
    const int MAX_STRIKES = 3;
    int streetCred = 0; // Местный аналог баллов
    int timeLeft = 30;
    QString currentHint;
};