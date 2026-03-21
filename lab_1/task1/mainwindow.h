#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFormLayout>
#include <QGroupBox>

#include "ticketmanager.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onTicketCountChanged();
    void onTicketSelected(QListWidgetItem* item);
    void onTicketDoubleClicked(QListWidgetItem* item);
    void onNameEdited();
    void onStatusChanged(int index);
    void onNextQuestionClicked();
    void onPreviousQuestionClicked();
    void onManagerTicketsChanged();

private:
    void setupUI();
    void updateView();
    void updateQuestionView(int index);
    void updateProgressBars();
    int findItemIndex(QListWidgetItem* item) const;

    QSpinBox *m_countBox;
    QListWidget *m_ticketView;
    QGroupBox *m_questionGroupBox;
    QLabel *m_numberLabel;
    QLabel *m_nameLabel;
    QLineEdit *m_nameEdit;
    QComboBox *m_statusCombo;
    QPushButton *m_nextButton;
    QPushButton *m_prevButton;
    QProgressBar *m_totalProgress;
    QProgressBar *m_greenProgress;

    TicketManager *m_manager;
    QVector<int> m_navigationHistory;
    int m_currentIndex;
};

#endif // MAINWINDOW_H