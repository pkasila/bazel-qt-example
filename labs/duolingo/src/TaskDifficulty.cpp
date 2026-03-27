#include "TaskDifficulty.h"

QString AsRussianLiteral(const TaskDifficulty& difficulty) {
    switch (difficulty) {
        case TaskDifficulty::Easy:
            return QString("Легкая");
        case TaskDifficulty::Medium:
            return QString("Средняя");
        case TaskDifficulty::Hard:
            return QString("Сложная");
        default:;
    }
    return {};
}