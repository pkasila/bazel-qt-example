#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "grammartask.h"
#include "translationtask.h"

#include <QAction>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QGroupBox>
#include <QKeyEvent>
#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QStackedWidget>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

const int TASKS_PER_SERIES = 5;
const int MAX_ATTEMPTS_PER_SERIES = 3;
const int SERIES_TIME_SECONDS = 180;

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   protected:
    void keyPressEvent(QKeyEvent* ev) override;

   private slots:
    void onStartBtnClicked();
    void switchToTransEx();
    void switchToGrammarEx();
    void checkTransAnswer();
    void checkGrammarAnswer();
    void updateSeriesTimer();
    void openDifficultyDialog();
    void showHelp();

   private:
    QSqlDatabase db_;
    QWidget* mainContainer_;
    QStackedWidget* stackedWidget_;
    QWidget* topPanel_;
    QProgressBar* progressBar_;
    QLabel* scoreLbl_;
    QLabel* attemptsLbl_;
    QLabel* timerLbl_;
    QTimer* seriesTimer_;
    QWidget* startPage_;
    QLabel* logoLbl_;
    QLabel* welcomeLbl_;
    QPushButton* startBtn_;
    QWidget* placeholderPage_;
    QLabel* placeholderLbl_;
    QPushButton* transBtn_;
    QPushButton* grammarBtn_;
    QWidget* transPage_;
    QLabel* transTaskLbl_;
    QTextEdit* transInput_;
    QPushButton* transSubmitBtn_;
    QWidget* grammarPage_;
    QLabel* grammarQuestLbl_;
    QGroupBox* grammarOptsGrp_;
    QVBoxLayout* grammarOptsLayout_;
    QVector<QRadioButton*> grammarOptsRb_;
    QPushButton* grammarSubmitBtn_;

    enum class ExType { None, Translation, Grammar };
    ExType curExType_;
    QVector<TranslationTask> curTransSeries_;
    QVector<GrammarTask> curGrammarSeries_;
    int curTaskIdx_;
    int completedTasks_;
    int curScore_;
    int remAttempts_;
    int seriesTimeRemSec_;
    int curDifficulty_;
    QDialog* diffDialog_;
    QComboBox* diffComboBox_;

    void initDb();
    void setupMainLayout();
    void setupTopPanel();
    void setupStartPage();
    void setupPlaceholderPage();
    void setupMenu();
    void setupTransPage();
    void setupGrammarPage();
    void setupConnections();
    void createDiffDialog();
    void startNewSeries(ExType type);
    void loadTasksForSeries();
    void loadNextTask();
    void updateTransTaskUI();
    void updateGrammarTaskUI();
    void endSeries(bool success, const QString& msg);
    void updateStatsUI();
    void resetGameStats();
};
#endif  // MAINWINDOW_H
