#ifndef GRAMMAR_WIDGET_H
#define GRAMMAR_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QButtonGroup>
#include <QTextEdit>
#include <QRandomGenerator>

class GrammarWidget : public QWidget {
    Q_OBJECT

public:
    explicit GrammarWidget(QWidget *parent = nullptr);

signals:
    void exerciseCompleted(bool success, int score);
    void exerciseFailed(const QString& reason);
    void progressUpdated(int current, int total);

public slots:
    void startExercise(int difficulty);

private slots:
    void checkAnswer();
    void showNextQuestion();

private:
    void setupUI();
    void loadQuestions(int difficulty);
    void showQuestion(int index);
    void applyTheme();
    
    // UI Components
    QLabel* titleLabel;
    QLabel* questionLabel;
    QTextEdit* questionDisplay;
    QButtonGroup* answerGroup;
    QList<QRadioButton*> answerRadioButtons;
    QPushButton* submitButton;
    QPushButton* nextButton;
    QLabel* feedbackLabel;
    QLabel* progressLabel;
    QGroupBox* questionGroupBox;
    QGroupBox* answersGroupBox;
    
    // Exercise data
    struct Question {
        QString sentence;
        QString correctAnswer;
        QStringList options;
        QString hint;
    };
    
    QList<Question> questions;
    int currentQuestionIndex;
    int correctAnswers;
    int totalQuestions;
    int mistakesCount;
    int maxMistakes;
    int difficulty;
    
    bool exerciseStarted;
};

#endif // GRAMMAR_WIDGET_H
