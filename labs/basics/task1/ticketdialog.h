#ifndef DIALOG_H
#define DIALOG_H

#include "mainwindow.h"
#include <QDialog>
#include <QLineEdit>
#include <QLabel>
#include <QString>
#include <QComboBox>
#include <QListWidget>
#include <QPushButton>

class TicketDialog : public QDialog {
    Q_OBJECT
public:
    TicketDialog(MainWindow *parent = nullptr, QListWidgetItem *item = nullptr);

private slots:
    void SetNewName();
    void ChangeStatus(int newStatus = 0);

private:
    QListWidgetItem *item_;
    int index_;
    int *status_;
    QLabel *labelTicketID_;
    QLabel *labelTicketName_;
    QLineEdit *lineEditNewName_;
    QPushButton *buttonSetNewName_;
    QLabel *labelStatus_;
    QComboBox *comboBoxStatus_;
    MainWindow *window_;
};

#endif // DIALOG_H
