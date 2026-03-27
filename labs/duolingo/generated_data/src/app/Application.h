#ifndef APPLICATION_H
#define APPLICATION_H

#include <QApplication>

class Application : public QApplication {
    Q_OBJECT
public:
    Application(int &argc, char **argv);
    ~Application();

    // TODO: Add methods for accessing global services if needed
    // static SettingsManager& settingsManager();
    // static DatabaseManager& databaseManager();
};

#endif // APPLICATION_H