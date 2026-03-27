//
// Created by Kirill Smychok on 18.03.25.
//
#include "Event.h"
QJsonObject Event::toJson() const {
    QJsonObject json;
    json["time"] = time;
    json["task"] = task;
    return json;
}
Event Event::fromJson(const QJsonObject &json) {
    return Event(json["time"].toString(), json["task"].toString());
}