#ifndef LANGUAGETOOL_CLIENT_H
#define LANGUAGETOOL_CLIENT_H

#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QObject>
#include <QString>
#include <QStringList>
#include <QUrlQuery>

struct LTMatch {
    QString message;
    QString shortMessage;
    int offset;
    int length;
    QStringList replacements;
    QString ruleId;
    QString ruleCategory;
};

struct LTCheckResult {
    bool success;
    QString errorString;
    QString detectedLanguage;
    QList<LTMatch> matches;
};

class LanguageToolClient : public QObject {
    Q_OBJECT

   public:
    explicit LanguageToolClient(QObject* parent = nullptr);

    void setBaseUrl(const QString& url);

    void check(const QString& text, const QString& language = "auto");

   signals:
    void checkFinished(const LTCheckResult& result);

   private slots:
    void onReplyFinished(QNetworkReply* reply);

   private:
    QNetworkAccessManager* manager_;
    QString base_url_;
};

#endif  // LANGUAGETOOL_CLIENT_H
