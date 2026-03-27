#ifndef TRANSLATIONVIEW_H
#define TRANSLATIONVIEW_H

#include "TaskDifficulty.h"

#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QWidget>

class TranslationView : public QWidget {
    Q_OBJECT
   public:
    explicit TranslationView(QWidget* parent = nullptr);

    void SetTask(TaskDifficulty difficulty, const int& taskId);

   signals:
    void Accepted();
    void WrongAnswer();

   private slots:
    void Submit();

   private:
    QLabel* question_label_ = nullptr;
    QLineEdit* input_field_ = nullptr;
    QPushButton* submit_button_ = nullptr;
    QString correct_answer_;
};

#endif  // TRANSLATIONVIEW_H
