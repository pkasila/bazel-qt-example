#ifndef GRAMMAREXERCISEWIDGET_H
#define GRAMMAREXERCISEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QGroupBox>
#include <QRadioButton>
#include <QPushButton>
#include <QButtonGroup>

struct GrammarData;

class GrammarExerciseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GrammarExerciseWidget(QWidget *parent = nullptr);
    void setExerciseData(const GrammarData& data);
    void showFeedback(const QString& message, bool isCorrect);

    signals:
        void answerSubmitted(const QString& answer);

    private slots:
        void onSubmitClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *instructionLabel;
    QLabel *sentenceLabel; 
    QGroupBox *optionsGroupBox; 
    QVBoxLayout *optionsLayout; 
    QButtonGroup *optionsButtonGroup;
    QPushButton *submitButton;
    QLabel *feedbackLabel;
    QPushButton *audioButton; 
};

#endif // GRAMMAREXERCISEWIDGET_H