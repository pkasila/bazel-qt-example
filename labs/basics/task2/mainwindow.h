#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "keyboardwidget.h"

#include <QActionGroup>
#include <QComboBox>
#include <QElapsedTimer>
#include <QLabel>
#include <QMainWindow>
#include <QParallelAnimationGroup>
#include <QProgressBar>
#include <QRadioButton>
#include <QTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

   protected:
    void keyPressEvent(QKeyEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;

   private slots:
    void loadTrainingText();
    void updateStats();
    void onLayoutChanged(int index);

   private:
    void SetupUi();
    void pauseSession();
    void resumeSession();
    void startNewSession();
    void updateDisplay();
    void processInput(const QString& input);
    void handleBackspace();
    void showCompletionMessage();

    // UI
    KeyboardWidget* keyboardWidget;
    QComboBox* layoutComboBox;
    QRadioButton* radioEnableKeyboard;
    QLabel* textDisplay;
    QLabel* textPaused;
    QLabel* statsLabel;
    QLabel* saluteEffect;
    QProgressBar* progressBar;
    QParallelAnimationGroup* saluteAnimGroup;
    QActionGroup* alignmentGroup;
    int charPerLine = 80;

    // Training Data
    QString trainingText;
    QStringList trainingLines;
    int currentLineIndex = 0;
    int currentPositionInLine = 0;

    // Stats
    QTimer statsTimer;
    QElapsedTimer activeTimer;
    QElapsedTimer pausedTimer;
    int64_t activems = 0;
    int64_t pausedms = 0;
    int totalTyped = 0;
    int correctTyped = 0;
    bool sessionActive = false;
};

#endif