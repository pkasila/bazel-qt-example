#ifndef HEADER_H
#define HEADER_H

#include <QMainWindow>
#include <QSoundEffect>
#include <QTemporaryFile>
#include <QDate>
#include <vector>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLineEdit;
class QCheckBox;
class QCalendarWidget;
class QDial;
class QLabel;
class QListWidget;
class QListWidgetItem;
class QTimer;
QT_END_NAMESPACE

struct Task {
    QDate date;
    QString text;
    bool completed = false;
};

class CreativeApp : public QMainWindow {
    Q_OBJECT

public:
    CreativeApp(QWidget *parent = nullptr);
    ~CreativeApp() = default;

private slots:
    void adjustVolume(int value);
    void toggleUrgency(bool checked);
    void addTask();
    void updateDateLabel();
    void toggleTaskStatus(QListWidgetItem* item);
    void deleteTask();
    void playReminder();

private:
    void setupUI();
    void renderTasks();

    std::vector<Task> tasks;

    QSoundEffect *alarmSound;
    QTemporaryFile tempSoundFile;

    QPushButton *addBtn;
    QPushButton *delBtn;
    QLineEdit *noteEdit;
    QCheckBox *urgentBox;
    QCalendarWidget *calendar;
    QDial *dial;
    QListWidget *taskList;
    
    QLabel *volumeLabel;
    QLabel *infoLabel;
    QLabel *dateInfo;

    QTimer *reminderTimer;

    int lastDialValue = 0;
    double accumulatedAngle = 0.0;
};

#endif // HEADER_H