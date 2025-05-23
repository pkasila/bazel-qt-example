#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidget>
#include <QSpinBox>
#include <QLabel>
#include <QComboBox>
#include <QPushButton>
#include <QProgressBar>
#include <QGroupBox>
#include <QLineEdit>

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

private slots:
    void on_spinBox_valueChanged(int arg1);
    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);
    void on_listWidget_itemClicked(QListWidgetItem *item);
    void on_comboBox_currentIndexChanged(int index);
    void on_lineEdit_editingFinished();
    void on_pushButton_clicked();
    void on_pushButton_2_clicked();

private:
    Ui::MainWindow *ui;
    QWidget *centralWidget;
    QListWidget *listWidget;
    QSpinBox *spinBox;
    QProgressBar *progressBar;
    QProgressBar *progressBar_2;
    QGroupBox *groupBox;
    QLabel *name;
    QLabel *number;
    QComboBox *comboBox;
    QLineEdit *lineEdit;
    QPushButton *randomButton;
    QPushButton *prevButton;
};

#endif // MAINWINDOW_H