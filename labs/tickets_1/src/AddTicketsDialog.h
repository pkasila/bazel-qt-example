#ifndef ADDTICKETSDIALOG_H
#define ADDTICKETSDIALOG_H
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDialog>
#include <QSpinBox>
#include <QLabel>
//
#include "ProjectConstants.h"
#include "InputUtils.h"
#include "CursorChangingButton.h"

class AddTicketsDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddTicketsDialog(QWidget *parent = nullptr);

    int getTicketCount();

private:
    QSpinBox *additionalTicketCountInput;
    CursorChangingButton *okButton;
    CursorChangingButton *cancelButton;
};
#endif // ADDTICKETSDIALOG_H
