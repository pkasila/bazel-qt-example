#ifndef TRANSLATIONEXERCISEWIDGET_H
#define TRANSLATIONEXERCISEWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit> 
#include <QPushButton>


struct TranslationData;

class TranslationExerciseWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TranslationExerciseWidget(QWidget *parent = nullptr);
    void setExerciseData(const TranslationData& data);
    void showFeedback(const QString& message, bool isCorrect);

    signals:
        void answerSubmitted(const QString& answer);

private:
    QVBoxLayout *mainLayout;
    QLabel *instructionLabel;
    QLabel *sentenceLabel;
    QLineEdit *answerInput; 
    QPushButton *submitButton;
    QLabel *feedbackLabel;
    QPushButton *audioButton; 
};

#endif // TRANSLATIONEXERCISEWIDGET_H