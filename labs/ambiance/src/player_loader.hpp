#ifndef PLAYER_LOADER_HPP
#define PLAYER_LOADER_HPP

#include <QObject>
#include <QMediaPlayer>
#include <QAudioOutput>

namespace amb {
    class PlayerLoader;
}

class amb::PlayerLoader : public QObject {
    Q_OBJECT

signals:
    void loaded(QMediaPlayer* player, QAudioOutput* output);

public slots:
    void load();

public:
    void setSourceThread(QThread* thread);

private:
    QThread* sourceThread = nullptr;
};

#endif // PLAYER_LOADER_HPP
