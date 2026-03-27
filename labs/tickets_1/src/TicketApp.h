// TicketApp.h
#ifndef TICKETAPP_H
#define TICKETAPP_H
//Qt libraries
#include <QWidget>
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QEvent>
#include <QSpinBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QStack>
#include <QComboBox>
#include <QRandomGenerator>
#include <QProgressBar>
//Custom libraries
#include "ProjectConstants.h"
#include "AddTicketsDialog.h"
#include "InputUtils.h"
#include "TicketTableWidget.h"
#include "EditableLabel.h"
#include "StatusComboBox.h"
#include "TicketTextEdit.h"
#include "CursorChangingButton.h"
#include "TicketsProgressBar.h"

class TicketApp : public QMainWindow {
    Q_OBJECT

public:
    explicit TicketApp(QWidget *parent = nullptr);

private:
    //---Initializing UI---
    void setUpUi();
    void initTickets();
    void initTicketView();
    void initProgressBars();
    void activateTicketView();
    void showAddTicketsDialog();

    //---Handling tickets---
    void selectTicket(int row);
    void selectRandomTicket();
    void addCurrentTicketToUndoTicketStack();

    //---Connectors---
    void setUpConnections();
    void connectCurrentTicketNameToCurrentTicket();
    void connectCurrentTicketStatusBoxToCurrentTicket();
    void connectCurrentTicketTextToCurrentTicket();

private slots:
    void onTableCellClicked(int row, int);
    void onTableCellDoubleClicked(int row, int);
    void selectPrevTicket();

private:
    //---Tickets control unit---
    QLabel *numberOfTickets;
    QSpinBox *ticketCountInput;
    CursorChangingButton *submitButton;
    CursorChangingButton *addTicketsButton;
    //---Progress-bars---
    TicketsProgressBar *totalProgressBar;
    TicketsProgressBar *greenProgressBar;
    //---Table of tickets---
    TicketTableWidget *ticketTable;
    //---Ticket view block---
    QGroupBox *ticketView;
    QLabel *currentTicketNumber;
    EditableLabel *currentTicketName;
    StatusComboBox *currentTicketStatusBox;
    TicketTextEdit *currentTicketText;
    CursorChangingButton *prevTicketButton;
    CursorChangingButton *randomTicketButton;

private:
    TicketCell *prevTicket;
    TicketCell *currentTicket;


    QStack<TicketCell*> undoTicketStack;
    bool isTicketViewActivated;
};

#endif // TICKETAPP_H





