#ifndef TICKET_REVIEWER_H
#define TICKET_REVIEWER_H

#include <QtWidgets>

struct Ticket {
    int number;
    QString name;

    enum Status { Default, Yellow, Green } status;

    Ticket(int num = 0) : number(num), name("Билет " + QString::number(num)), status(Default) {
    }
};

class TicketReviewer : public QMainWindow {
    Q_OBJECT

   public:
    TicketReviewer(QWidget* parent = nullptr);

   private slots:
    void onTicketCountChanged(int count);
    void onTicketClicked(QListWidgetItem* item);
    void onTicketDoubleClicked(QListWidgetItem* item);
    void onNameEditReturnPressed();
    void onNameEditFocusChanged(bool hasFocus);
    void onStatusChanged(int index);
    void onNextQuestion();
    void onPreviousQuestion();
    void updateProgress();

   private:
    void createWidgets();
    void createLayouts();
    void createConnections();
    void updateTicketView(int index);
    void selectRandomTicket();
    void updateTicketColors();
    bool isFocusInNameEdit() const;

    QSpinBox* countSpinBox;
    QListWidget* viewWidget;
    QLabel* numberLabel;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QComboBox* statusComboBox;
    QPushButton* nextButton;
    QPushButton* previousButton;
    QProgressBar* totalProgressBar;
    QProgressBar* greenProgressBar;

    QVector<Ticket> tickets;
    int currentTicketIndex;
    int previousTicketIndex;
    QVector<int> history;
};

#endif
