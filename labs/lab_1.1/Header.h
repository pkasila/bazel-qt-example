#ifndef LAB_1_1_HEADER_H
#define LAB_1_1_HEADER_H

#include <QtWidgets/QMainWindow>
#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QGridLayout>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QTableWidget>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QApplication>
#include <QtCore/QRandomGenerator>
#include <QtCore/QVector>
#include <QtGui/QColor>

enum class TicketStatus {
    Default,
    Yellow,
    Green
};

struct Ticket {
    int number;
    QString name;
    TicketStatus status;
    
    Ticket(int num) : number(num), name(QString("Билет %1").arg(num)), status(TicketStatus::Default) {}
};

class ExamApp : public QMainWindow {
public:
    ExamApp(QWidget *parent = nullptr);
    ~ExamApp();

private:
    void onTicketCountChanged(int count);
    void onTicketClicked(int row, int column);
    void onTicketDoubleClicked(int row, int column);
    void onNameChanged();
    void onStatusChanged();
    void onNextQuestion();
    void onPreviousQuestion();

private:
    void setupUI();
    void updateTicketView();
    void updateQuestionView();
    void updateProgressBars();
    void selectTicket(int index);
    QColor getTicketColor(TicketStatus status);
    QVector<int> getAvailableTickets();
    
    QWidget *centralWidget;
    QVBoxLayout *mainLayout;
    QHBoxLayout *contentLayout;

    QSpinBox *ticketCount;
    QTableWidget *ticketView;
    
    QGroupBox *questionGroup;
    QVBoxLayout *questionLayout;
    QLabel *ticketNumber;
    QLabel *ticketName;
    QLineEdit *nameEdit;
    QComboBox *statusCombo;
    QHBoxLayout *buttonLayout;
    QPushButton *nextQuestion;
    QPushButton *previousQuestion;
    
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;
    
    QVector<Ticket> tickets;
    int currentTicketIndex;
    QVector<int> history;
    int historyIndex;
};

#endif // LAB_1_1_HEADER_H
