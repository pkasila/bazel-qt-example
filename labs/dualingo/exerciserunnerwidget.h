#ifndef EXERCISERUNNERWIDGET_H
#define EXERCISERUNNERWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QRadioButton>
#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QButtonGroup>
#include "taskdata.h" 

class ExerciseRunnerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExerciseRunnerWidget(QWidget *parent = nullptr);

    void setupForTask(const TaskData& task);
    QString getTranslationAnswer() const;
    int getGrammarAnswerIndex() const; 
    void clearInputs();
    void showFeedback(const QString& message, bool isCorrect); 

signals:
    void submitClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *promptLabel;
    QLabel *feedbackLabel; 
    
    QLineEdit *translationEdit;
    
    QGroupBox *grammarGroup;
    QVBoxLayout *radioLayout; 
    QList<QRadioButton*> radioButtons;
    QButtonGroup *grammarButtonGroup;

    QPushButton *submitButton;

    ExerciseType currentType;

    void clearRadioButtons();
};

#endif 
