#ifndef TICKET_REVIEW_APP_H
#define TICKET_REVIEW_APP_H

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
#include <QGridLayout>
#include <QMap>
#include <vector>
#include <stack>

class TicketReviewApp : public QMainWindow
{
    Q_OBJECT

public:
    TicketReviewApp(QWidget *parent = nullptr);
    ~TicketReviewApp();

private slots:
    void onTicketCountChanged(int count);
    void onViewItemClicked(QListWidgetItem *item);
    void onViewItemDoubleClicked(QListWidgetItem *item);
    void onNameEditReturnPressed();
    void onStatusChanged(int index);
    void onNextQuestionClicked();
    void onPreviousQuestionClicked();

private:
    struct TicketInfo {
        QString name;
        int status; // 0 - default (gray), 1 - yellow, 2 - green
    };
    
    std::vector<TicketInfo> tickets;
    std::stack<int> historyStack;
    int currentTicketIndex;
    
    QSpinBox *countSpinBox;
    QListWidget *viewListWidget;
    QGroupBox *questionViewGroupBox;
    QLabel *numberLabel;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *statusComboBox;
    QPushButton *nextQuestionButton;
    QPushButton *previousQuestionButton;
    QProgressBar *totalProgressBar;
    QProgressBar *greenProgressBar;
    
    void setupUI();
    void updateView();
    void updateQuestionView(int ticketIndex);
    void updateProgressBars();
    void clearHistory();
    void addToHistory(int ticketIndex);
    int getRandomUnfinishedTicket() const;
    void setTicketStatus(int ticketIndex, int newStatus);
    QString getStatusColor(int status) const;
    void updateViewItemColor(int ticketIndex);
    int calculateTotalProgress() const;
    int calculateGreenProgress() const;
};

#endif // TICKET_REVIEW_APP_H