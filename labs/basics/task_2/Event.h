#pragma once
#include <QJsonObject>


class Event {
public:
    QString time;
    QString task;

    Event(const QString &time, const QString &task) : time(time), task(task) {}

    // serialize to JSON
    QJsonObject toJson() const;

    // deserialize from JSON
    static Event fromJson(const QJsonObject &json);
};
