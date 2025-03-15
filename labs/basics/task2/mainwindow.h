#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCheckBox>
#include <QGroupBox>
#include <QTextEdit>
#include <QSpinBox>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QString>


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

    QTextEdit *textEdit;
    QSpinBox *spinBox1;
    QSpinBox *spinBox2;
    QSpinBox *spinBox3;
    QSpinBox *spinBox4;
    QGroupBox *groupBox;
    QCheckBox *checkBox;
    QLabel *label1;
    QLabel *label2;
    QLabel *label3;
    QLabel *label4;
    QLabel *label5;
    QRadioButton *radioButton1;
    QRadioButton *radioButton2;
    QRadioButton *radioButton3;
    QRadioButton *radioButton4;
    QPushButton *resultsButton;
    QButtonGroup *radioGroup;
    bool facultyChosen = false;
    bool hasBenefits = false;

    QString name = "";
    int facultyPicked = -1;
    int langPoints = 30;
    int mathPoints = 30;
    int physPoints = 30;
    int gpaPoints = 30;




    private slots:
        void changeName();
    void onRadioButtonClicked();
    void onSpinBox1ValueChanged(int newValue);
    void onSpinBox2ValueChanged(int newValue);
    void onSpinBox3ValueChanged(int newValue);
    void onSpinBox4ValueChanged(int newValue);
    void giveBenefits(int val);
    void showResultsDialog();
};
#endif // MAINWINDOW_H
