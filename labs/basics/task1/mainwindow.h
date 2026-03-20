// NOLINTBEGIN(cppcoreguidelines-owning-memory, readability-identifier-naming)
#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTime>
#include <QTimer>
#include <QVector>
#include <utility>

enum class TicketStatus : int8_t { Default, Yellow, Green };

struct Ticket {
    int number;
    QString name;
    TicketStatus status;

    Ticket(int num, QString n, TicketStatus s) : number(num), name(std::move(n)), status(s) {
    }
};

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override;

   private slots:
    void onCountChanged(int count);
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onNameEdited();
    void onStatusChanged(int index);
    void onNextClicked();
    void onPreviousClicked();

   private:
    QSpinBox* countSpin;
    QListWidget* ticketsList;
    QLabel* numberLabel;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QComboBox* statusCombo;
    QPushButton* nextButton;
    QPushButton* previousButton;
    QProgressBar* progressBar;
    QProgressBar* progressBar2;
    QLabel* statsLabel;
    QTime totalStudyTime;
    QTimer* studyTimer;
    QPushButton* pauseButton;

    bool isPaused = false;

    QVector<Ticket> tickets;
    QVector<int> history;
    int currentIndex = -1;

    void updateTicketList();
    void updateQuestionView();
    void updateCurrentItem();
    void updateProgress();
    void updateStatistics();

    QColor statusColor(TicketStatus status);
    TicketStatus intToStatus(int index);
    int statusToInt(TicketStatus status);
    QVector<int> getAvailableTickets();
};

#endif  // MAINWINDOW_H
        // NOLINTEND(cppcoreguidelines-owning-memory, readability-identifier-naming)