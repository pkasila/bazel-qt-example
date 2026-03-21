#pragma once

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QVector>

enum class TicketStatus { kNotStart, kRepeat, kMastered };

struct Ticket {
    QString name;
    TicketStatus status;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() = default;

   private slots:
    void onCountChanged(int count);
    void onNextClicked();
    void onPrevClicked();
    void onClearClicked();

    void onListRowChanged(int row);
    void onStatusIndexChanged(int index);
    void onItemDoubleClicked(QListWidgetItem *item);

    void onNameEditReturnPressed();

   private:
    QVector<Ticket> tickets;
    QVector<int> history;
    int currentTicketId = -1;
    int historyIndex = -1;
    
    //Left
    QSpinBox *countSpinBox;
    QListWidget *ticketsListWidget;
    QProgressBar *started;
    QProgressBar *mastered;

    //Right
    QGroupBox *questionGroupBox;
    QLabel *numberLabel;   
    QLabel *nameLabel; 
    QLineEdit *nameEdit;
    QComboBox *statusEdit;
    QPushButton *next;
    QPushButton *prev;
    QPushButton *clear;

    void updateProgressBars(); 
};