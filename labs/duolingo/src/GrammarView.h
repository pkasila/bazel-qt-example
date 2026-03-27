#ifndef GRAMMARVIEW_H
#define GRAMMARVIEW_H

#include "TaskDifficulty.h"

#include <QButtonGroup>
#include <QLabel>
#include <QRadioButton>
#include <QWidget>

class GrammarView : public QWidget {
    Q_OBJECT
   public:
    GrammarView(QWidget* parent = nullptr);

    void SetTask(TaskDifficulty, const int&);

   signals:
    void Accepted();
    void WrongAnswer();

   private slots:
    void Submit();

   private:
    QLabel* question_ = nullptr;
    std::vector<QRadioButton*> options_{};
    QButtonGroup* option_group_ = nullptr;
    int answer_{};
};

#endif  // GRAMMARVIEW_H
