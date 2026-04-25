#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QSoundEffect>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QRadioButton>
#include <QButtonGroup>
#include <QTimer>
#include <QVector>
#include <QPair>
#include <tuple>
#include <QDialog>
#include <QRegularExpression>
#include <QMessageBox>
#include <QKeyEvent>
#include <QFrame>
#include <QGridLayout>
#include <QMediaPlayer>
#include <QAudioOutput> 

struct Exercise {
    enum Type { Translation, Grammar };
    Type type;
    QString prompt;
    QString correctAnswer;
    QStringList options;
    QString hint;
    QString example;
};

class DifficultyDialog : public QDialog {
    Q_OBJECT
public:
    explicit DifficultyDialog(QWidget *parent = nullptr);
    int getDifficulty() const { return difficulty_; }
private:
    int difficulty_ = 2;
    QRadioButton *easyRadio_, *mediumRadio_, *hardRadio_;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void selectDifficulty();
    void showHelp();
    void startTranslation();
    void startGrammar();
    void submitAnswer();
    void onTimerTimeout();

private:
    void setupMenuBar();
    void setupUI();
    void applyModernStyle();
    void generateExercises(int diff, Exercise::Type type);
    void showCurrentExercise();
    void finishExercise(bool success);
    bool checkTranslation(const QString &user, const QString &correct);
    void showHint();

    QStackedWidget *stacked_;
    QWidget *menuWidget_, *exerciseWidget_;
    
    QPushButton *transBtn_, *grammarBtn_;
    QLabel *promptLabel_;
    QProgressBar *progress_;
    QTextEdit *answerEdit_;
    QVector<QRadioButton*> radios_;
    QButtonGroup *radioGroup_;
    QPushButton *submitBtn_, *hintBtn_;
    QTimer *timer_;
    QLabel *scoreValueLabel_;
    QLabel *attemptsValueLabel_;
    QLabel *timerValueLabel_;

    QMediaPlayer *correctPlayer_ = nullptr;
    QMediaPlayer *wrongPlayer_ = nullptr;
    QAudioOutput *audioOutput_ = nullptr;
    
    void playCorrectSound();
    void playWrongSound();
    int levenshteinDistance(const QString &s1, const QString &s2);
    int difficulty_ = 2;
    int score_ = 0, currentIdx_ = 0, wrongAttempts_ = 0;
    int timeRemaining_ = 300;
    QVector<Exercise> exercises_;
};

#endif