#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class QStackedWidget;
class QAction;
class QMenu;
class TranslationExerciseWidget; // Forward declaration
class GrammarExerciseWidget;     // Forward declaration
class StatisticsViewWidget;    // Forward declaration
class DifficultyDialog;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void openTranslationExercise();
    void openGrammarExercise();
    void openStatistics();
    void changeDifficulty();
    void showHelp(); // For 'H' key

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    void createMenus();
    void createActions();
    void setupCentralWidget();

    QStackedWidget *m_stackedWidget;

    // Potentially separate views or reuse exercise widgets
    TranslationExerciseWidget *m_translationWidget;
    GrammarExerciseWidget *m_grammarWidget;
    StatisticsViewWidget *m_statisticsWidget;

    // Menu Bar
    QMenu *m_fileMenu;
    QMenu *m_exercisesMenu;
    QMenu *m_settingsMenu;
    QMenu *m_helpMenu;

    // Actions
    QAction *m_translationAction;
    QAction *m_grammarAction;
    QAction *m_statisticsAction;
    QAction *m_difficultyAction;
    QAction *m_exitAction;
    QAction *m_aboutAction; // Example
};

#endif // MAINWINDOW_H