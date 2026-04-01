#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QList>
#include <vector>
#include <stack>
#include "tickdialog.h"
#include <QListWidget>
#include <QLayout>
#include <QLabel>
#include <QSpinBox>
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
private slots:
    void SingleClickOnTicket(QListWidgetItem*);
    void DoubleClickOnTicket(QListWidgetItem*);
    void NextTicketButton();
    void PrevTicketButton();
    void UpdateTickets(int newCount);
    void onNameChanged(int index, const QString& new_name);
    void onStatusChanged(int index, int new_status);
    void onHintChanged(int index, const QString& new_hint);
    void LoadTickets(const QString& filename);
    void SaveTickets(const QString& filename);
private:
    Ui::MainWindow *ui;
    QVBoxLayout* main_layout_;
    std::vector<Ticket> tickets_;
    QSpinBox* cnt_;
    QListWidget* list_;
    QPushButton* next_tick_;
    QPushButton* prev_tick_;
    QLabel* ticket_num_;
    QLabel* ticket_text_;
    QProgressBar* tot_pr_bar_;
    QProgressBar* learn_pr_bar_;
    QLabel* tot_pr_label_;
    QLabel* learn_pr_label_;
    std::stack<int> tick_stack_;
    void GetNextTicket();
    int GetPref();
    void UpdateTicketInList(int index);
    void UpdateProgressBars();
};
#endif // MAINWINDOW_H
