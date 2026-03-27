#ifndef SETTINGSMANAGER_H
#define SETTINGSMANAGER_H

#include <QString>
#include <QVariant>

// Basic stub for settings
class SettingsManager {
public:
    static SettingsManager& instance();

    void setSetting(const QString& key, const QVariant& value);
    QVariant getSetting(const QString& key, const QVariant& defaultValue = QVariant()) const;

    // Example settings
    QString getCurrentLanguage() const;
    void setCurrentLanguage(const QString& langCode); // e.g., "en", "fr"

    int getCurrentDifficulty() const; // e.g., 0 for Easy, 1 for Medium
    void setCurrentDifficulty(int level);

    void loadSettings();
    void saveSettings();

private:
    SettingsManager();
    SettingsManager(const SettingsManager&) = delete;
    SettingsManager& operator=(const SettingsManager&) = delete;

    // Store settings (e.g., in QSettings or a file)
    // QSettings* m_settings;
};

#endif // SETTINGSMANAGER_H