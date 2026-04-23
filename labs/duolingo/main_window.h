#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QLabel>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSoundEffect>
#include <QStackedWidget>
#include <QVBoxLayout>

#include "translation_exercise.h"
#include <vector>
#include <QtSql/QSqlDatabase>


class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

    ~MainWindow();

private slots:
    void OnTranslation();

    void OnGrammar();

    void OnExerciseAnswered(bool ok);

    void OpenDB();

signals:
    void ResizeEvent(double w_ratio, double h_ratio);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void SetupMenu();

    void SetupUI();

    void ApplyStyles() const;

    void OnSelectDifficulty();

    void UpdateEXP();

    void SaveUserData();

    void CallHint();

    void StartTimer();

    QWidget *central_widget_;
    QLabel *level_circle_;
    QLabel *learn_german_label_;
    QProgressBar *exp_bar_;
    QLabel *exp_text_;
    QPushButton *translation_button_;
    QPushButton *grammar_button_;
    QStackedWidget *stacked_widget_;
    QSoundEffect complete_effect_;
    QVBoxLayout *main_layout_;
    QLabel *timer_label_;
    int difficult_level_ = 0;
    int level_ = 1;
    int exp_ = 0;
    int necessary_exp_ = 10;
    int correct_answer_ = 0;
    bool not_in_task_ = true;
    QSqlDatabase db_;
};

#endif //MAIN_WINDOW_H_
