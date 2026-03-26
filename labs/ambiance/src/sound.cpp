#include <QGuiApplication>
#include <QThread>
#include "sound.hpp"
#include "player_loader.hpp"

void amb::Sound::load() {
    connect(this, &Sound::volumeChanged, this, &Sound::updateVolume);

    loadThread = new QThread(this);
    PlayerLoader* loader = new PlayerLoader();
    loader->setSourceThread(thread());
    loader->moveToThread(loadThread);

    connect(loadThread, &QThread::started, loader, &PlayerLoader::load);
    connect(loader, &PlayerLoader::loaded, this, &Sound::onPlayerLoaded);
    connect(loadThread, &QThread::finished, loadThread, &QThread::deleteLater);
    connect(loadThread, &QThread::finished, loader, &PlayerLoader::deleteLater);

    loadThread->start();
}

void amb::Sound::onPlayerLoaded(QMediaPlayer* player, QAudioOutput* output) {
    this->player = player;
    this->output = output;
    player->setParent(this);
    output->setParent(this);

    loaded = true;
    play();
    emit loadedChanged();

    // Causes sound to crash for some reason I don't understand
    // loadThread->quit();
}

void amb::Sound::play() {
    if(!loaded) {
        load();
        return;
    }
    player->setSource(path);
    player->setLoops(QMediaPlayer::Infinite);
    output->setVolume(static_cast<float>(volume));
    player->play();
}

void amb::Sound::stop() {
    player->stop();
}

void amb::Sound::updateVolume() {
    output->setVolume(static_cast<float>(volume));
}
