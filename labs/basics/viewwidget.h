#ifndef VIEWWIDGET_H
#define VIEWWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QVBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QComboBox>
#include <QGroupBox>

class Ticket {
public:
    enum Status { Default, Yellow, Green };

    Ticket(int number, const QString& name = "")
        : number(number),
        name(name.isEmpty() ? "Билет " + QString::number(number) : name),
        status(Default) {}

    int number;
    QString name;
    Status status;
};
class ProgressWidget : public QWidget {
    Q_OBJECT
public:
    explicit ProgressWidget(QWidget *parent = nullptr);
    int count_yellow = 0;
    int count_green = 0;
    int tickets_count = 0;
    void updateBars(int condtition);
    void Clear();
private:
    QProgressBar *totalProgress;
    QLabel *green;
    QLabel *tot;
    QProgressBar *greenProgress;
};

class QuestionWidget : public QWidget {
    Q_OBJECT
public:
    explicit QuestionWidget(QWidget *parent = nullptr);
    void setTickets(const QList<Ticket*>& tickets);
    void addToHistory(Ticket* ticket);
    bool isPrev = false;
signals:
    void ticketSelected(Ticket* ticket);

private slots:
    void handleNext();
    void handlePrev();
private:
    QPushButton *next_question;
    QPushButton *prev_question;
    QList<Ticket*> tickets;
    QList<Ticket*> history;
    int currentIndex = -1;
};
class ViewWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ViewWidget(QWidget *parent = nullptr);

    void setItemCount(int count);
    void updateItemAppearance(QListWidgetItem* item);

private slots:
    void handleTicketSelection(Ticket* ticket);
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onNameEditFinished();
    void onStatusChanged(int index);

private:
    QListWidget *listWidget;
    QLabel *numberLabel;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *statusCombo;
    QGroupBox *questionView;
    QuestionWidget *questionWidget;
    ProgressWidget *progressWidget;
    QList<Ticket*> allTickets;
    Ticket* currentTicket;
    QListWidgetItem* currentItem;
};

#endif // VIEWWIDGET_H
