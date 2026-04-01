#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "translate.h"
#include "grammar.h"
#include "leftmenu.h"

#include <QMainWindow>
#include <QStackedWidget>
#include <QMessageBox>


QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();


private slots:
    void showLevelDialogB() {
        if (indexDifficulty == 0) {
            return;
        }
        QAction *action = qobject_cast<QAction*>(sender());
        if (action) {
            QMessageBox::information(this, "Level Selection", "You selected: " + action->text());
        }
        indexDifficulty = 0;
        translateW->setupDiffculty(0);
    }
    void showLevelDialogI() {
        if (indexDifficulty == 1) {
            return;
        }
        QAction *action = qobject_cast<QAction*>(sender());
        if (action) {
            QMessageBox::information(this, "Level Selection", "You selected: " + action->text());
        }
        indexDifficulty = 1;
        translateW->setupDiffculty(1);
    }
    void showLevelDialogA() {
        if (indexDifficulty == 2) {
            return;
        }
        QAction *action = qobject_cast<QAction*>(sender());
        if (action) {
            QMessageBox::information(this, "Level Selection", "You selected: " + action->text());
        }
        indexDifficulty = 2;
        translateW->setupDiffculty(2);
    }
private:
    //Ui::MainWindow *ui;
    Translate* translateW;
    Grammar* grammarW;
    LeftMenu* leftW;
    QMenuBar* menuBar;

    QStackedWidget* stackWidgets;
    int indexDifficulty = 0;
    QWidget* hello;

};
#endif // MAINWINDOW_H
