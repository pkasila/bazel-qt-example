#ifndef TASKDIFFICULTY_H
#define TASKDIFFICULTY_H

#include <QString>
#include <cstdint>

enum class TaskDifficulty : uint8_t { Easy = 0, Medium, Hard };

[[nodiscard]] QString AsRussianLiteral(const TaskDifficulty&);

#endif  // TASKDIFFICULTY_H
