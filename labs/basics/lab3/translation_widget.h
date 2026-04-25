#ifndef TRANSLATION_WIDGET_H
#define TRANSLATION_WIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGroupBox>
#include <QTextEdit>
#include <QRandomGenerator>

class TranslationWidget : public QWidget {
    Q_OBJECT

public:
    explicit TranslationWidget(QWidget *parent = nullptr);

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
    QLineEdit* answerInput;
    QPushButton* submitButton;
    QPushButton* nextButton;
    QLabel* feedbackLabel;
    QLabel* progressLabel;
    QGroupBox* questionGroupBox;
    
    // Exercise data
    struct Question {
        QString french;
        QString russian;
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

#endif // TRANSLATION_WIDGET_H
