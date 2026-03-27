
#include "Ray.h"

[[nodiscard]] Ray Ray::Rotate(double angle) const {
    auto a = end_ - begin_;
    return {
      begin_, begin_ + QPointF(
                           ((a.x() * std::cos(angle)) - (a.y() * std::sin(angle))),
                           ((a.x() * std::sin(angle)) + (a.y() * std::cos(angle))))};
}
