
#include "DateSchedule.h"
#include "Event.h"
#include <QJsonArray>
#include <QJsonObject>

QJsonObject DateSchedule::toJson() const {
    QJsonObject json;
    json["date"] = date.toString("yyyy-MM-dd");
    QJsonArray eventsArray;
    for (const Event &event: events) {
        eventsArray.append(event.toJson());
    }
    json["events"] = eventsArray;
    return json;
}

DateSchedule DateSchedule::fromJson(const QJsonObject &json) {
    DateSchedule dateSchedule(QDate::fromString(json["date"].toString(), Qt::ISODate));
    QJsonArray eventsArray = json["events"].toArray();
    for (const QJsonValue &value : eventsArray) {
        dateSchedule.events.append(Event::fromJson(value.toObject()));
    }
    return dateSchedule;
}