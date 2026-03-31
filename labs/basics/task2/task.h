#ifndef TASK_H
#define TASK_H

#include <QString>
#include <QDateTime>
#include <QDataStream>

class Task
{
public:
    Task(const QString& name = "", const QString& description = "", const QDateTime& date_time = QDateTime(), bool reminder_enabled = false);

    QString getName() const;
    QString getDescription() const;
    QDateTime getDateTime() const;
    bool isReminderEnabled() const;
    void setName(const QString& name);
    void setDescription(const QString& description);
    void setDateTime(const QDateTime& date_time);
    void setReminderEnabled(bool reminder_enabled);
    bool operator < (const Task& other) const;
    bool operator > (const Task& other) const;
    friend QDataStream &operator<<(QDataStream &out, const Task &task);
    friend QDataStream &operator>>(QDataStream &in, Task &task);

private:
    QString name_;
    QString description_;
    QDateTime date_time_;
    bool reminder_enabled_;
};

#endif // TASK_H
