#ifndef LAB_LIGHTING_CONTROLLER_H_
#define LAB_LIGHTING_CONTROLLER_H_

#include <QMainWindow>

class LightingController : public QMainWindow {
  Q_OBJECT

 public:
  LightingController();

 private:
  void SetupUI();
  void ConnectSignals();
  void ApplyLightSettings();
  void UpdateColorPreview();
  void UpdateInfoDisplay();
  void UpdateStatus(const QString& message);

  void OnRoomChanged(const QString& room_name);
  void OnPowerToggled(bool is_checked);
  void OnBrightnessChanged(int value);
  void OnColorTemperatureChanged(int value);
  void OnWarmPresetClicked();
  void OnNeutralPresetClicked();
  void OnCoolPresetClicked();
  void OnSavePresetClicked();

 private:
  class Impl;
  Impl* impl_;
};

#endif  // LAB_LIGHTING_CONTROLLER_H_

