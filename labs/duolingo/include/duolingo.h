#ifndef DUOLINGO_H
#define DUOLINGO_H

#include <QAudioOutput>
#include <QButtonGroup>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPair>
#include <QProgressBar>
#include <QRadioButton>
#include <QRegularExpression>
#include <QStackedWidget>
#include <QTimer>
#include <QVector>
#include <qboxlayout.h>
#include <qlistwidget.h>

class Duolingo : public QMainWindow {
    Q_OBJECT

   public:
    explicit Duolingo(QWidget* parent = nullptr);
    ~Duolingo() override;

   protected:
    //void keyPressEvent(QKeyEvent* event) override;
    bool eventFilter(QObject* obj, QEvent* event) override;

   private slots:
    void OnLearnClicked();
    void OnTranslationClicked();
    void OnGrammarClicked();
    void ShowLearnExercise();
    void OnSubmitClicked();
    void OnTimeout();
    void UpdateTimer();
    void UpdateTimerDisplay() const;
    void ShowHelp();
    void ShowDifficultyDialog();
    void ShowRating();

   private:
    QListWidget* ratingList;
    QStackedWidget* stackedWidget;
    QStackedWidget* exercisesStack;
    QProgressBar* learnProgressBar;
    QProgressBar* translateProgressBar;
    QProgressBar* grammarProgressBar;
    QLabel* learnScoreLabel;
    QLabel* learnTimerLabel;
    QLabel* translateScoreLabel;
    QLabel* translateTimerLabel;
    QLabel* grammarScoreLabel;
    QLabel* grammarTimerLabel;
    QTimer* exerciseTimer;
    QTimer* countdownTimer;
    QMediaPlayer* player;
    QAudioOutput* audioOutput;
    QWidget* learnPageWidget;
    QWidget* translationPageWidget;
    QWidget* grammarPageWidget;
    QVBoxLayout* learnLayout;
    QVBoxLayout* translationLayout;
    QVBoxLayout* grammarLayout;
    QWidget* learnExerciseWidget;
    QWidget* translationExerciseWidget;
    QWidget* grammarExerciseWidget;

    int mode;
    int currentExercise;
    int score;
    int wrongAttempts;
    int timeLeft;
    QString currentDifficulty;
    QVector<QPair<QString, QString>> translationExercises;
    QVector<QPair<QString, QString>> grammarExercises;
    QVector<QString> grammarQuestions;
    QVector<QVector<QString>> grammarOptions;
    QVector<int> grammarAnswers;

    void IsModeChanged(int index);
    void SetupExercises();
    void CreatePages();
    void LearnPage(QVBoxLayout* layout);
    void TranslationPage(QVBoxLayout* layout);
    void GrammarPage(QVBoxLayout* layout);
    void CreateCommonPageElements(QVBoxLayout* layout);
    void ShowTranslationExercise();
    void ShowGrammarExercise();
    void ClearExerciseWidget() const;
    void CheckTranslationAnswer();
    void CheckGrammarAnswer();
    void FinishExercise(bool success);
    void PlaySound(bool correct);
    bool IsAnswerCorrect(const QString& userAnswer, const QString& correctAnswer);
};

#endif  // DUOLINGO_H