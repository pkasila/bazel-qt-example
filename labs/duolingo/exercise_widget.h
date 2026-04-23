#ifndef EXERCISE_WIDGET_H
#define EXERCISE_WIDGET_H
#include <QLabel>
#include <QLineEdit>
#include <QWidget>
#include <QPushButton>

class ExerciseWidget : public QWidget {
    Q_OBJECT

public:
    explicit ExerciseWidget(QWidget *parent = nullptr);

    virtual bool ValidateAnswer() = 0;

    virtual ~ExerciseWidget() = default;

protected slots:
    void OnSubmit();

signals:
    void ExerciseAnswered(bool correct);

private:
    friend class TranslationExercise;

    friend class GrammarExercise;

    friend class MainWindow;

    void ShowMessageBox();

    virtual void LoadQuestion() = 0;

    QLabel *question_label_;
    QPushButton *submit_button_;
    QString answer_;
};

#endif
