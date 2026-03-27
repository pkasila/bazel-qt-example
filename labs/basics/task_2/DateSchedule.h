#pragma once

#include "Event.h"

class DateSchedule {
public:
    QDate date;
    QList<Event> events;

    DateSchedule(const QDate &date) : date(date) {}
    DateSchedule(const QDate &date, QList<Event> events) : date(date), events(events) {}


    // serialize to JSON
    QJsonObject toJson() const;

    // deserialize from JSON
    static DateSchedule fromJson(const QJsonObject &json);
};
