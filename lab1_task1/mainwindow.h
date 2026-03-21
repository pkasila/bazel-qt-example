#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QListWidget;
class QListWidgetItem;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
QT_END_NAMESPACE

struct Ticket {
    int number;
    QString name;
    int status;
};

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(QWidget *parent = nullptr);
private:
    QVector<Ticket> tickets;
    QVector<int> history;
    int currentIndex = -1;

    QSpinBox *countSpinBox;
    QListWidget *ticketList;

    QLabel *numberLabel;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *statusCombo;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;

    void rebuildTickets(int count);
    void refreshList();
    void updateListItem(int index);
    void showTicket(int index);
    void updateButtons();
    void updateProgressBars();
    void setTicketStatus(int index, int status);
    int randomAvailableTicket() const;
    
};

#endif