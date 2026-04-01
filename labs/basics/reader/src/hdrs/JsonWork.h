#ifndef JSONWORK_H
#define JSONWORK_H

#include <QFile>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QTextStream>

class JsonWork {
   public:
    static QJsonObject parseJson(const QString& path);

    static void writeToJson(const QString& path, const QJsonObject& json);
};

#endif  // JSONWORK_H
