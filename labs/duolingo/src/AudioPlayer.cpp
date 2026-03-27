#include "AudioPlayer.h"

#include "Sound.h"

#include <QAudioOutput>
#include <QMediaPlayer>

AudioPlayer::AudioPlayer() : player_(new QMediaPlayer(this)), output_(new QAudioOutput(this)) {
    output_->setVolume(0.5f);
    player_->setAudioOutput(output_);
}

void AudioPlayer::PlaySound(const Sound::Type& sound) {
    player_->setSource(Sound::GetSoundUrl(sound));
    sound_ = sound;
    connect(
        player_, &QMediaPlayer::mediaStatusChanged, this, [this](QMediaPlayer::MediaStatus status) {
            if (sound_ == Sound::Chill && status == QMediaPlayer::EndOfMedia) {
                player_->setPosition(0);
                player_->play();
            }
        });
    player_->play();
}

void AudioPlayer::Shutdown() const {
    player_->stop();
    player_->setSource({});
}
