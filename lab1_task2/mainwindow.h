#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QVector>
#include <QMap>
#include <QDate>
#include <QString>

class QWidget;
class QLineEdit;
class QPushButton;
class QCheckBox;
class QListWidget;
class QListWidgetItem;
class QComboBox;
class QLabel;
class QProgressBar;
class QDateEdit;

struct Habit {
    int id;
    QString name;
    QString category;
    QDate startDate;
    bool active;
};

class MainWindow : public QMainWindow
{
public:
    MainWindow(QWidget *parent = nullptr);

protected:
    void keyPressEvent(QKeyEvent *event) override;

private:
    QVector<Habit> habits;
    QMap<QDate, QMap<int, bool>> history;
    QDate currentDate;
    int currentHabitId = -1;
    int nextHabitId = 1;

    QPushButton *prevDayButton;
    QPushButton *nextDayButton;
    QPushButton *todayButton;
    QLabel *dateLabel;
    QDateEdit *dateEdit;

    QLineEdit *nameEdit;
    QComboBox *categoryBox;
    QPushButton *addButton;
    QComboBox *filterBox;

    QListWidget *habitList;

    QLabel *nameLabel;
    QLabel *categoryLabel;
    QLabel *startDateLabel;
    QLabel *statusHintLabel;

    QCheckBox *doneCheckBox;
    QPushButton *toggleDoneButton;
    QPushButton *archiveButton;

    QProgressBar *progressBar;
    QLabel *progressLabel;

    void setupUi();
    void connectUi();

    void addHabit();
    void archiveCurrentHabit();

    void setCurrentDate(const QDate &date);
    void updateDateLabel();

    void refreshHabitList();
    void updateHabitItem(QListWidgetItem *item, int habitId);
    void showHabit(int habitId);
    void clearHabitDetails();
    void updateProgress();

    QVector<Habit*> visibleHabitsForCurrentDate();
    const Habit* findHabitById(int id) const;
    Habit* findHabitById(int id);

    bool isHabitVisibleOnDate(const Habit &habit, const QDate &date) const;
    bool passesCategoryFilter(const Habit &habit) const;

    bool isHabitDoneOnCurrentDate(int habitId) const;
    void setHabitDoneOnCurrentDate(int habitId, bool done);
};

#endif