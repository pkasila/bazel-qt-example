#ifndef EXAM_REVIEWER_H
#define EXAM_REVIEWER_H

#include <QApplication>
#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QListWidgetItem>
#include <vector>

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

class ExamReviewer : public QMainWindow {
    Q_OBJECT

private:
    // Виджеты
    QSpinBox* countSpinBox;
    QListWidget* ticketList;
    QGroupBox* questionView;
    QLabel* numberLabel;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QComboBox* statusCombo;
    QPushButton* nextButton;
    QPushButton* previousButton;
    QProgressBar* totalProgress;
    QProgressBar* greenProgress;
    
    // Данные
    std::vector<Ticket> tickets;
    int currentTicketIndex;
    std::vector<int> history;
    
    // Layouts
    QWidget* centralWidget;
    QHBoxLayout* mainLayout;
    QVBoxLayout* leftLayout;
    QVBoxLayout* rightLayout;
    QVBoxLayout* questionLayout;
    QHBoxLayout* buttonLayout;
    QHBoxLayout* progressLayout;

public:
    ExamReviewer(QWidget *parent = nullptr);
    ~ExamReviewer();

private slots:
    void onTicketCountChanged(int count);
    void onTicketClicked(QListWidgetItem* item);
    void onTicketDoubleClicked(QListWidgetItem* item);
    void onNameChanged();
    void onStatusChanged();
    void onNextQuestion();
    void onPreviousQuestion();
    void updateProgress();

private:
    void setupUI();
    void updateTicketList();
    void updateQuestionView();
    void updateTicketItem(int index);
    TicketStatus getTicketStatus(const QString& statusText) const;
    QString getStatusText(TicketStatus status) const;
    QColor getStatusColor(TicketStatus status) const;
    int selectRandomUncompletedTicket() const;
};

#endif // EXAM_REVIEWER_H
