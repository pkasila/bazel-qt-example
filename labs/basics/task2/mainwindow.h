#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include<QTableWidgetItem>


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
    void RenewField();
    void on_pushButton_clicked();
    void SetColorInField();
    void SetColor(QTableWidgetItem* item, int val);
    void SwapTokens(int row1, int column1, int row2, int column2);
    void on_tableWidget_cellClicked(int row, int column);
    void UpdateCounters();
    void CheckWinner();
    void RestartGame();
    void MakeResultMessage(QString text);
    void UpDateProgress();

    void on_Colors1_currentTextChanged(const QString &arg1);

    void on_Colors2_currentTextChanged(const QString &arg1);

private:
    Ui::MainWindow *ui;

};
#endif // MAINWINDOW_H
