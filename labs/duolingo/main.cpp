#include "authdialog.h"
#include "datamanager.h"
#include "mainwindow.h"
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QMessageBox>
#include <QStandardPaths>
#include <QString>

int main(int argc, char* argv[]) {
    QApplication a(argc, argv);

    QCoreApplication::setOrganizationName("YourCompany");
    QCoreApplication::setApplicationName("DuolingoApp");

    if (QFile styleFile(":/styles/duolingo_style.qss"); styleFile.open(QFile::ReadOnly)) {
        QString styleSheet = QLatin1String(styleFile.readAll());
        a.setStyleSheet(styleSheet);
        styleFile.close();
    }

    DataManager& dbManager = DataManager::Instance();
    QObject::connect(
        &dbManager, &DataManager::DatabaseError,
        [](const QString& errorMsg) {
            QMessageBox::critical(
                nullptr, "Ошибка базы данных",
                "Произошла ошибка при работе с базой данных:\n" + errorMsg +
                    "\nПриложение может работать некорректно.");
        });

    QString dataPath =
        QStandardPaths::writableLocation(QStandardPaths::AppLocalDataLocation);
    if (dataPath.isEmpty()) {
        dataPath = QCoreApplication::applicationDirPath();
    }

    if (QDir dataDir(dataPath); !dataDir.exists()) {
        if (!dataDir.mkpath(".")) {
            QMessageBox::critical(nullptr, "Критическая ошибка",
                                  "Не удалось создать директорию для данных "
                                  "приложения. Приложение будет закрыто.");
            return -1;
        }
    }

    if (QString dbFilePath = dataPath + QDir::separator() + "duolingo_data.db";
        !dbManager.OpenDb(dbFilePath)) {
        return -1;
    }

    if (AuthDialog authDialog; authDialog.exec() == QDialog::Accepted) {
        long long currentUserId = -1;
        currentUserId = authDialog.CurrentUserId();
        if (currentUserId != -1) {
            dbManager.SetCurrentUserId(currentUserId);
        } else {
            QMessageBox::critical(nullptr, "Ошибка входа",
                                  "Произошла внутренняя ошибка при входе. "
                                  "Приложение будет закрыто.");
            return -1;
        }
    } else {
        return 0;
    }

    MainWindow window;
    window.show();

    int result = a.exec();

    dbManager.CloseDb();
    return result;
}