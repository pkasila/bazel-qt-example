#ifndef CALENDAR_H
#define CALENDAR_H

#include <QCalendarWidget>
#include <QDataStream>
#include <QDate>
#include <QDateEdit>
#include <QFile>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMainWindow>
#include <QMap>
#include <QPainter>
#include <QPushButton>
#include <QSet>

struct Task {
    QString name;
    bool isDone;

    friend QDataStream& operator<<(QDataStream& out, const Task& task) {
        out << task.name << task.isDone;
        return out;
    }

    friend QDataStream& operator>>(QDataStream& in, Task& task) {
        in >> task.name >> task.isDone;
        return in;
    }
};

class CustomCalendar : public QCalendarWidget {
    Q_OBJECT
   public:
    explicit CustomCalendar(QWidget* parent = nullptr) : QCalendarWidget(parent), dataRef(nullptr) {
    }

    QMap<QDate, QList<Task>>* dataRef;
    QSet<QDate> highlightedDates;

    void forceUpdate() {
        this->updateCells();
    }

   protected:
    void paintCell(QPainter* painter, const QRect& rect, QDate date) const override;
};

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void onDateSelected();
    void onAddTask();
    void onDeleteTask();
    void onTaskStateChanged(QListWidgetItem* item);
    void onDistributeTasks();
    void onUndoDistribution();
    void onResetAll();

   private:
    void setupUi();
    void applyStyles();
    void refreshTaskList();
    void saveData();
    void loadData();

    CustomCalendar* calendar;
    QListWidget* taskList;
    QLineEdit *singleTaskInput, *bulkTaskInput;
    QDateEdit *startDateEdit, *endDateEdit;
    QPushButton *btnAddTask, *btnDeleteTask, *btnDistribute, *btnUndo, *btnReset;
    QLabel* lblCurrentDate;

    QMap<QDate, QList<Task>> dataStore;
    QList<QPair<QDate, QString>> lastDistribution;
};

#endif  // CALENDAR_H