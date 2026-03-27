#ifndef AUDIOPLAYER_H
#define AUDIOPLAYER_H

#include "Sound.h"

#include <QAudioOutput>
#include <QMediaPlayer>
#include <QObject>

class AudioPlayer : public QObject {
    Q_OBJECT
   public:
    AudioPlayer();

    void PlaySound(const Sound::Type&);

    void Shutdown() const;

   private:
    QMediaPlayer* player_ = nullptr;
    QAudioOutput* output_ = nullptr;
    Sound::Type sound_ = Sound::Chill;
};

#endif  // AUDIOPLAYER_H
