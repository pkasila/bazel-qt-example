#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QListWidgetItem>
#include <QDateTime>
#include <QDialog>
#include "task.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

private slots:
    void onAddTaskClicked();
    void onRemoveTaskClicked();
    void updateTaskList();
    void checkReminders();
    void onSaveClicked();
    void onLoadClicked();
    void onTaskDoubleClicked(QListWidgetItem* item);
private:
    Ui::MainWindow* ui_;
    QList<Task> tasks_;
    QListWidget* tasks_list_widget_;
    QTimer* reminder_timer_;
    void addTask(const Task& task);
    void removeTask(int index);
    void setupReminderTimer();
};

#endif // MAINWINDOW_H
