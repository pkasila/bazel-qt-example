#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qbuttongroup.h>
#include <qcombobox.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qlistwidget.h>
#include <QMainWindow>
#include <qprogressbar.h>
#include <QPushButton>
#include <qspinbox.h>
#include <qtablewidget.h>
#include <random>
#include <set>
#include <stack>

QT_BEGIN_NAMESPACE

QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:

    struct ticket {
        int number;
        QString name;
        int state;
    };

    MainWindow();

    QSpinBox *count;
    QListWidget *view;
    QPushButton *next, *previous;
    QProgressBar *progresstotal, *progressgreen;
    QGroupBox *question_view;
    QLabel * number;
    QLabel * name;
    QLineEdit* line_edit;
    QComboBox *status;

    std::vector<ticket> tickets;
    int active = 0;

    std::stack<int> history;
    std::set<int> def;
    std::set<int> ng;

    std::mt19937 gen;

    void setItemColor (int ind, int col);

    public slots:

    void changeCount ();
    void doubleClick (QListWidgetItem*);
    void singleClick (QListWidgetItem*);
    void changeName();
    void changeColor();
    void tonext();
    void toprev();
    void updateBars();
    void currentChanged();
};
#endif // MAINWINDOW_H
