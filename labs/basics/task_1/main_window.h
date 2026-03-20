#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include "ticket.h"

#include <QMainWindow>
#include <stack>
#include <unordered_set>

constexpr auto kDefaultWidth = 1200;
constexpr auto kDefaultHeight = 600;

constexpr auto kDefaultTicketsCount = 30;

QT_BEGIN_NAMESPACE
class QLabel;
class QComboBox;
class QPushButton;
class QSpinBox;
class QListWidget;
class QLineEdit;
class QGroupBox;
class QProgressBar;
class QListWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();

private slots:
    void CountChanged(int count);
    void ViewItemClicked(const QListWidgetItem* item);
    void ViewItemDoubleClicked(const QListWidgetItem* item) const;
    void ViewCurrentItemChanged(QListWidgetItem* current, const QListWidgetItem* previous);
    void TicketNameChanged(int index, const QString& name) const;
    void TicketStatusChanged(int index, Ticket::Status status, Ticket::Status prev_status);
    void StatusChanged(int index) const;
    void NameEdited() const;
    void PreviousClicked();
    void NextClicked();

private: // NOLINT(readability-redundant-access-specifiers)
    QSpinBox* count_;
    QListWidget* view_;
    QGroupBox* question_view_;
    QLabel* number_;
    QLabel* name_;
    QLineEdit* name_edit_;
    QComboBox* status_;
    QPushButton* next_question_;
    QPushButton* previous_question_;
    QProgressBar* total_progress_;
    QProgressBar* green_progress_;

    std::vector<std::shared_ptr<Ticket> > tickets_;
    std::unordered_set<std::shared_ptr<Ticket> > non_green_tickets_;
    std::shared_ptr<Ticket> selected_ticket_ = nullptr;
    std::stack<std::shared_ptr<Ticket> > previous_tickets_;
    int tickets_count_ = kDefaultTicketsCount;
    bool previous_button_click_ = false;

    void AllocateTickets(int count);
    void SelectTicket(int index);
    void Connect();

    [[nodiscard]] std::shared_ptr<Ticket> GetRandomTicket() const;
};

#endif