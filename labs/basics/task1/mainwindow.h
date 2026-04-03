#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QApplication>
#include <QMainWindow>
#include <QListWidget>
#include <QPushButton>
#include <QSpinBox>
#include <QProgressBar>
#include <QLabel>
#include <QColor>
#include <QTimer>





class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    friend class TicketDialog;

private slots:
    void RebuildList();
    void NextTicket();
    void PreviousTicket();
    void ChooseRandomTicket();
    void OnItemClicked(QListWidgetItem *item);
    void OnItemDoubleClicked(QListWidgetItem *item);
    void ChangeStatus(QListWidgetItem *item);
    void OpenDialog();

private:
    QProgressBar *progBarStarted_;
    QProgressBar *progBarTotal_;
    QSpinBox *spinBoxTicketsNum_;
    QListWidget *listWidget_;
    QPushButton *buttonNext_;
    QPushButton *buttonPrev_;
    QLabel *labelTotal_;
    QLabel *labelStarted_;
    int cnt_ = 0;
    int cntStarted_ = 0;
    int cntTotal_ = 0;
    int currentTicket_ = -1;
    std::vector<int>ticketsHistory_;
    std::vector<int>status_;
    QTimer *singleClickTimer_;
    QListWidgetItem *lastClikedItem_;
};


#endif // MAINWINDOW_H
