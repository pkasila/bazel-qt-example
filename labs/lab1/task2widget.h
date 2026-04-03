#ifndef TASK2WIDGET_H
#define TASK2WIDGET_H

#include <QLabel>
#include <QProgressBar>
#include <QPushButton>
#include <QRadioButton>
#include <QSlider>
#include <QSpinBox>
#include <QTextEdit>
#include <QWidget>

class Task2Widget : public QWidget {
    Q_OBJECT

   public:
    explicit Task2Widget(QWidget* parent = nullptr);

   private slots:
    void updateChances();
    void showVerdict();

   private:
    QLabel* catImageLabel;
    QSpinBox* missedMthClasses;
    QRadioButton* didLabsMyself;
    QRadioButton* copiedFromGithub;
    QSlider* stressSlider;
    QPushButton* calculateButton;
    QProgressBar* expulsionChance;
    QTextEdit* verdictText;
};

#endif
