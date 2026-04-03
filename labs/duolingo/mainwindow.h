#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QPushButton>
#include <QTextEdit>
#include <QRadioButton>
#include <QProgressBar>
#include <QLabel>
#include <QTimer>
#include <QGroupBox>
#include <QVBoxLayout>
#include <vector>

#include "exercisedata.h"


class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void ShowDifficultyDialog();
    void StartTranslationExercise();
    void StartGrammarExercise();
    void HandleSubmit();
    void UpdateTimer();
    void HandleTimeout();

private:
    void SetupUi();
    void SetupMenu();
    void CreateMainMenuPage();
    void CreateExercisePage();
    void LoadExerciseData();
    void StartNewExerciseSet(ExerciseType type);
    void LoadNextQuestion();
    void DisplayTranslationQuestion(const TranslationQuestion& q);
    void DisplayGrammarQuestion(const GrammarQuestion& q);
    void CheckAnswer();
    void EndExercise(bool ok, const QString& msg);
    void UpdateProgressBar();
    void ShowHelp();

    QMenuBar* menu_bar_;
    QStackedWidget *main_stacked_widget_;
    QWidget *main_menu_page_;
    QWidget *exercise_page_;
    QAction* difficulty_action_;
    QPushButton *translation_btn_;
    QPushButton *grammar_btn_;
    QLabel *score_label_;
    QLabel *exercise_title_label_;
    QProgressBar *progress_bar_;
    QLabel *timer_label_;
    QLabel *question_text_label_;
    QTextEdit *translation_answer_edit_;
    QGroupBox *grammar_options_group_;
    QVBoxLayout *grammar_options_layout_;
    QVector<QRadioButton*> grammar_radio_btns_;
    QPushButton *submit_btn_;
    QWidget *translation_input_widget_;
    QWidget *grammar_input_widget_;
    QStackedWidget *exercise_input_stacked_widget_;

    Difficulty cur_difficulty_ = Difficulty::Easy;
    ExerciseType cur_exercise_type_ = ExerciseType::None;

    int total_score_ = 0;
    int cur_exercise_score_ = 0;
    int questions_in_set_ = 5;
    int max_incorrect_attempts_ = 3;
    int time_limit_seconds_ = 60;

    std::vector<TranslationQuestion> all_translation_questions_;
    std::vector<GrammarQuestion> all_grammar_questions_;
    std::vector<TranslationQuestion> cur_translation_set_;
    std::vector<GrammarQuestion> cur_grammar_set_;

    int cur_question_index_ = 0;
    int incorrect_attempts_count_ = 0;
    QTimer *exercise_timer_;
    int remaining_time_;
};

#endif // MAINWINDOW_H
