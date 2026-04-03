#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "Ticket.h"


#include <QString>
#include <QColor>

#include <QMainWindow>
#include <QLabel>
#include <QSpinBox>
#include <QTableWidget>
#include <QGroupBox>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QStack>
#include <QProgressBar>
#include <QVector>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;

    QSpinBox *spinBox;
    QTableWidget *tableWidget;
    QGroupBox *groupBox;
    QLabel *numberLabel1;
    QLabel *numberLabel2;
    QLineEdit *textEdit;
    QComboBox *statusComboBox;

    QPushButton *next;
    QPushButton *last;

    QProgressBar *progressBar;
    QProgressBar *progressBarDone;

    QVector<Ticket> tickets;


    void initializeQuestions(int count);
    void fillTable();
    void updateTableRow(int row);
    void onCellDoubleClicked(int row, int column);
    void onSpinBoxValueChanged(int arg1);
    void resizeQuestions(int newSize);
    void setDetailsGroupBoxEnabled(bool alpha);
    void onQuestionTextChanged();
    void onQuestionStatusChanged(int index);
    void updateDetailsGroupBox();
    void goToNextTicket();
    void goToLastTicket();
    void refreshProgressBar1();
    void refreshProgressBar2();

    QStack<int> rows;
    int currentRow = -1;
    int numberInProgress = 0;
    int numberDone = 0;

};



#endif // MAINWINDOW_H
