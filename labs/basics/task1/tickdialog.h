#ifndef TICKDIALOG_H
#define TICKDIALOG_H

#include <QDialog>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QVBoxLayout>
#include "ticket.h"
#include <QTextEdit>

class TickDialog : public QDialog {
    Q_OBJECT

public:
    explicit TickDialog(Ticket& ticket, QWidget *parent = nullptr);

private slots:
    void onNameChanged();
    void onStatusChanged(int index);
    void onHintChanged();
signals:
    void nameChanged(int index, const QString& newName);
    void statusChanged(int index, int newStatus);
    void hintChanged(int index, const QString& newHint);
private:
    Ticket& ticket_;
    int ticket_index_;
    QLabel* name_label_;
    QLineEdit* name_edit_;
    QLabel* status_label_;
    QComboBox* status_combo_;
    QTextEdit* hint_edit_;
};

#endif // TICKDIALOG_H
