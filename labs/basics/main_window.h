//
// Created by blalex on 3/12/25.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H

#include <QMainWindow>

enum class TicketStatus : std::uint8_t { Default, Yellow, Green };

struct Ticket {
    QString name{};
    TicketStatus status = TicketStatus::Default;
    QString description{};
};

QT_BEGIN_NAMESPACE
class QSpinBox;
class QListWidget;
class QGroupBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
class QTextEdit;
class QListWidgetItem;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);

   private:
    void CreateWidgets() const;
    void SetupLayout();

    void UpdateTickets(size_t ticket_count);
    void OnItemClicked(const QListWidgetItem* item);
    void ChangeName();
    void LoadTicket(int index);
    void OnItemDoubleClicked(const QListWidgetItem* item);
    void ChangeType(int index);
    void UpdateProgressBar() const;
    void NextTicket();
    void PreviousTicket();
    void ChangeText();
    void SaveTickets();
    void LoadTickets();

    QSpinBox* count_{};
    QListWidget* view_{};
    QProgressBar* total_progress_{};
    QProgressBar* green_progress_{};
    QGroupBox* question_view_{};
    QLabel* number_label_{};
    QLabel* name_label_{};
    QLineEdit* name_edit_{};
    QComboBox* status_{};
    QPushButton* next_button_{};
    QPushButton* previous_button_{};
    QPushButton* load_button_{};
    QPushButton* save_button_{};
    QLabel* description_label_{};
    QTextEdit* description_{};

    std::vector<Ticket> tickets_{};
    std::vector<size_t> tickets_index_history_{};
    size_t current_ticket_ = 0;
    size_t ticket_count_ = 0;
    bool user_change_ = true;
    const QString filepath_ = "labs/basics/tickets.txt";
};

#endif  // MAIN_WINDOW_H
