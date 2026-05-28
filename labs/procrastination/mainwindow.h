#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QListWidgetItem>
#include <QMainWindow>
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

private slots:
    void updateTickets(int count);
    void showTicket();
    void renameTicket();
    void changeStatus();

    void itemDoubleClicked(QListWidgetItem *item);

    void nextQuestion();
    void previousQuestion();

private:
    enum Status
    {
        Default,
        Yellow,
        Green
    };

    void updateColors();
    void updateProgress();

    QVector<Status> statuses;

    int currentTicket = -1;
    int previousTicketIndex = -1;

    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
