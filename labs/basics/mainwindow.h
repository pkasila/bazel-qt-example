#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QSpinBox>
#include <QListWidget>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QRandomGenerator>
#include <QVBoxLayout>
#include <QStack>
#include <QTextEdit>
#include <QMessageBox>

#include "HintTextEdit.h"

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void updateView(int count);
    void handleItemClick(QListWidgetItem *item);
    void handleItemDoubleClick(QListWidgetItem *item);
    void updateQuestionView(int ticketNumber);
    void updateTicketName();
    void updateStatus(int index);
    void nextQuestion();
    void previousQuestion();
    void updateProgress();
    void saveHint();
    void resetStatusToDefault();

private:
    void setupUI();

    QSpinBox *spinBox;
    QListWidget *listWidget;
    QLabel *numberLabel;
    QLabel *nameLabel;
    QLineEdit *nameEdit;
    QComboBox *statusComboBox;
    QPushButton *nextButton;
    QPushButton *previousButton;
    QProgressBar *totalProgress;
    QProgressBar *greenProgress;
    HintTextEdit *hintEdit;
    int currentTicket = 1;
    QStack<int> ticketHistory;
    QPushButton *resetStatusButton;

};

#endif // MAINWINDOW_H
