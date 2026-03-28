#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "grammar.h"
#include "translate.h"

#include <QLabel>
#include <QMainWindow>
#include <QMenuBar>
#include <QPushButton>
#include <QStackedWidget>
#include <QVBoxLayout>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void changeModeTranslate();
    void changeModeGrammar();
    void setModeBeginner();
    void setModeIntermediate();
    void setModeAdvanced();
    void showTranslationFeedback(bool isCorrect);
    void showGrammarFeedback(int answerId, bool isCorrect);

   private:
    void setupUI();

    QVBoxLayout* layout;
    QWidget* centralWidget;
    QStackedWidget* stackedWidget;
    QMenuBar* menuBar;
    TranslateMode* translate;
    GrammarMode* grammar;
    QPushButton* translateButton;
    QPushButton* grammarButton;
    QLabel* scoreLabel;
    int mode = 1;
};

#endif  // MAINWINDOW_H