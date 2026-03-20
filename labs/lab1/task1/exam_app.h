#ifndef EXAMAPP_H
#define EXAMAPP_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QStack>
#include <QStyledItemDelegate>
#include <QWidget>
#include <vector>

enum class TicketStatus { Default = 0, Yellow = 1, Green = 2 };

struct Ticket {
    int number;
    QString name;
    TicketStatus status = TicketStatus::Default;
};

class TicketDelegate : public QStyledItemDelegate {
   public:
    void paint(
        QPainter* painter, const QStyleOptionViewItem& option,
        const QModelIndex& index) const override;
    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override;
};

class ExamApp : public QWidget {
    Q_OBJECT
   public:
    ExamApp(QWidget* parent = nullptr);
    ~ExamApp();

   private slots:
    void applyNewTicketCount();
    void onTicketClicked(QListWidgetItem* item);
    void updateTicketName();
    void onStatusChanged(int index);
    void nextRandomQuestion();
    void previousQuestion();
    void resetProgress();
    void showHelp();

   private:
    void setupUi();
    void setupShortcuts();
    void refreshTicketView(int index);
    void updateProgress();
    void loadData();
    void saveData();

    std::vector<Ticket> tickets;
    QStack<int> history;
    int currentTicketIndex = -1;

    QSpinBox* countSpinBox;
    QListWidget* ticketList;
    QLabel *ticketNumLabel, *ticketNameLabel;
    QLineEdit* nameEdit;
    QComboBox* statusCombo;
    QProgressBar *totalProgress, *greenProgress;
    QPushButton *btnNext, *btnPrev, *btnReset, *btnHelp;
};

#endif