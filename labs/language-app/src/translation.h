#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QVector>
#include <QLabel>
#include <QTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>

struct Sentence;

class Translation : public QWidget {
    Q_OBJECT

public:
    explicit Translation(QWidget *parent = nullptr);
    ~Translation() override;

    void startExercise(const QString &level, const QVector<Sentence> &sentences, int timePerItemSec = 60);
    void updateLevel(const QString &level);
    int getScore() const;
    QTimer* getTimer() const;

signals:
    void exerciseComplete(int finalScore);
    void answerIncorrect();
    void timeExpiredForItem();

private slots:
    void onSubmitBtnClicked();
    void onStartBtnClicked();
    void onItemTimerTimeout();

private:
    void initGui();
    void applyStyle();
    void loadDifficultySettings();
    void displayItem();
    void checkAnswer(bool timedOut = false);
    void nextItem();
    void enableControls(bool enable);
    void updateUiProgress();

    QLabel *infoLabel;
    QLabel *timeLabel;
    QProgressBar *progressBar;
    QLabel *sourceTextLabel;
    QTextEdit *inputEdit;
    QPushButton *submitBtn;
    QPushButton *startBtn;

    QVBoxLayout *mainLayout;
    QHBoxLayout *topInfoLayout;
    QHBoxLayout *buttonsLayout;

    QVector<Sentence> allSentences;
    int itemIdx;
    int maxAttempts;
    int currentFails;
    int score;
    int pointsPerItem;
    QString difficultyLevel;

    QTimer *timer;
    int timeAllowedPerItem;
    int timeRemaining;
};

#endif // TRANSLATION_H
