#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <unordered_map>

#include "Event.h"
#include "Schedule.h"

class QAction;
class QCalendarWidget;
class QLabel;
class QTimeEdit;
class QTableWidget;
class QLineEdit;
class QProgressBar;
class QMenu;
class QSpinBox;
class QPushButton;

namespace std {
    template<>
    struct hash<QDate> {
        size_t operator()(const QDate &date) const noexcept {
            return static_cast<size_t>(date.toJulianDay());
        }
    };
}

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow();
    ~MainWindow();

    void addEvent(const QDate &date, const Event &event);

    void removeEvent(const QDate &date, const Event &event);

    QList<Event> getEvents(const QDate &date) const;

private slots:
    void onDateClicked();

    void setNewTime();

    void updateTaskLabel();

    void addNewEvent();

    void deleteSelectedEvent();

private:
    void createActions();

    void updateEventsTable();

    Schedule schedule_;

    QTableWidget *events_table_;
    QCalendarWidget *calendar_;

    QTimeEdit *time_edit_;
    QLabel *time_;

    QLabel *event_label_;
    QLineEdit *event_edit_;

    QPushButton *submit_event_button_;
    QPushButton *delete_event_button_;

    QDate selected_date_ = QDate::currentDate();
    QString selected_time_ = QTime::currentTime().toString("hh:mm");

    QString file_name_ = "schedule.json";

    // storage of events
    std::unordered_map<QDate, QList<Event> > events_map_;
};

#endif // MAINWINDOW_H
