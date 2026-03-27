
#include "Utils.h"

#include <QtCore/qfile.h>

void Utils::writeScheduleToFile(const QString &filename, const Schedule &schedule) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc = schedule.toJson();
        file.write(doc.toJson());
        file.close();
    }
}

Schedule Utils::readScheduleFromFile(const QString &filename) {
    static Schedule schedule; // Static object persists between function calls

    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
        schedule = Schedule::fromJson(doc); // Update the static object
        file.close();
    }

    return schedule; // Return a reference to the static object
}