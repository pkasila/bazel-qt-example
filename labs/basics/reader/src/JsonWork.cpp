#include "hdrs/JsonWork.h"

QJsonObject JsonWork::parseJson(const QString& path) {
    QString val;
    QFile file;
    file.setFileName(path);
    file.open(QIODevice::ReadOnly | QIODevice::Text);
    val = file.readAll();
    file.close();
    QJsonDocument doc = QJsonDocument::fromJson(val.toUtf8());
    QJsonObject json = doc.object();
    return json;
}

void JsonWork::writeToJson(const QString& path, const QJsonObject& json) {
    QJsonDocument doc(json);
    QString jsonString = doc.toJson(QJsonDocument::Indented);

    QFile file;
    file.setFileName(path);
    file.open(QIODevice::WriteOnly | QIODevice::Text);
    QTextStream stream(&file);
    stream << jsonString;
    file.close();
}
