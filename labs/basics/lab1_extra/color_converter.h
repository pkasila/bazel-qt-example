#ifndef LAB_COLOR_CONVERTER_H_
#define LAB_COLOR_CONVERTER_H_

#include <QColor>
#include <algorithm>

class ColorConverter {
 public:
  /**
   * Converts color temperature in Kelvin to RGB color.
   * Based on simplified blackbody radiation model.
   *
   * @param kelvin Color temperature (3000-8000K where 3000 is warm, 8000 is cool)
   * @return QColor with appropriate RGB values
   */
  static QColor TemperatureToColor(int kelvin) {
    kelvin = std::max(1000, std::min(10000, kelvin));

    int red, green, blue;

    if (kelvin < 4000) {
      // Warm lights: more red and orange
      red = 255;
      green = static_cast<int>(200 - (4000 - kelvin) * 0.02);
      blue = static_cast<int>(150 - (4000 - kelvin) * 0.03);
    } else if (kelvin < 6500) {
      // Neutral white
      red = 255;
      green = static_cast<int>(220 + (kelvin - 4000) * 0.015);
      blue = static_cast<int>(180 + (kelvin - 4000) * 0.01);
    } else {
      // Cool lights: more blue
      red = static_cast<int>(255 - (kelvin - 6500) * 0.01);
      green = static_cast<int>(235 - (kelvin - 6500) * 0.005);
      blue = 255;
    }

    red = std::max(0, std::min(255, red));
    green = std::max(0, std::min(255, green));
    blue = std::max(0, std::min(255, blue));

    return QColor(red, green, blue);
  }

  /**
   * Applies brightness to a color and returns the dimmed version.
   *
   * @param color Base color
   * @param brightness_percent Brightness level (0-100)
   * @return Dimmed QColor
   */
  static QColor ApplyBrightness(const QColor& color, int brightness_percent) {
    brightness_percent = std::max(0, std::min(100, brightness_percent));
    QColor result = color;
    result.setHsv(result.hue(), result.saturation(),
                  (result.value() * brightness_percent) / 100);
    return result;
  }
};

#endif  // LAB_COLOR_CONVERTER_H_
