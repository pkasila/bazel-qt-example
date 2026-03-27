#ifndef ADDMEDICINEDIALOG_H
#define ADDMEDICINEDIALOG_H

#include <QDialog>
#include <QWidget>
#include <QTextEdit>
#include <QLineEdit>
#include <QTimeEdit>
#include <QTime>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QFormLayout>
#include <QComboBox>
#include <QSpinBox>
//
#include "ProjectConstants.h"
#include "Medicine.h"

class AddMedicineDialog : public QDialog {
    Q_OBJECT

public:
    explicit AddMedicineDialog(QWidget *parent = nullptr);
    Medicine getMedicine() const;

private slots:
    void confirmMedicine();

private:
    QLineEdit *nameEdit;
    QSpinBox *dosageSpinBox;
    QComboBox *measureComboBox;
    QTimeEdit *timeEdit;
    QTextEdit *notesEdit;
    QPushButton *okButton;
    QPushButton *cancelButton;
};

#endif // ADDMEDICINEDIALOG_H
