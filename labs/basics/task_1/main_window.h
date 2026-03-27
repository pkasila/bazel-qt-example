#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qmainwindow.h>

#include "Ticket.h"
#include <set>
#include <stack>

class Ticket;
QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QComboBox;
class QTableWidget;
class QLineEdit;
class QProgressBar;
class QMenu;
class QSpinBox;
class QPushButton;

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

    void updateRowColor(QColor color);

    void updateTicket(int row);

    void updateProgress();

private slots:

    void chooseTicket(int row);

    void onCellClicked(int row);

    void updateTable();

    void getRandomTicket();

    void getPreviousTicket();

    void onCellDoubleClicked();

    void statusChanged();

    void onEnterPressed();


private:
    void createActions();

    // widgets
    QTableWidget *view_;
    QLabel *number_label_;
    QLabel *name_label_;
    QComboBox *status_box_;
    QLineEdit *name_edit_;
    QProgressBar *total_progress_;
    QProgressBar *green_progress_;
    QSpinBox *count_box_;
    QPushButton *next_question_button_;
    QPushButton *previous_question_button_;

    // selected items
    int selected_row_;
    int selected_column_;
    Ticket selected_ticket_;

    // total progress support field
    int grey_count_;

    // containers
    std::vector<Ticket> tickets_;
    std::set<Ticket> revision_tickets_;
    std::stack<Ticket> tickets_stack_;
};

#endif
