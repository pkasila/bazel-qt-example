#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <vector>

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void updateTicketCount(int count);
    void selectTicket(QListWidgetItem* item);
    void updateTicketName();
    void updateTicketStatus(int index);
    void nextRandomTicket();
    void previousTicket();
    void toggleTicketStatus(QListWidgetItem* item);

   private:
    void setupUi();
    void updateProgress();

    QSpinBox* count_spinbox;
    QListWidget* view_list;
    QLabel* number_label;
    QLabel* name_label;
    QLineEdit* name_edit;
    QComboBox* status_combo;
    QPushButton* next_button;
    QPushButton* prev_button;
    QProgressBar* total_progress;
    QProgressBar* green_progress;
    QGroupBox* question_view;

    struct Ticket {
        QString name;
        int status;
    };

    std::vector<Ticket> tickets;
    std::vector<int> history;
    int current_ticket_index = -1;
};

#endif
