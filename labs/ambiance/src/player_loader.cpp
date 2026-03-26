#include "player_loader.hpp"

void amb::PlayerLoader::load() {
    QMediaPlayer* player = new QMediaPlayer();
    QAudioOutput* output = new QAudioOutput();
    player->setAudioOutput(output);
    player->moveToThread(sourceThread);
    output->moveToThread(sourceThread);
    emit loaded(player, output);
}

void amb::PlayerLoader::setSourceThread(QThread* thread) {
    this->sourceThread = thread;
}
