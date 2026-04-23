#ifndef TRANSLATION_EXERCISE_H
#define TRANSLATION_EXERCISE_H

#include <QtSql/QSqlDatabase>

#include "exercise_widget.h"

class TranslationExercise : public ExerciseWidget {
    Q_OBJECT

public:
    TranslationExercise(int difficult_level, const QSqlDatabase &db, QWidget *parent = nullptr);

    bool ValidateAnswer() override;

    ~TranslationExercise() override;

private:
    void LoadQuestion() override;

    void OnParentResized(double w_ratio, double h_ratio) const;

    QLineEdit *answer_edit_;
    int difficult_level_{};
    int index_{};
    friend class MainWindow;
    QSqlDatabase db_;
    std::vector<QString> difficulties_{
        "translation_questions_easy", "translation_questions_medium", "translation_questions_hard"
    };
    std::vector<QString> difficulty_indices_table_names_{
        "unanswered_translation_easy_questions", "unanswered_translation_medium_questions",
        "unanswered_translation_hard_questions"
    };
};

#endif //TRANSLATION_EXERCISE_H
