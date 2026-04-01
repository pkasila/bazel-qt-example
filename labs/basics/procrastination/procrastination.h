#ifndef PROCRASTINATION_H
#define PROCRASTINATION_H

#include <QApplication>
#include <QBrush>
#include <QColor>
#include <QComboBox>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSpinBox>
#include <QTableWidget>
#include <QVBoxLayout>
#include <ctime>
#include <deque>

enum class TicketState { Default, InProgress, Done };

class Ticket {
   public:
    Ticket() = default;

    explicit Ticket(const size_t&);

    void setNumber(const size_t&);

    [[nodiscard]] const size_t& getNumber() const;

    void setName(const QString& name);

    [[nodiscard]] const QString& getName() const;

    void setState(const TicketState& state);

    [[nodiscard]] const TicketState& getState() const;

    void nextState();

   private:
    size_t number_ = 1;
    QString name_ = QString("Билет 1");
    TicketState state_ = TicketState::Default;
};

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);

   private slots:
    void countChanged();

    void resetProgress();

    void ticketChosen(int, int);

    void toNextTicketState(int, int);

    void updateTicketName(Ticket&, const QString&) const;

    void updateTicketState(Ticket&, TicketState);

    void updateProgressBars(const size_t&, const size_t&) const;

    void choosePrevTicket();

    void chooseNextTicket();

   signals:
    void progressChanged(const size_t&, const size_t&);

   private:
    void resetView() const;

    static void deleteLayout(QLayout*);

    void showTicketView(Ticket&, bool forward = true);

    void hideTicketView(bool forwrd = true);

    void stateChanged(const TicketState&, const TicketState&);

    QSpinBox* count_{};
    QPushButton* reset_button_{};
    QTableWidget* view_{};
    QGroupBox* ticket_view_{};
    QPushButton* prev_ticket_button_{};
    QPushButton* next_ticket_button_{};
    QProgressBar* total_progress_{};
    QProgressBar* green_progress_{};

    std::vector<Ticket> tickets_{};
    std::deque<size_t> seen_tickets_ind_{};
    std::vector<size_t> undone_tickets_ind_{};

    Ticket* chosen_ticket_ = nullptr;

    size_t seen_tickets_ = 0;
    size_t done_tickets_ = 0;

    const size_t k_max_roll_back_ = 1024;

    QRandomGenerator random_{};
};

#endif  // PROCRASTINATION_H
