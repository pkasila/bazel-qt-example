#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "ticket.h"

#include <QMainWindow>
#include <QSpinBox>
#include <QTableWidget>
#include <vector>
#include <QModelIndex>
#include <QTimer>
#include <QProgressBar>

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
    void updateProgressBar();

private slots:
    void updateTable(int num);

    void onCellClicked();
    void aboutClick(int row);

    void onCelldoubleClicked(QModelIndex ind);

private:
    //Ui::MainWindow *ui;
    QSpinBox *box;
    QTableWidget *list;
    std::vector<Ticket> tickets;
    void connections();
    QTimer* timer;
    int last = -1;
    long long clicks = 0;
    std::vector<Ticket> history;
    QProgressBar* total_bar;
    QProgressBar* green_bar;

};

#endif // MAINWINDOW_H
