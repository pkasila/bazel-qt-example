#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qbuttongroup.h>
#include <qcheckbox.h>
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

    MainWindow();

    QLineEdit* line_edit;
    QCheckBox* check1, *check2;
    QLabel *label;
    QPushButton *button;
    QListWidget* listWidget1;
    QTableWidget* table;

public slots:
    void clicked();
    void edited();
    void buttonpress();
    void clear();
};
#endif // MAINWINDOW_H
