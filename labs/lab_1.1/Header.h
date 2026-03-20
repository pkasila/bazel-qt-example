#ifndef HEADER_H
#define HEADER_H

#include <QMainWindow>
#include <vector>
#include <QString>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QListWidget;
class QListWidgetItem;
class QGroupBox;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
QT_END_NAMESPACE

enum class Status { Default, Yellow, Green };

struct Ticket {
    int id;
    QString name;
    Status status = Status::Default;
};

class TicketApp : public QMainWindow {
    Q_OBJECT
public:
    TicketApp();

private slots:
    void updateTicketCount(int count);
    void onTicketSelected(QListWidgetItem* item);
    void onTicketDoubleClicked(QListWidgetItem* item);
    void onNameEdited();
    void onStatusComboChanged(int comboIndex);
    void nextRandomQuestion();
    void previousQuestion();

private:
    void setupUI();
    void displayTicket(int index);
    void updateStatus(int index, Status s);
    void updateProgress();
    void clearQuestionView();

    std::vector<Ticket> tickets;
    std::vector<int> history;
    int currentIndex = -1;

    QSpinBox* countSpinBox;
    QListWidget* listWidget;
    QGroupBox* questionGroup;
    QLabel *numLabel, *nameLabel;
    QLineEdit* nameEdit;
    QComboBox* statusCombo;
    QPushButton *nextBtn, *prevBtn;
    QProgressBar *totalProgress, *greenProgress;
};

#endif // HEADER_H