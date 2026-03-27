#pragma once
#include <QJsonDocument>
#include <QtCore/QList>
#include "DateSchedule.h"


class Schedule {
public:
    QList<DateSchedule> dates;

    // serialize to JSON
    QJsonDocument toJson() const;

    // deserialize from JSON
    static Schedule fromJson(const QJsonDocument &doc);

};
