#ifndef MULTICORRECTGRAMMAREXERCISE_H
#define MULTICORRECTGRAMMAREXERCISE_H

#include <QWidget>
#include <QLabel>
#include <QCheckBox>
#include <QPushButton>
#include <QVector>
#include <QGroupBox>
#include <QSet>
#include <QVBoxLayout>
#include "exercisedata.h" 

class MultiCorrectGrammarExerciseWidget : public QWidget {
    Q_OBJECT

public:
    explicit MultiCorrectGrammarExerciseWidget(QWidget *parent = nullptr);
    void setTask(const MultiCorrectGrammarTask& task);
    QSet<int> getSelectedIndices() const;
    void showCorrectAnswers(const QSet<int>& correctIndices, const QStringList& options);
    void clearSelection();

signals:
    void submitClicked();

private:
    QLabel *instructionLabel;
    QLabel *questionLabel;
    QGroupBox *optionsGroup;
    QVBoxLayout *optionsLayout;
    QVector<QCheckBox*> checkBoxes;
    QPushButton *submitButton;
    QLabel *feedbackLabel;
};

#endif