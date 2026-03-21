#pragma once

#include <QWidget>
#include <QListWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QVector>
#include <QStack>

enum class TicketStatus { Default, Yellow, Green };

struct Ticket {
    QString name;
    TicketStatus status = TicketStatus.Default;
};

class ExamApp : public QWidget {
    Q_OBJECT

public:
    ExamApp(QWidget *parent = nullptr);

private slots:
    void onCountChanged(int count);
    void onTicketSelected(QListWidgetItem *item);
    void onTicketDoubleClicked(QListWidgetItem *item);
    void updateTicketName();
    void onStatusChanged(int index);
    void nextRandomTicket();
    void previousTicket();

private:
    void setupUi();
    void updateUIForTicket(int index);
    void updateProgress();
    void updateItemVisuals(int index);

    // Data
    QVector<Ticket> tickets;
    QStack<int> history;
    int currentIndex = -1;

    // UI Elements
    QSpinBox *countSpinBox;
    QListWidget *viewWidget;
    QLabel *numberLabel;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *statusCombo;
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;
};