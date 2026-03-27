#include "DatabaseManager.h"
#include <QSqlQuery>
#include <QSqlError>
#include <QDebug>
#include <QFile>
#include <QStandardPaths>
#include <QDir>
#include <QTextStream>
#include <QApplication> // For organizationName and applicationName

DatabaseManager& DatabaseManager::instance() {
    static DatabaseManager inst;
    return inst;
}

DatabaseManager::DatabaseManager() : m_connectionName("LanguageAppDBConnection") {
    qInfo() << "DatabaseManager: Instance created.";
}

DatabaseManager::~DatabaseManager() {
    qInfo() << "DatabaseManager: Instance being destroyed.";
    disconnect();
}

bool DatabaseManager::connect(const QString& dbName) {
    qInfo() << "DatabaseManager: connect() called with dbName:" << dbName;

    if (QSqlDatabase::contains(m_connectionName)) {
        qInfo() << "DatabaseManager: Connection" << m_connectionName << "already exists. Using it.";
        m_db = QSqlDatabase::database(m_connectionName);
    } else {
        qInfo() << "DatabaseManager: Adding new database connection" << m_connectionName << "with driver QSQLITE.";
        m_db = QSqlDatabase::addDatabase("QSQLITE", m_connectionName);
        if (!m_db.isValid()) {
            qCritical() << "DatabaseManager: CRITICAL: QSQLITE driver not valid!";
            qCritical() << "DatabaseManager: m_db.lastError() after addDatabase:" << m_db.lastError().text();
            qCritical() << "DatabaseManager: CRITICAL: Ensure SQLite plugin (e.g., qsqlite.dll/libqsqlite.so) is available and in the correct path.";
            return false;
        }
        qInfo() << "DatabaseManager: QSQLITE driver added and valid for connection" << m_connectionName;
    }

    QString actualDbPath = dbName;
    if (dbName == "langapp.sqlite") {
        qInfo() << "DatabaseManager: Using default DB name 'langapp.sqlite', resolving standard path.";
        if (QApplication::organizationName().isEmpty()) {
            qWarning() << "DatabaseManager: OrganizationName not set by Application class prior to DB init. Using default 'YourOrganization'.";
        }
        if (QApplication::applicationName().isEmpty()) {
             qWarning() << "DatabaseManager: ApplicationName not set by Application class prior to DB init. Using default 'LanguageAppDuolingo'.";
        }

        QString dataPath = QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
        qInfo() << "DatabaseManager: Attempting QStandardPaths::AppLocalDataLocation:" << dataPath;

        if (dataPath.isEmpty()) {
            qWarning() << "DatabaseManager: AppLocalDataLocation is empty. Trying AppDataLocation.";
            dataPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
            qInfo() << "DatabaseManager: Attempting QStandardPaths::AppDataLocation (fallback 1):" << dataPath;
        }
        if (dataPath.isEmpty()) {
            qWarning() << "DatabaseManager: Both AppLocalDataLocation and AppDataLocation are empty. Using current directory '.' as last resort.";
            dataPath = ".";
        }
        qInfo() << "DatabaseManager: Selected data path for DB:" << dataPath;

        QDir dir(dataPath);
        if (!dir.exists()) {
            qInfo() << "DatabaseManager: Data directory" << dataPath << "does not exist. Attempting to create it.";
            if (!dir.mkpath(".")) {
                 qWarning() << "DatabaseManager: WARNING: Could not create data directory:" << dataPath;
            } else {
                qInfo() << "DatabaseManager: Data directory" << dataPath << "created successfully.";
            }
        } else {
            qInfo() << "DatabaseManager: Data directory" << dataPath << "already exists.";
        }
        actualDbPath = dataPath + QDir::separator() + dbName;
    }

    qInfo() << "DatabaseManager: Setting database name to:" << actualDbPath;
    m_db.setDatabaseName(actualDbPath);

    qInfo() << "DatabaseManager: Attempting to open database...";
    if (!m_db.open()) {
        qCritical() << "DatabaseManager: CRITICAL: Database file open error:" << m_db.lastError().text();
        qCritical() << "DatabaseManager: CRITICAL: Database path attempted:" << actualDbPath;
        return false;
    }
    qInfo() << "DatabaseManager: Database file opened successfully at:" << m_db.databaseName();

    qInfo() << "DatabaseManager: Attempting to create tables if they do not exist...";
    if (!createTablesIfNotExist()) {
        qCritical() << "DatabaseManager: CRITICAL: Failed to process database schema. Database might be in an inconsistent state.";
        return false;
    }
    qInfo() << "DatabaseManager: Database schema processed. Connection successful and database initialized.";
    return true;
}

void DatabaseManager::disconnect() {
    if (m_db.isOpen()) {
        qInfo() << "DatabaseManager: Closing database connection:" << m_connectionName << "for" << m_db.databaseName();
        m_db.close();
    }
    if (QSqlDatabase::contains(m_connectionName)) {
         QSqlDatabase::removeDatabase(m_connectionName);
         qInfo() << "DatabaseManager: Removed database connection" << m_connectionName << "from global list.";
    }
    qInfo() << "DatabaseManager: Disconnect sequence complete.";
}

bool DatabaseManager::isConnected() const {
    bool connected = m_db.isValid() && m_db.isOpen();
    return connected;
}

QSqlDatabase DatabaseManager::database() const {
    return m_db;
}

bool DatabaseManager::executeQuery(QSqlQuery& query) const {
    if (!isConnected()) {
        qWarning("DatabaseManager: Database not connected. Cannot execute query (QSqlQuery object).");
        return false;
    }
    if (!query.exec()) {
        qWarning() << "DatabaseManager: Query execution failed (QSqlQuery object):" << query.lastError().text();
        qWarning() << "DatabaseManager: Offending Query:" << query.lastQuery();
        if (!query.boundValues().isEmpty()) {
            qWarning() << "DatabaseManager: Bound values:" << query.boundValues();
        }
        return false;
    }
    return true;
}

bool DatabaseManager::executeQuery(const QString& queryString) {
    if (!isConnected()) {
        qWarning("DatabaseManager: Database not connected. Cannot execute query (QString).");
        return false;
    }
    QSqlQuery query(m_db);
    if (!query.exec(queryString)) {
        qWarning() << "DatabaseManager: Query execution failed (QString):" << query.lastError().text();
        qWarning() << "DatabaseManager: Offending Query:" << queryString;
        return false;
    }
    return true;
}

// Implementation of the helper method
bool DatabaseManager::executeSchemaStatements(const QStringList& statements, const QString& passName) {
    QSqlQuery query(m_db);
    int stmtNumber = 0;
    for (const QString& stmt : statements) {
        stmtNumber++;
        QString trimmedStmt = stmt.trimmed();

        if (trimmedStmt.isEmpty() || trimmedStmt.startsWith("--")) {
            continue;
        }
        // Special handling for PRAGMA foreign_keys = ON to ensure it's attempted if it's the first statement of the first pass.
        // This specific PRAGMA should ideally be run outside a transaction or as the very first thing.
        // However, within the loop, we just execute it.
        // The logic to enforce it as the *very* first overall statement is now in createTablesIfNotExist.

        qInfo().noquote() << QString("DatabaseManager: %1 - Executing schema statement #%2: %3").arg(passName).arg(stmtNumber).arg(trimmedStmt.left(120) + (trimmedStmt.length() > 120 ? "..." : ""));
        if (!query.exec(trimmedStmt)) {
            qWarning() << "DatabaseManager: " << passName << " - ERROR: Failed to execute schema statement #" << stmtNumber << ":" << query.lastError().text();
            qWarning() << "DatabaseManager: " << passName << " - Offending Statement:" << trimmedStmt;
            return false; // Stop on first error in this pass
        } else {
            qInfo() << "DatabaseManager: " << passName << " - Schema statement #" << stmtNumber << "executed successfully. Rows affected:" << query.numRowsAffected();
        }
    }
    return true;
}


bool DatabaseManager::createTablesIfNotExist(const QString& schemaFilePath) {
    qInfo() << "DatabaseManager: createTablesIfNotExist() called with schema file:" << schemaFilePath;
    if (!isConnected()) {
        qWarning("DatabaseManager: Database not connected. Cannot create tables.");
        return false;
    }

    QFile schemaFile(schemaFilePath);
    if (!schemaFile.exists()) {
        qCritical() << "DatabaseManager: CRITICAL: Schema file not found in resources:" << schemaFilePath;
        return false;
    }
    qInfo() << "DatabaseManager: Schema file" << schemaFilePath << "found in resources.";

    if (!schemaFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "DatabaseManager: WARNING: Could not open schema file from resources:" << schemaFile.errorString();
        return false;
    }
    qInfo() << "DatabaseManager: Schema file opened successfully for reading.";

    QTextStream in(&schemaFile);
    QString schemaContent = in.readAll();
    schemaFile.close();

    if (schemaContent.trimmed().isEmpty()) {
        qWarning() << "DatabaseManager: WARNING: Schema file" << schemaFilePath << "is empty.";
        return true;
    }
    qInfo() << "DatabaseManager: Schema content read. Total length:" << schemaContent.length();

    QStringList allStatements = schemaContent.split(';', Qt::SkipEmptyParts);
    qInfo() << "DatabaseManager: Schema split into" << allStatements.count() << "statements.";

    QStringList tableCreationStatements;
    QStringList dataInsertionStatements;

    // Ensure PRAGMA foreign_keys = ON is the very first statement to be executed.
    bool pragmaFoundAndFirst = false;
    if (!allStatements.isEmpty() && allStatements.first().trimmed().toUpper().startsWith("PRAGMA FOREIGN_KEYS = ON")) {
        tableCreationStatements.append(allStatements.first());
        allStatements.removeFirst(); // Remove it so it's not processed again
        pragmaFoundAndFirst = true;
        qInfo() << "DatabaseManager: 'PRAGMA foreign_keys = ON' found as first statement and added to Pass 1.";
    } else {
        // If not found or not first, add it manually to the beginning of the table creation pass.
        // This ensures it's executed before any table creation or data insertion within the transaction.
        tableCreationStatements.prepend("PRAGMA foreign_keys = ON;");
        qWarning() << "DatabaseManager: 'PRAGMA foreign_keys = ON;' was not the first statement or was missing. Prepended it to Pass 1.";
    }


    for (const QString& stmt : allStatements) {
        QString upperStmt = stmt.trimmed().toUpper();
        if (upperStmt.startsWith("CREATE TABLE")) {
            tableCreationStatements.append(stmt);
        } else if (upperStmt.startsWith("INSERT")) {
            dataInsertionStatements.append(stmt);
        } else if (!upperStmt.startsWith("PRAGMA") && !upperStmt.startsWith("--") && !upperStmt.isEmpty()) {
            // Other statements (e.g. CREATE INDEX, other PRAGMAs) put them with table creations for now
            qInfo() << "DatabaseManager: Adding non-CREATE/INSERT/PRAGMA statement to table creation pass:" << stmt.trimmed().left(60);
            tableCreationStatements.append(stmt);
        }
    }

    qInfo() << "DatabaseManager: Pass 1 (Tables and PRAGMA) statements count:" << tableCreationStatements.count();
    qInfo() << "DatabaseManager: Pass 2 (Data Inserts) statements count:" << dataInsertionStatements.count();


    qInfo() << "DatabaseManager: Attempting to start transaction for schema processing...";
    if(!m_db.transaction()) {
        qWarning() << "DatabaseManager: WARNING: Failed to start transaction. DB Error:" << m_db.lastError().text();
        return false;
    }
    qInfo() << "DatabaseManager: Transaction started successfully.";

    bool overallSuccess = true;

    // Pass 1: Create tables (and execute PRAGMA)
    if (!executeSchemaStatements(tableCreationStatements, "Pass 1 (Tables and PRAGMA)")) {
        overallSuccess = false;
    }

    // Pass 2: Insert data (only if Pass 1 was successful)
    if (overallSuccess) {
        if (!executeSchemaStatements(dataInsertionStatements, "Pass 2 (Data Inserts)")) {
            overallSuccess = false;
        }
    }

    if (overallSuccess) {
        qInfo() << "DatabaseManager: All schema passes completed. Attempting to commit transaction...";
        if (!m_db.commit()) {
            qWarning() << "DatabaseManager: WARNING: Failed to commit schema transaction:" << m_db.lastError().text();
            overallSuccess = false;
            qInfo() << "DatabaseManager: Attempting to rollback transaction due to commit failure...";
            if (!m_db.rollback()) {
                 qWarning() << "DatabaseManager: CRITICAL: Rollback also failed:" << m_db.lastError().text();
            } else {
                 qInfo() << "DatabaseManager: Rollback successful after commit failure.";
            }
        } else {
            qInfo("DatabaseManager: Database schema transaction committed successfully.");
        }
    } else {
        qWarning("DatabaseManager: Database schema processing failed in one of the passes. Attempting to rollback transaction...");
        if (!m_db.rollback()) {
            qWarning() << "DatabaseManager: CRITICAL: Rollback failed after pass error:" << m_db.lastError().text();
        } else {
            qInfo() << "DatabaseManager: Rollback successful after pass error.";
        }
    }
    return overallSuccess;
}