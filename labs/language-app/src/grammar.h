#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <QVector>
#include <QStringList>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QTimer>

struct Question;

class Grammar : public QWidget {
    Q_OBJECT

public:
    explicit Grammar(QWidget *parent = nullptr);
    ~Grammar() override;

    void startExercise(const QString &level, const QVector<Question> &questions, int timePerQSec = 30);
    void updateLevel(const QString &level);
    int getScore() const;
    QTimer* getTimer();

signals:
    void exerciseComplete(int finalScore);
    void answerIncorrect();
    void timeExpiredForQuestion();

private slots:
    void onSubmitButtonClicked();
    void onStartButtonClicked();
    void onQuestionTimerTimeout();

private:
    void setupUi();
    void applyStyle();
    void loadDifficultySettings();
    void clearOptions();
    void displayQuestion();
    void checkAnswer(bool timedOut = false);
    void nextQuestion();
    void enableControls(bool enable);
    void updateUiProgress();


    QLabel *infoLabel;
    QLabel *timeLabel;
    QProgressBar *progressBar;
    QLabel *questionTextLabel;
    QButtonGroup *optionGroup;
    QVector<QRadioButton*> optionButtons;
    QPushButton *submitBtn;
    QPushButton *startBtn;

    QVBoxLayout *mainLayout;
    QHBoxLayout *topInfoLayout;
    QVBoxLayout *optionsLayout;
    QHBoxLayout *buttonsLayout;

    QVector<Question> allQuestions;
    int qIdx;
    int maxAttempts;
    int currentFails;
    int score;
    int pointsPerQ;
    QString difficultyLevel;

    QTimer *timer;
    int timeAllowedPerQ;
    int timeRemaining;
};

#endif // GRAMMAR_H
