#include "Sound.h"

#include <QUrl>

QUrl Sound::GetSoundUrl(const Type& type) {
    QString path = "labs/duolingo/data/";
    switch (type) {
        case Chill:
            path += "chill";
            break;
        case Fail:
            path += "fail";
            break;
        case Success:
            path += "success";
            break;
        default:;
    }
    path += ".wav";
    return QUrl::fromLocalFile(path);
}
