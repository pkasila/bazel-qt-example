#ifndef SOUND_H
#define SOUND_H

#include <QUrl>
#include <cstdint>

namespace Sound {
enum Type : uint8_t { Chill = 0, Fail, Success };

QUrl GetSoundUrl(const Type&);

};  // namespace Sound

#endif  // SOUND_H
