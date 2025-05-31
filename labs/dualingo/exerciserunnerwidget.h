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
#include "taskdata.h" // For ExerciseType

class ExerciseRunnerWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExerciseRunnerWidget(QWidget *parent = nullptr);

    void setupForTask(const TaskData& task);
    QString getTranslationAnswer() const;
    int getGrammarAnswerIndex() const; // Returns selected radio button ID or -1
    void clearInputs();
    void showFeedback(const QString& message, bool isCorrect); // Shows temporary feedback

signals:
    void submitClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *promptLabel;
    QLabel *feedbackLabel; // For "Correct!" / "Incorrect!"

    // Translation specific
    QLineEdit *translationEdit;

    // Grammar specific
    QGroupBox *grammarGroup;
    QVBoxLayout *radioLayout; // Inside grammarGroup
    QList<QRadioButton*> radioButtons;
    QButtonGroup *grammarButtonGroup;

    QPushButton *submitButton;

    ExerciseType currentType;

    void clearRadioButtons();
};

#endif // EXERCISERUNNERWIDGET_H
