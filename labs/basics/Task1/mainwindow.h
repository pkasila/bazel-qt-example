#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QApplication>
#include <QSpinBox>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QMap>
#include <QGridLayout>

enum class TicketStatus {
    Gray,
    Yellow,
    Green
};

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

private:
    Ui::MainWindow *ui;
    QSpinBox *CountSpinBox;
    QListWidget *TicketList;
    QPushButton *NextButton;
    QPushButton *PrevButton;
    QGroupBox *QuastionView;
    QProgressBar *GreenProgress;
    QProgressBar *TotalProgress;
    QLineEdit *TicketNameEdit;
    QLabel *TicketNumberLabel;
    QComboBox *statusDropdown;
    int CurrentTicket;
    QMap<QListWidgetItem*, TicketStatus> TicketStatuses;
    QVector<QListWidgetItem*> nonGreenTickets;
    QList<QListWidgetItem*> History;
    void UpdateNonGreenTickets();
    int HistoryIndex=-1;
    void setupUI();

private slots:
    void InitializeTickets(int count);
    void UpdateTicketCount();
    void UpdateProgress();
    void ChangeTicketStatus(QListWidgetItem *item);
    void UpdateTicketInfo(QListWidgetItem *item);
    void ChangeTicketName();
    void ChangeTicketStatusFromDropdown();
    void NextTicket();
    void PrevTicket();


};
#endif
