#ifndef INFOBOX_H
#define INFOBOX_H

#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include "ticketitem.h"

class InfoBox : public QGroupBox
{
    Q_OBJECT

public:
    explicit InfoBox(QWidget *parent = nullptr);
    void setTicket(TicketItem *ticket);

private slots:
    void change_the_name();
    void change_the_status(int index);

private:
    QLabel *number;
    QLineEdit *name_edit;
    QComboBox *status;
    TicketItem *currenttick;
};

#endif // INFOBOX_H
