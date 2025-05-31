#ifndef EXERCISESETUPWIDGET_H
#define EXERCISESETUPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>

class ExerciseSetupWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ExerciseSetupWidget(QWidget *parent = nullptr);

signals:
    void startTranslationChallengeClicked();
    void startGrammarChallengeClicked();

private:
    QVBoxLayout *mainLayout;
    QLabel *titleLabel;
    QPushButton *translationButton;
    QPushButton *grammarButton;
};

#endif // EXERCISESETUPWIDGET_H
