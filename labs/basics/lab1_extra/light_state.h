#ifndef LAB_LIGHT_STATE_H_
#define LAB_LIGHT_STATE_H_

class LightState {
 public:
  LightState(int brightness = 100, int color_temperature = 5000,
             bool is_enabled = true)
      : brightness_(brightness),
        color_temperature_(color_temperature),
        is_enabled_(is_enabled) {}

  int GetBrightness() const { return brightness_; }
  void SetBrightness(int brightness) {
    brightness_ = brightness < 0 ? 0 : (brightness > 100 ? 100 : brightness);
  }

  int GetColorTemperature() const { return color_temperature_; }
  void SetColorTemperature(int kelvin) {
    color_temperature_ = kelvin < 3000 ? 3000 : (kelvin > 8000 ? 8000 : kelvin);
  }

  bool IsEnabled() const { return is_enabled_; }
  void SetEnabled(bool enabled) { is_enabled_ = enabled; }

 private:
  int brightness_;
  int color_temperature_;
  bool is_enabled_;
};

#endif  // LAB_LIGHT_STATE_H_
