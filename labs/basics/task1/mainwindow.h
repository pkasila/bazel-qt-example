#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QString>
#include <QStack>
#include <QTableWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>

enum class TicketStatus { Default, Yellow, Green };

struct Ticket {
    QString name;
    TicketStatus status = TicketStatus::Default;
};

class MainWindow : public QMainWindow {

public: 
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    private:
    void onCountChanged(int count);
    void onCellClicked(int row, int column);
    void onCellDoubleClicked(int row, int column);
    void onNameEdited();
    void onStatusChanged(int index);
    void nextRandomTicket();
    void previousTicket();

    void setupUI();
    void updateQuestionView(int index);
    void updateProgress();
    void applyStatusColor(int row, TicketStatus status);

    QVector<Ticket> m_tickets;
    QStack<int> m_history;
    int m_currentIndex = -1;
    bool m_blockSignals = false;

    QSpinBox *m_countSpinBox;
    QTableWidget *m_viewTable;
    
    QGroupBox *m_questionGroup;
    QLabel *m_numLabel;
    QLabel *m_nameLabel;
    QLineEdit *m_nameEdit;
    QComboBox *m_statusCombo;
    
    QPushButton *m_nextBtn;
    QPushButton *m_prevBtn;
    
    QProgressBar *m_totalProgress;
    QProgressBar *m_greenProgress;
};

#endif
