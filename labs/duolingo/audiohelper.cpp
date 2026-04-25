#include "audiohelper.h"

#include <QCoreApplication>
#include <QDebug>
#include <QDir>
#include <QFileInfo>
#include <QProcessEnvironment>
#include <QUrl>

AudioPlayer::AudioPlayer(QObject* parent)
    : QObject(parent), correctSound(this), wrongSound(this) {
    correctSound.setVolume(1.0f);
    wrongSound.setVolume(1.0f);
}

bool AudioPlayer::load() {
    const QString correctPath = findSoundFile("labs/duolingo/correct.wav");
    const QString wrongPath = findSoundFile("labs/duolingo/wrong.wav");

    correctLoaded = !correctPath.isEmpty();
    wrongLoaded = !wrongPath.isEmpty();

    if (correctLoaded) {
        configureSound(correctSound, correctPath);
    }
    if (wrongLoaded) {
        configureSound(wrongSound, wrongPath);
    }

    if (isReady()) {
        lastStatus = QString("Sound ready: %1, %2").arg(correctPath, wrongPath);
    } else {
        lastStatus = "Sound files were not found. Put correct.wav and wrong.wav near the executable, run through Bazel data, or set DUOLINGO_SOUND_DIR.";
        qWarning() << lastStatus;
    }

    return isReady();
}

bool AudioPlayer::isReady() const {
    return correctLoaded && wrongLoaded;
}

QString AudioPlayer::statusText() const {
    return lastStatus;
}

void AudioPlayer::playCorrect() {
    if (correctLoaded) {
        correctSound.play();
    }
}

void AudioPlayer::playWrong() {
    if (wrongLoaded) {
        wrongSound.play();
    }
}

void AudioPlayer::configureSound(QSoundEffect& sound, const QString& filePath) {
    sound.setSource(QUrl::fromLocalFile(filePath));
    sound.setLoopCount(1);
    sound.setVolume(1.0f);
}

QString AudioPlayer::findSoundFile(const QString& fileName) const {
    QStringList roots;
    auto addRoot = [&roots](const QString& root) {
        if (root.isEmpty()) {
            return;
        }
        const QString clean = QDir::cleanPath(root);
        if (!roots.contains(clean)) {
            roots << clean;
        }
    };

    const QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    addRoot(env.value("DUOLINGO_SOUND_DIR"));
    addRoot(QCoreApplication::applicationDirPath());
    addRoot(QDir::currentPath());

    const QString appDir = QCoreApplication::applicationDirPath();
    addRoot(QDir(appDir).absoluteFilePath(".."));
    addRoot(QDir(appDir).absoluteFilePath("../.."));
    addRoot(QDir(appDir).absoluteFilePath("../../.."));

    addRoot(env.value("RUNFILES_DIR"));
    addRoot(env.value("TEST_SRCDIR"));

    const QStringList subDirs = {
        "",
        "duolingo",
        "_main/duolingo",
        "__main__/duolingo",
        "main/duolingo",
        "labs/basics/duolingo",
        "_main/labs/basics/duolingo",
        "__main__/labs/basics/duolingo",
        "main/labs/basics/duolingo"
    };

    for (const QString& root : roots) {
        for (const QString& subDir : subDirs) {
            const QString candidate = QDir(root).absoluteFilePath(subDir.isEmpty() ? fileName : subDir + "/" + fileName);
            if (QFileInfo::exists(candidate)) {
                return QDir::cleanPath(candidate);
            }
        }
    }

    return {};
}
