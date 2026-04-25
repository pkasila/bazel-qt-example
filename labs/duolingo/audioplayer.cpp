#include "audioplayer.h"
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QDebug>
#include <QProcessEnvironment>

AudioPlayer::AudioPlayer() {
    successSound = std::make_unique<QSoundEffect>();
    failSound = std::make_unique<QSoundEffect>();
    menuMusic = std::make_unique<QSoundEffect>();
    gameMusic = std::make_unique<QSoundEffect>();
    lowHpMusic = std::make_unique<QSoundEffect>();

    loadSound(successSound.get(), "correct.wav");
    loadSound(failSound.get(), "wrong.wav");
    
    loadSound(menuMusic.get(), "menu_bg.wav");
    menuMusic->setLoopCount(QSoundEffect::Infinite);
    menuMusic->setVolume(0.5f); // Background music should be softer
    
    loadSound(gameMusic.get(), "game_bg.wav");
    gameMusic->setLoopCount(QSoundEffect::Infinite);
    gameMusic->setVolume(0.4f);
    
    loadSound(lowHpMusic.get(), "game_bg_low.wav");
    lowHpMusic->setLoopCount(QSoundEffect::Infinite);
    lowHpMusic->setVolume(0.6f); // A bit louder for stress
}

void AudioPlayer::loadSound(QSoundEffect* effect, const QString& filename) {
    QStringList searchPaths;
    searchPaths << QCoreApplication::applicationDirPath() + "/" + filename;
    searchPaths << QDir::currentPath() + "/" + filename;
    searchPaths << QDir::currentPath() + "/labs/duolingo/" + filename;
    
    // Also look up in runfiles
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    QString runfilesDir = env.value("RUNFILES_DIR");
    if (!runfilesDir.isEmpty()) {
        searchPaths << runfilesDir + "/_main/labs/duolingo/" + filename;
        searchPaths << runfilesDir + "/bazel-qt-projects/labs/duolingo/" + filename;
    }

    bool found = false;
    for (const QString& path : searchPaths) {
        if (QFile::exists(path)) {
            effect->setSource(QUrl::fromLocalFile(path));
            found = true;
            break;
        }
    }

    if (!found) {
        qWarning() << "Audio file not found:" << filename << ". Running in silent mode.";
    }
}

void AudioPlayer::playSuccess() {
    if (successSound->source().isValid()) successSound->play();
}

void AudioPlayer::playFail() {
    if (failSound->source().isValid()) failSound->play();
}

void AudioPlayer::stopAllMusic() {
    menuMusic->stop();
    gameMusic->stop();
    lowHpMusic->stop();
}

void AudioPlayer::playMenuMusic() {
    stopAllMusic();
    if (menuMusic->source().isValid()) menuMusic->play();
}

void AudioPlayer::playGameMusic() {
    stopAllMusic();
    if (gameMusic->source().isValid()) gameMusic->play();
}

void AudioPlayer::playLowHpMusic() {
    stopAllMusic();
    if (lowHpMusic->source().isValid()) lowHpMusic->play();
}

void AudioPlayer::setMuted(bool muted) {
    qreal vol = muted ? 0.0 : 1.0;
    if (menuMusic) menuMusic->setVolume(vol * 0.5f);
    if (gameMusic) gameMusic->setVolume(vol * 0.4f);
    if (lowHpMusic) lowHpMusic->setVolume(vol * 0.6f);
    if (successSound) successSound->setVolume(vol);
    if (failSound) failSound->setVolume(vol);
}
