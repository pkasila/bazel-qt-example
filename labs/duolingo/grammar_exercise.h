#ifndef GRAMMAR_EXERCISE_H
#define GRAMMAR_EXERCISE_H
#include <qradiobutton.h>
#include <QtSql/QSqlDatabase>
#include "exercise_widget.h"


class GrammarExercise : public ExerciseWidget {
    Q_OBJECT

public:
    GrammarExercise(int difficult_level, const QSqlDatabase &db, QWidget *parent = nullptr);

    bool ValidateAnswer() override;

    ~GrammarExercise() override;

public slots:
    void ShowHintBox();

private:
    void LoadQuestion() override;

    void OnParentResized(double w_ratio, double h_ratio) const;

    int difficult_level_{};
    int index_{};
    int answer_index_;
    friend class MainWindow;
    QRadioButton *option_1_;
    QRadioButton *option_2_;
    QRadioButton *option_3_;
    QRadioButton *option_4_;
    QButtonGroup *options_group_;
    QString rule_name_;
    QString hint_;
    QSqlDatabase db_;
    std::vector<QString> difficulties_{
        "grammar_questions_easy", "grammar_questions_medium", "grammar_questions_hard"
    };
    std::vector<QString> difficulty_indices_table_names_{
        "unanswered_grammar_easy_questions", "unanswered_grammar_medium_questions",
        "unanswered_grammar_hard_questions"
    };
};

#endif //GRAMMAR_EXERCISE_H
