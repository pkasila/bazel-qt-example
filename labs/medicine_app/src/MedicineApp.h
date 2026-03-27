#ifndef MEDICINEAPP_H
#define MEDICINEAPP_H
//Qt libraries
#include <QWidget>
#include <QMainWindow>
#include <QTableWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QDialog>
#include <QMessageBox>
#include <QHeaderView>
#include <QEvent>
#include <QSpinBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QComboBox>
#include <QCalendar>
#include <QCalendarWidget>
#include <QMap>
#include <QVector>
#include <QDate>
#include <QTextCharFormat>
#include <QPoint>
#include <QModelIndex>
#include <QMenu>

//C++ libraries
#include <algorithm>

//Custom libraries
#include "ProjectConstants.h"
#include "Medicine.h"
#include "AddMedicineDialog.h"

class MedicineApp : public QMainWindow {
    Q_OBJECT

public:
    MedicineApp(QWidget *parent = nullptr);

private:
    void setupUI();
    void setupConnections();

    void deleteMedicineRow(int row);
    void addMedicineToMedicineTable(const Medicine& medicine, int newRow);

private slots:
    void updateTable();
    void addMedicine();
    void showContextMenu(const QPoint& position);

private:
    QCalendarWidget *calendar;
    QTableWidget *medicineTable;
    QPushButton *addButton;

private:
    QMap<QDate, QVector<Medicine>> medicineData;
    QTextCharFormat format;
};

#endif // MEDICINEAPP_H
