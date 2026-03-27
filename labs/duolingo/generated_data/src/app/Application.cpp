#include "Application.h"
#include "app/SettingsManager.h"
#include "db/DatabaseManager.h"
#include <QDebug> // For qInfo/qCritical

Application::Application(int &argc, char **argv) : QApplication(argc, argv) {
    QApplication::setApplicationName("LanguageAppDuolingo");
    QApplication::setOrganizationName("YourOrganization");
    qInfo() << "Application: Name set to" << QApplication::applicationName();
    qInfo() << "Application: Organization set to" << QApplication::organizationName();


    // Initialize singletons or global services here
    qInfo() << "Application: Initializing SettingsManager...";
    SettingsManager::instance().loadSettings();
    qInfo() << "Application: SettingsManager initialized.";

    qInfo() << "Application: Initializing DatabaseManager...";
    if (!DatabaseManager::instance().connect()) { // Default DB name "langapp.sqlite" will be used
        qCritical() << "Application: CRITICAL FAILURE: Failed to connect to the database.";
        qCritical() << "Application: The application may not function correctly or at all.";
        // In a production app, you might want to show a message box to the user and exit.
        // For example: QMessageBox::critical(nullptr, "Database Error", "Could not initialize the database. The application will now exit.");
        // exit(1); // Or handle this more gracefully
    } else {
        qInfo() << "Application: DatabaseManager connection sequence completed successfully.";
    }
    qInfo() << "Application: DatabaseManager initialization attempted."; // Signal that process is done
}

Application::~Application() {
    qInfo() << "Application: Destructor called. Saving settings and disconnecting DB.";
    // It's good practice to disconnect DB before SettingsManager saves,
    // in case settings depend on DB state (though not in this app)
    DatabaseManager::instance().disconnect();
    SettingsManager::instance().saveSettings();
    qInfo() << "Application: Cleanup complete.";
}