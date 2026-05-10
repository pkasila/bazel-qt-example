#include "languagetool_client.h"

#include <QDebug>
#include <QUrl>

LanguageToolClient::LanguageToolClient(QObject* parent)
    : QObject(parent)
    , manager_(new QNetworkAccessManager(this))
    , base_url_("https://api.languagetool.org/v2") {
    connect(
        manager_, &QNetworkAccessManager::finished, this, &LanguageToolClient::onReplyFinished);
}

void LanguageToolClient::setBaseUrl(const QString& url) {
    base_url_ = url;
}

void LanguageToolClient::check(const QString& text, const QString& language) {
    QUrl url(base_url_ + "/check");
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QUrlQuery params;
    params.addQueryItem("text", text);
    params.addQueryItem("language", language);
    params.addQueryItem("level", "picky");

    manager_->post(request, params.toString(QUrl::FullyEncoded).toUtf8());
}

void LanguageToolClient::onReplyFinished(QNetworkReply* reply) {
    LTCheckResult result;
    result.success = false;

    if (reply->error() != QNetworkReply::NoError) {
        result.errorString = reply->errorString();
        emit checkFinished(result);
        reply->deleteLater();
        return;
    }

    QByteArray data = reply->readAll();
    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(data, &parseError);

    if (parseError.error != QJsonParseError::NoError) {
        result.errorString = "JSON parse error: " + parseError.errorString();
        emit checkFinished(result);
        reply->deleteLater();
        return;
    }

    QJsonObject root = doc.object();
    result.success = true;

    QJsonObject langObj = root["language"].toObject();
    QJsonObject detectedObj = langObj["detectedLanguage"].toObject();
    result.detectedLanguage = detectedObj["code"].toString();

    QJsonArray matchesArr = root["matches"].toArray();
    for (const QJsonValue& val : matchesArr) {
        QJsonObject matchObj = val.toObject();
        LTMatch match;
        match.message = matchObj["message"].toString();
        match.shortMessage = matchObj["shortMessage"].toString();
        match.offset = matchObj["offset"].toInt();
        match.length = matchObj["length"].toInt();

        QJsonArray replacementsArr = matchObj["replacements"].toArray();
        for (const QJsonValue& rep : replacementsArr) {
            match.replacements.append(rep.toObject()["value"].toString());
        }

        QJsonObject ruleObj = matchObj["rule"].toObject();
        match.ruleId = ruleObj["id"].toString();
        QJsonObject catObj = ruleObj["category"].toObject();
        match.ruleCategory = catObj["id"].toString();

        result.matches.append(match);
    }

    emit checkFinished(result);
    reply->deleteLater();
}
