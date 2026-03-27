#include "Schedule.h"
#include "DateSchedule.h"

#include <QJsonArray>
#include <QJsonObject>

QJsonDocument Schedule::toJson() const {
    QJsonArray datesArray;
    for (const DateSchedule &dateSchedule : dates) {
        datesArray.append(dateSchedule.toJson());
    }
    QJsonObject json;
    json["schedule"] = datesArray;
    return QJsonDocument(json);
}

Schedule Schedule::fromJson(const QJsonDocument &doc) {
    Schedule schedule;
    QJsonObject json = doc.object();
    QJsonArray datesArray = json["schedule"].toArray();
    for (const QJsonValue &value : datesArray) {
        schedule.dates.append(DateSchedule::fromJson(value.toObject()));
    }
    return schedule;
}