#pragma once

#include <QSoundEffect>
#include <memory>
#include <QString>

class AudioPlayer {
public:
    AudioPlayer();
    
    void playSuccess();
    void playFail();
    
    // Background music controls
    void playMenuMusic();
    void playGameMusic();
    void playLowHpMusic();
    void stopAllMusic();
    void setMuted(bool muted);

private:
    void loadSound(QSoundEffect* effect, const QString& filename);

    std::unique_ptr<QSoundEffect> successSound;
    std::unique_ptr<QSoundEffect> failSound;
    
    std::unique_ptr<QSoundEffect> menuMusic;
    std::unique_ptr<QSoundEffect> gameMusic;
    std::unique_ptr<QSoundEffect> lowHpMusic;
};
