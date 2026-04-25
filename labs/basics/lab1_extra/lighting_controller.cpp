#include "lighting_controller.h"
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QSlider>
#include <QDial>
#include <QCheckBox>
#include <QComboBox>
#include <QLCDNumber>
#include <QPushButton>
#include <QLabel>
#include <QProgressBar>
#include <QGroupBox>
#include <QColor>
#include <QFrame>
#include <QMap>
#include <algorithm>

// ============================================================================
// Light State Model
// ============================================================================

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

// ============================================================================
// Color Converter
// ============================================================================

class ColorConverter {
 public:
  static QColor TemperatureToColor(int kelvin) {
    kelvin = std::max(1000, std::min(10000, kelvin));

    int red, green, blue;

    if (kelvin < 4000) {
      red = 255;
      green = static_cast<int>(200 - (4000 - kelvin) * 0.02);
      blue = static_cast<int>(150 - (4000 - kelvin) * 0.03);
    } else if (kelvin < 6500) {
      red = 255;
      green = static_cast<int>(220 + (kelvin - 4000) * 0.015);
      blue = static_cast<int>(180 + (kelvin - 4000) * 0.01);
    } else {
      red = static_cast<int>(255 - (kelvin - 6500) * 0.01);
      green = static_cast<int>(235 - (kelvin - 6500) * 0.005);
      blue = 255;
    }

    red = std::max(0, std::min(255, red));
    green = std::max(0, std::min(255, green));
    blue = std::max(0, std::min(255, blue));

    return QColor(red, green, blue);
  }

  static QColor ApplyBrightness(const QColor& color, int brightness_percent) {
    brightness_percent = std::max(0, std::min(100, brightness_percent));
    QColor result = color;
    result.setHsv(result.hue(), result.saturation(),
                  (result.value() * brightness_percent) / 100);
    return result;
  }
};

// ============================================================================
// Light Manager
// ============================================================================

class LightManager {
 public:
  LightManager() { InitializeDefaultRooms(); }

  LightState GetRoomState(const QString& room_name) const {
    return rooms_.value(room_name, LightState());
  }

  void SetRoomState(const QString& room_name, const LightState& state) {
    if (rooms_.contains(room_name)) {
      rooms_[room_name] = state;
    }
  }

  QStringList GetAllRoomNames() const { return rooms_.keys(); }

 private:
  void InitializeDefaultRooms() {
    rooms_["Living Room"] = LightState(100, 5000, true);
    rooms_["Bedroom"] = LightState(50, 3000, true);
    rooms_["Kitchen"] = LightState(100, 6500, true);
    rooms_["Bathroom"] = LightState(80, 5500, true);
    rooms_["Office"] = LightState(90, 6000, true);
  }

  QMap<QString, LightState> rooms_;
};

// ============================================================================
// Implementation
// ============================================================================

class LightingController::Impl {
 public:
  QString current_room;
  LightManager light_manager;

  QComboBox* room_combo;
  QPushButton* preset_warm_button;
  QPushButton* preset_neutral_button;
  QPushButton* preset_cool_button;
  QPushButton* save_preset_button;
  QLabel* status_label;

  QCheckBox* power_checkbox;
  QSlider* brightness_slider;
  QLabel* brightness_label;
  QProgressBar* brightness_progress;
  QDial* color_dial;
  QLabel* color_temp_label;

  QFrame* color_preview_frame;
  QLCDNumber* brightness_lcd;
  QLabel* info_label;
};

// ============================================================================
// LightingController Implementation
// ============================================================================

LightingController::LightingController()
    : impl_(new Impl()) {
  impl_->current_room = "Living Room";
  SetupUI();
  ConnectSignals();
  ApplyLightSettings();
}

void LightingController::SetupUI() {
  auto* central_widget = new QWidget(this);
  auto* main_layout = new QHBoxLayout(central_widget);

  // Left panel
  auto* left_panel = new QGroupBox("Room Selection");
  auto* left_layout = new QVBoxLayout(left_panel);

  left_layout->addWidget(new QLabel("Select Room:"));
  impl_->room_combo = new QComboBox();
  impl_->room_combo->addItems(impl_->light_manager.GetAllRoomNames());
  left_layout->addWidget(impl_->room_combo);

  left_layout->addSpacing(15);
  left_layout->addWidget(new QLabel("Quick Presets:"));

  impl_->preset_warm_button = new QPushButton("Warm (3000K)");
  impl_->preset_neutral_button = new QPushButton("Neutral (5000K)");
  impl_->preset_cool_button = new QPushButton("Cool (6500K)");

  left_layout->addWidget(impl_->preset_warm_button);
  left_layout->addWidget(impl_->preset_neutral_button);
  left_layout->addWidget(impl_->preset_cool_button);

  left_layout->addSpacing(15);
  impl_->save_preset_button = new QPushButton("💾 Save Settings");
  left_layout->addWidget(impl_->save_preset_button);

  impl_->status_label = new QLabel("Status: Ready");
  impl_->status_label->setStyleSheet("color: green; font-weight: bold;");
  left_layout->addSpacing(10);
  left_layout->addWidget(impl_->status_label);
  left_layout->addStretch();

  main_layout->addWidget(left_panel, 1);

  // Middle panel - Light Control
  auto* middle_panel = new QGroupBox("Light Control");
  auto* middle_layout = new QVBoxLayout(middle_panel);

  impl_->power_checkbox = new QCheckBox("Power");
  impl_->power_checkbox->setChecked(true);
  impl_->power_checkbox->setStyleSheet("QCheckBox { font-size: 14px; font-weight: bold; }");
  middle_layout->addWidget(impl_->power_checkbox);
  middle_layout->addSpacing(10);

  // Brightness
  auto* brightness_group = new QGroupBox("Brightness");
  auto* brightness_layout = new QVBoxLayout(brightness_group);

  auto* brightness_top = new QHBoxLayout();
  brightness_top->addWidget(new QLabel("0%"));
  impl_->brightness_label = new QLabel("100%");
  impl_->brightness_label->setStyleSheet("font-weight: bold; color: #ff9800;");
  brightness_top->addStretch();
  brightness_top->addWidget(impl_->brightness_label);
  brightness_top->addWidget(new QLabel("100%"));
  brightness_layout->addLayout(brightness_top);

  impl_->brightness_slider = new QSlider(Qt::Horizontal);
  impl_->brightness_slider->setMinimum(0);
  impl_->brightness_slider->setMaximum(100);
  impl_->brightness_slider->setValue(100);
  impl_->brightness_slider->setTickPosition(QSlider::TicksBelow);
  impl_->brightness_slider->setTickInterval(10);
  brightness_layout->addWidget(impl_->brightness_slider);

  impl_->brightness_progress = new QProgressBar();
  impl_->brightness_progress->setMaximum(100);
  impl_->brightness_progress->setValue(100);
  impl_->brightness_progress->setStyleSheet(
      "QProgressBar { border: 1px solid #ddd; border-radius: 3px; }"
      "QProgressBar::chunk { background-color: #ff9800; }");
  brightness_layout->addWidget(impl_->brightness_progress);

  middle_layout->addWidget(brightness_group);
  middle_layout->addSpacing(10);

  // Color temperature
  auto* temp_group = new QGroupBox("Color Temperature (Kelvin)");
  auto* temp_layout = new QVBoxLayout(temp_group);

  auto* temp_dial_layout = new QHBoxLayout();
  temp_dial_layout->addWidget(new QLabel("Warm\n(3000K)"));

  impl_->color_dial = new QDial();
  impl_->color_dial->setMinimum(3000);
  impl_->color_dial->setMaximum(8000);
  impl_->color_dial->setValue(5000);
  impl_->color_dial->setNotchesVisible(true);
  temp_dial_layout->addWidget(impl_->color_dial);

  temp_dial_layout->addWidget(new QLabel("Cool\n(8000K)"));
  temp_layout->addLayout(temp_dial_layout);

  impl_->color_temp_label = new QLabel("5000 K");
  impl_->color_temp_label->setAlignment(Qt::AlignCenter);
  impl_->color_temp_label->setStyleSheet("font-weight: bold; font-size: 12px;");
  temp_layout->addWidget(impl_->color_temp_label);

  middle_layout->addWidget(temp_group);

  main_layout->addWidget(middle_panel, 2);

  // Right panel - Status & Preview
  auto* right_panel = new QGroupBox("Status & Preview");
  auto* right_layout = new QVBoxLayout(right_panel);

  right_layout->addWidget(new QLabel("Color Preview:"));
  impl_->color_preview_frame = new QFrame();
  impl_->color_preview_frame->setMinimumHeight(100);
  impl_->color_preview_frame->setStyleSheet("background-color: white; border: 2px solid #ddd;");
  impl_->color_preview_frame->setFrameShape(QFrame::StyledPanel);
  right_layout->addWidget(impl_->color_preview_frame);

  right_layout->addSpacing(10);

  right_layout->addWidget(new QLabel("Brightness Display:"));
  impl_->brightness_lcd = new QLCDNumber();
  impl_->brightness_lcd->setDigitCount(3);
  impl_->brightness_lcd->setSegmentStyle(QLCDNumber::Filled);
  impl_->brightness_lcd->display("100");
  right_layout->addWidget(impl_->brightness_lcd);

  right_layout->addSpacing(10);

  right_layout->addWidget(new QLabel("Information:"));
  impl_->info_label = new QLabel();
  impl_->info_label->setWordWrap(true);
  impl_->info_label->setStyleSheet(
      "background-color: #f5f5f5; padding: 8px; border-radius: 3px;");
  right_layout->addWidget(impl_->info_label);

  right_layout->addStretch();

  main_layout->addWidget(right_panel, 1);

  setCentralWidget(central_widget);
  setWindowTitle("Smart Lighting Control System");
  resize(1200, 500);
}

void LightingController::ConnectSignals() {
  connect(impl_->room_combo, QOverload<const QString&>::of(&QComboBox::currentTextChanged),
          this, &LightingController::OnRoomChanged);
  connect(impl_->power_checkbox, &QCheckBox::toggled, this,
          &LightingController::OnPowerToggled);
  connect(impl_->brightness_slider, &QSlider::valueChanged, this,
          &LightingController::OnBrightnessChanged);
  connect(impl_->color_dial, &QDial::valueChanged, this,
          &LightingController::OnColorTemperatureChanged);
  connect(impl_->preset_warm_button, &QPushButton::clicked, this,
          &LightingController::OnWarmPresetClicked);
  connect(impl_->preset_neutral_button, &QPushButton::clicked, this,
          &LightingController::OnNeutralPresetClicked);
  connect(impl_->preset_cool_button, &QPushButton::clicked, this,
          &LightingController::OnCoolPresetClicked);
  connect(impl_->save_preset_button, &QPushButton::clicked, this,
          &LightingController::OnSavePresetClicked);
}

void LightingController::ApplyLightSettings() {
  LightState light_state = impl_->light_manager.GetRoomState(impl_->current_room);

  impl_->brightness_slider->blockSignals(true);
  impl_->brightness_slider->setValue(light_state.GetBrightness());
  impl_->brightness_slider->blockSignals(false);

  impl_->color_dial->blockSignals(true);
  impl_->color_dial->setValue(light_state.GetColorTemperature());
  impl_->color_dial->blockSignals(false);

  impl_->power_checkbox->blockSignals(true);
  impl_->power_checkbox->setChecked(light_state.IsEnabled());
  impl_->power_checkbox->blockSignals(false);

  UpdateColorPreview();
  UpdateInfoDisplay();
}

void LightingController::UpdateColorPreview() {
  LightState light_state = impl_->light_manager.GetRoomState(impl_->current_room);
  QColor color = ColorConverter::TemperatureToColor(light_state.GetColorTemperature());
  int brightness = light_state.IsEnabled() ? light_state.GetBrightness() : 30;
  color = ColorConverter::ApplyBrightness(color, brightness);

  QString style = QString("background-color: rgb(%1, %2, %3); border: 2px solid #ddd;")
                      .arg(color.red())
                      .arg(color.green())
                      .arg(color.blue());
  impl_->color_preview_frame->setStyleSheet(style);
}

void LightingController::UpdateInfoDisplay() {
  LightState light_state = impl_->light_manager.GetRoomState(impl_->current_room);
  QString info = QString("Room: %1\nBrightness: %2%\nColor Temp: %3K\nStatus: %4")
                     .arg(impl_->current_room)
                     .arg(light_state.GetBrightness())
                     .arg(light_state.GetColorTemperature())
                     .arg(light_state.IsEnabled() ? "ON" : "OFF");
  impl_->info_label->setText(info);
}

void LightingController::UpdateStatus(const QString& message) {
  impl_->status_label->setText("Status: " + message);
}

void LightingController::OnRoomChanged(const QString& room_name) {
  impl_->current_room = room_name;
  ApplyLightSettings();
  UpdateStatus(QString("Room: %1").arg(room_name));
}

void LightingController::OnPowerToggled(bool is_checked) {
  LightState state = impl_->light_manager.GetRoomState(impl_->current_room);
  state.SetEnabled(is_checked);
  impl_->light_manager.SetRoomState(impl_->current_room, state);

  impl_->brightness_slider->setEnabled(is_checked);
  impl_->color_dial->setEnabled(is_checked);
  UpdateColorPreview();
  UpdateInfoDisplay();
  UpdateStatus(is_checked ? "Light ON" : "Light OFF");
}

void LightingController::OnBrightnessChanged(int value) {
  LightState state = impl_->light_manager.GetRoomState(impl_->current_room);
  state.SetBrightness(value);
  impl_->light_manager.SetRoomState(impl_->current_room, state);

  impl_->brightness_label->setText(QString::number(value) + "%");
  impl_->brightness_progress->setValue(value);
  impl_->brightness_lcd->display(value);
  UpdateColorPreview();
  UpdateInfoDisplay();
}

void LightingController::OnColorTemperatureChanged(int value) {
  LightState state = impl_->light_manager.GetRoomState(impl_->current_room);
  state.SetColorTemperature(value);
  impl_->light_manager.SetRoomState(impl_->current_room, state);

  impl_->color_temp_label->setText(QString::number(value) + " K");
  UpdateColorPreview();
  UpdateInfoDisplay();
}

void LightingController::OnWarmPresetClicked() {
  impl_->color_dial->setValue(3000);
  UpdateStatus("Warm preset (3000K)");
}

void LightingController::OnNeutralPresetClicked() {
  impl_->color_dial->setValue(5000);
  UpdateStatus("Neutral preset (5000K)");
}

void LightingController::OnCoolPresetClicked() {
  impl_->color_dial->setValue(6500);
  UpdateStatus("Cool preset (6500K)");
}

void LightingController::OnSavePresetClicked() {
  UpdateStatus(QString("✓ Settings saved for %1").arg(impl_->current_room));
}

