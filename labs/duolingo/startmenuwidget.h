#ifndef STARTMENUWIDGET_H
#define STARTMENUWIDGET_H

#include <QWidget>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class StartMenuWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StartMenuWidget(QWidget *parent = nullptr);

    signals:
        void translationExerciseRequested();
    void grammarExerciseRequested();

private:
    QVBoxLayout *mainLayout;
    QLabel *welcomeLabel;
    QPushButton *translationButton;
    QPushButton *grammarButton;
};

#endif // STARTMENUWIDGET_H