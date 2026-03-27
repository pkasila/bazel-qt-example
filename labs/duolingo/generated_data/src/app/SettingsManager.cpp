#include "SettingsManager.h"
#include <QSettings> // Example for QSettings
#include <QDebug> // Added for qDebug

SettingsManager& SettingsManager::instance() {
    static SettingsManager inst;
    return inst;
}

SettingsManager::SettingsManager() {
    // m_settings = new QSettings("YourOrg", "LanguageApp", this); // If QObject based
    // loadSettings();
}

void SettingsManager::setSetting(const QString& key, const QVariant& value) {
    // m_settings->setValue(key, value);
    qDebug("Setting %s to %s", qUtf8Printable(key), qUtf8Printable(value.toString()));
}

QVariant SettingsManager::getSetting(const QString& key, const QVariant& defaultValue) const {
    // return m_settings->value(key, defaultValue);
    qDebug("Getting %s, returning default for now", qUtf8Printable(key));
    return defaultValue;
}

QString SettingsManager::getCurrentLanguage() const {
    return getSetting("current_language", "en").toString();
}

void SettingsManager::setCurrentLanguage(const QString& langCode) {
    setSetting("current_language", langCode);
}

int SettingsManager::getCurrentDifficulty() const {
    return getSetting("current_difficulty", 0).toInt(); // 0 = Easy
}

void SettingsManager::setCurrentDifficulty(int level) {
    setSetting("current_difficulty", level);
}


void SettingsManager::loadSettings() {
    // Load settings from persistent storage
}

void SettingsManager::saveSettings() {
    // Save settings to persistent storage
}