#ifndef LEFTMENU_H
#define LEFTMENU_H

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QSoundEffect>

class LeftMenu : public QWidget
{
    Q_OBJECT
public:
    explicit LeftMenu(QWidget *parent = nullptr);
    int globalScore = 0;
    void updateLabel(int num);

    void endTr() {
        translate->setEnabled(false);
    }
    void endGr() {
        grammar->setEnabled(false);
    }
    void stTr() {
        translate->setEnabled(true);
    }
    void stGr() {
        grammar->setEnabled(true);
    }

signals:

    void trButClicked();
    void grButClicked();

public slots:



private:
    QPushButton* translate;
    QPushButton* grammar;
    QLabel* globalScoreL;
    QSoundEffect* click;
    void connections();
};

#endif // LEFTMENU_H
