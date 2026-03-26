#ifndef SOUND_HPP
#define SOUND_HPP

#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>

namespace amb {
    class Sound;
}

class amb::Sound : public QObject {
    Q_OBJECT

    Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged)
    Q_PROPERTY(QUrl path MEMBER path NOTIFY pathChanged)
    Q_PROPERTY(double volume MEMBER volume NOTIFY volumeChanged)
    Q_PROPERTY(bool loaded MEMBER loaded NOTIFY loadedChanged)

public:
    using QObject::QObject;
    void load();
    void play();
    void stop();

public slots:
    void onPlayerLoaded(QMediaPlayer* player, QAudioOutput* output);
    void updateVolume();

signals:
    void nameChanged();
    void pathChanged();
    void volumeChanged();
    void loadedChanged();

private:
    QMediaPlayer* player;
    QAudioOutput* output;
    QThread* loadThread;

    QString name;
    QUrl path;
    double volume = 1;
    bool loaded = false;
};

#endif // SOUND_HPP
