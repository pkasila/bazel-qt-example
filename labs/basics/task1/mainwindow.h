#ifndef LABS_BASICS_TASK1_MAINWINDOW_H_
#define LABS_BASICS_TASK1_MAINWINDOW_H_

#include <QMainWindow>
#include <QString>
#include <QVector>

class QComboBox;
class QColor;
class QGroupBox;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QProgressBar;
class QPushButton;
class QSpinBox;

class MainWindow : public QMainWindow {
public:
    MainWindow();

private:
    enum class TicketStatus {
        Default,
        Yellow,
        Green,
    };

    struct Ticket {
        int number = 0;
        QString name;
        TicketStatus status = TicketStatus::Default;
    };

    void setupUi();
    void applyStyle();
    void setupConnections();

    void rebuildTickets(int count);
    void setCurrentTicket(int index, bool add_to_history);
    void pushHistory(int index);
    void updateQuestionView();
    void updateTicketItem(int index);
    void updateProgressBars();
    void setTicketStatus(int index, TicketStatus status);
    void applyStatusToCurrentFromCombo(int combo_index);
    void commitCurrentNameFromEditor();
    void moveToNextRandomTicket();
    void moveToPreviousTicket();

    int toComboIndex(TicketStatus status) const;
    TicketStatus fromComboIndex(int combo_index) const;
    QString statusText(TicketStatus status) const;
    QColor statusColor(TicketStatus status) const;
    QString ticketDisplayText(const Ticket &ticket) const;

    QSpinBox *countSpinBox_ = nullptr;
    QListWidget *viewList_ = nullptr;

    QGroupBox *questionBox_ = nullptr;
    QLabel *numberLabel_ = nullptr;
    QLabel *nameLabel_ = nullptr;
    QLineEdit *nameEdit_ = nullptr;
    QComboBox *statusCombo_ = nullptr;
    QPushButton *nextButton_ = nullptr;
    QPushButton *previousButton_ = nullptr;
    QProgressBar *totalProgressBar_ = nullptr;
    QProgressBar *greenProgressBar_ = nullptr;

    QVector<Ticket> tickets_;
    QVector<int> history_;
    int historyPosition_ = -1;
    int currentTicketIndex_ = -1;
    bool suppressStatusComboSignal_ = false;
};

#endif
