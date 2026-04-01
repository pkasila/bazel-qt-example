#include "task.h"

Task::Task(const QString& name, const QString& description, const QDateTime& date_time, bool reminder_enabled)
    : name_(name), description_(description), date_time_(date_time), reminder_enabled_(reminder_enabled) {}

QString Task::getName() const {
    return name_;
}

QString Task::getDescription() const {
    return description_;
}

QDateTime Task::getDateTime() const {
    return date_time_;
}

void Task::setName(const QString& name) {
    name_ = name;
}

void Task::setDescription(const QString& description) {
    description_ = description;
}

void Task::setDateTime(const QDateTime& date_time) {
    date_time_ = date_time;
}

bool Task::operator < (const Task& other) const {
    return date_time_ < other.date_time_;
}

bool Task::operator > (const Task& other) const {
    return date_time_ > other.date_time_;
}

bool Task::isReminderEnabled() const {
    return reminder_enabled_;
}

void Task::setReminderEnabled(bool reminder_enabled) {
    reminder_enabled_ = reminder_enabled;
}

QDataStream &operator<<(QDataStream &out, const Task &task) {
    out << task.name_ << task.description_ << task.date_time_ << task.reminder_enabled_;
    return out;
}

QDataStream &operator>>(QDataStream &in, Task &task) {
    in >> task.name_ >> task.description_ >> task.date_time_ >> task.reminder_enabled_;
    return in;
}
