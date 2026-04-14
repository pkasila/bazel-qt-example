#include "TaskManager.h"

#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtCore/QJsonArray>
#include <QtCore/QJsonDocument>
#include <QtCore/QJsonObject>

bool TaskManager::loadFromFile(const QString& path) {
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open file:" << path;
        return false;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (doc.isNull()) {
        qDebug() << "Failed to parse JSON:" << parseError.errorString() << "at offset"
                 << parseError.offset;
        return false;
    }
    QJsonObject root = doc.object();

    QJsonArray transArr = root["translation"].toArray();
    for (auto val : transArr) {
        QJsonObject obj = val.toObject();
        translationTasks.push_back(
            {obj["original"].toString(), obj["translation"].toString(), obj["hint"].toString()});
    }

    QJsonArray gramArr = root["grammar"].toArray();
    for (auto val : gramArr) {
        QJsonObject obj = val.toObject();
        QStringList opts;
        for (auto opt : obj["options"].toArray()) {
            opts << opt.toString();
        }
        grammarTasks.push_back(
            {obj["question"].toString(), opts, obj["answer"].toString(), obj["hint"].toString()});
    }

    QJsonArray mathHardArr = root["math_hard"].toArray();
    for (auto val : mathHardArr) {
        QJsonObject obj = val.toObject();
        mathHardTasks.push_back(
            {obj["expression"].toString(), obj["answer"].toInt(), obj["hint"].toString()});
    }

    qDebug() << "Loaded" << translationTasks.size() << "translation tasks," << grammarTasks.size()
             << "grammar tasks," << mathHardTasks.size() << "math hard tasks from" << path;

    return true;
}