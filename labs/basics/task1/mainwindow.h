#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QString>


enum Status{
    DEFAULT,
    PROGRESS,
    LEARNED
};

class Question{
public:
    Question() = default;
    Question(int num, QString text = "", QString name = "") :num_(num), text_(text), name_(name){}

    QString GetText(){
        return text_;
    }

    QString GetName(){
        return name_;
    }

    Status GetStatus(){
        return status_;
    }

    int GetNumber(){
        return num_;
    }

    void SetStatus(Status s){
        status_ = s;
    }

    void SetText(QString text){
        text_ = text;
    }

private:
    int num_ = 0;
    QString text_;
    QString name_;
    Status status_ = Status::DEFAULT;
};

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
    void on_pushButton_clicked();

    void on_listWidget_itemClicked(QListWidgetItem *item);

    void on_listWidget_itemDoubleClicked(QListWidgetItem *item);

    void on_spinBox_valueChanged(int arg1);

    void on_statusBox_currentIndexChanged(int index);

    void on_NextButton_clicked();

    void on_PreviousButton_clicked();

    void SetCurrentQuestion(int number);

    void on_SaveButton_clicked();

    void RenewProgress();

    void RenewUnused(Question x, std::vector<int>& unused);

private:
    Ui::MainWindow *ui;

    std::vector<int> unused;
};
#endif // MAINWINDOW_H
