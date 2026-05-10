#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include <QVector>

class QComboBox;
class QLabel;
class QLineEdit;
class QProgressBar;
class QPushButton;
class QSpinBox;
class QTableWidget;

class MainWindow : public QMainWindow {
   public:
    explicit MainWindow(QWidget* parent = nullptr);
    MainWindow(const MainWindow& other) = delete;
    MainWindow(MainWindow&& other) = delete;
    MainWindow& operator=(const MainWindow& other) = delete;
    MainWindow& operator=(MainWindow&& other) = delete;
    ~MainWindow() override = default;

   private:
    enum class TicketStatus : uint8_t {
        kDefault = 0,
        kYellow = 1,
        kGreen = 2,
    };

    struct Ticket {
        QString name;
        TicketStatus status = TicketStatus::kDefault;
    };

    void OnTicketCountChanged(int value);
    void OnTicketCellClicked(int row, int column);
    void OnTicketCellDoubleClicked(int row, int column);
    void OnStatusComboBoxChanged(int index);
    void OnNameEditReturnPressed();
    void OnNextQuestionClicked();
    void OnPreviousQuestionClicked();

    void SetupUi();
    void SetupConnections();
    void RebuildTickets(int count);
    void UpdateTableRow(int index);
    void UpdateQuestionView();
    void ClearQuestionView();
    void SelectTicket(int index, bool push_to_history);
    void SetTicketStatus(int index, TicketStatus new_status);
    void UpdateProgressBars();

    QSpinBox* count_spin_box_ = nullptr;
    QTableWidget* view_table_widget_ = nullptr;

    QLabel* number_value_label_ = nullptr;
    QLabel* name_value_label_ = nullptr;
    QLineEdit* name_line_edit_ = nullptr;
    QComboBox* status_combo_box_ = nullptr;

    QPushButton* next_question_button_ = nullptr;
    QPushButton* previous_question_button_ = nullptr;

    QProgressBar* total_progress_bar_ = nullptr;
    QProgressBar* green_progress_bar_ = nullptr;

    QVector<Ticket> tickets_;
    QVector<int> history_;
    int current_index_ = -1;
};

#endif  // MAINWINDOW_H