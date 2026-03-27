#ifndef DATABASEMANAGER_H
#define DATABASEMANAGER_H

#include <QSqlDatabase>
#include <QString>
#include <QVariant>
#include <QSqlQuery>
#include <QStringList> // Added for executeSchemaStatements declaration

class DatabaseManager {
public:
    static DatabaseManager& instance();

    bool connect(const QString& dbPath = "langapp.sqlite");
    void disconnect();
    bool isConnected() const;

    QSqlDatabase database() const;

    bool executeQuery(QSqlQuery& query) const;
    bool executeQuery(const QString& queryString);

    bool createTablesIfNotExist(const QString& schemaFilePath = ":/db/schema.sql");

private:
    DatabaseManager();
    ~DatabaseManager();
    DatabaseManager(const DatabaseManager&) = delete;
    DatabaseManager& operator=(const DatabaseManager&) = delete;

    // Helper method for executing schema statements in passes
    bool executeSchemaStatements(const QStringList& statements, const QString& passName); // Declaration added

    QSqlDatabase m_db;
    QString m_connectionName;
};

#endif // DATABASEMANAGER_H