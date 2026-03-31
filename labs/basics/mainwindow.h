#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSpinBox>
#include <QListWidget>
#include <QListWidgetItem>
#include <QColor>
#include <QBrush>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QComboBox>
#include <QPushButton>
#include <QRandomGenerator>
#include <stack>
#include <QProgressBar>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QSpinBox *spinBox;
    QListWidget *listWidget;
    QGroupBox *groupBox;
    QGroupBox *groupBox1;
    QGroupBox *groupBox2;
    QLabel *label;
    QLabel *label2;
    QLabel *label3;
    QLabel *label4;
    QLineEdit *lineEdit;
    QComboBox *comboBox;
    QPushButton *pushButton;
    QPushButton *pushButton1;
    std::stack<int> indexStack;
    int previousIndex = -1;
    QProgressBar *progressBar;
    QProgressBar *greenProgressBar;

private slots:
    void valueChanged();
    void changeItemColor(QListWidgetItem *item);
    void chosenItemName(QListWidgetItem *item);
    void chosenItem();
    void nameChange();
    void statusChange();
    void nextTicket();
    void previousTicket();
    void stackFill();
    void progressChange();
    void totalProgressChange();
    void nullProgress();
};
#endif // MAINWINDOW_H
