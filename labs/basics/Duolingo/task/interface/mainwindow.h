#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "../database/database_manager.h"
#include "admin_dialog.h"
#include "difficulty_dialog.h"
#include "grammar_widget.h"
#include "lt_settings_dialog.h"
#include "sidebar_widget.h"
#include "translation_widget.h"

#include <QKeyEvent>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QStackedWidget>
#include <QTimer>

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    explicit MainWindow(QWidget* parent = nullptr);

   protected:
    void keyPressEvent(QKeyEvent* event) override;

   private slots:
    void startTranslationSession();
    void startGrammarSession();
    void onAnswerSubmitted(bool correct);
    void updateTimer();
    void showDifficultyDialog();
    void showHelp();
    void showAdminDialog();
    void showLTSettingsDialog();

   private:
    void setupMenus();
    void nextQuestion();
    void endSession(bool success, const QString& message);

    SidebarWidget* sidebar_;
    QStackedWidget* stack_;
    TranslationWidget* translation_widget_;
    GrammarWidget* grammar_widget_;
    QWidget* welcome_widget_;

    QTimer* exercise_timer_;
    QProgressBar* progress_bar_;
    QLabel* timer_label_;
    int time_left_;

    int score_;
    int hearts_;
    int questions_left_;
    bool is_translation_mode_;
    QString difficulty_;
    int default_duration_;

    QList<TranslationQuestion> trans_questions_;
    QList<GrammarQuestion> grammar_questions_;
    int current_question_index_;
};

#endif  // MAINWINDOW_H
