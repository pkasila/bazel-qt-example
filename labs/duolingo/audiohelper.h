#pragma once

#include <QObject>
#include <QString>
#include <QSoundEffect>

class AudioPlayer : public QObject {
    Q_OBJECT

public:
    explicit AudioPlayer(QObject* parent = nullptr);

    bool load();
    bool isReady() const;
    QString statusText() const;

    void playCorrect();
    void playWrong();

private:
    QString findSoundFile(const QString& fileName) const;
    void configureSound(QSoundEffect& sound, const QString& filePath);

    QSoundEffect correctSound;
    QSoundEffect wrongSound;
    bool correctLoaded = false;
    bool wrongLoaded = false;
    QString lastStatus;
};
