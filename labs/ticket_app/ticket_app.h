#ifndef TICKET_APP_H
#define TICKET_APP_H

#include <QStack>
#include <QString>
#include <QVector>
#include <QWidget>

QT_BEGIN_NAMESPACE
class QSpinBox;
class QListWidget;
class QListWidgetItem;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
class QGroupBox;
class QTextEdit;
QT_END_NAMESPACE

enum class TicketStatus { Default = 0, Yellow = 1, Green = 2 };

struct Ticket {
    QString name;
    QString hint;
    TicketStatus status = TicketStatus::Default;
};

class TicketApp : public QWidget {
    Q_OBJECT

   public:
    TicketApp(QWidget* parent = nullptr);

   private slots:
    void onCountChanged(int count);
    void onTicketSelected(QListWidgetItem* item);
    void onTicketDoubleClicked(QListWidgetItem* item);
    void onNameEdited();
    void onStatusComboChanged(int index);
    void onHintChanged();
    void nextRandomTicket();
    void previousTicket();
    void saveToFile();
    void loadFromFile();

   private:
    void setupUI();
    void updateTicketDisplay(int index);
    void updateProgress();
    void setItemColor(QListWidgetItem* item, TicketStatus status);

    QVector<Ticket> m_tickets;
    QStack<int> m_history;
    int m_currentIndex = -1;

    QSpinBox* m_countSpin;
    QListWidget* m_viewList;
    QProgressBar* m_totalProgress;
    QProgressBar* m_greenProgress;

    QGroupBox* m_questionGroup;
    QLabel* m_numberLabel;
    QLabel* m_nameLabel;
    QLineEdit* m_nameEdit;
    QComboBox* m_statusCombo;
    QTextEdit* m_hintEdit;

    QPushButton* m_nextBtn;
    QPushButton* m_prevBtn;
    QPushButton* m_saveBtn;
    QPushButton* m_loadBtn;
};

#endif