#include <QApplication>
#include <QMainWindow>
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

  void CreateRoom(const QString& room_name, const LightState& initial_state) {
    rooms_[room_name] = initial_state;
  }

  bool HasRoom(const QString& room_name) const { return rooms_.contains(room_name); }

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
// Main Application Window
// ============================================================================

class LightingController : public QMainWindow {
  Q_OBJECT

 public:
  LightingController()
      : current_room_("Living Room"), light_manager_() {
    SetupUI();
    ConnectSignals();
    ApplyLightSettings();
  }

 private:
  void SetupUI() {
    auto* central_widget = new QWidget(this);
    auto* main_layout = new QHBoxLayout(central_widget);

    // Left panel
    auto* left_panel = CreateLeftPanel();
    main_layout->addWidget(left_panel, 1);

    // Middle panel
    auto* middle_panel = CreateMiddlePanel();
    main_layout->addWidget(middle_panel, 2);

    // Right panel
    auto* right_panel = CreateRightPanel();
    main_layout->addWidget(right_panel, 1);

    setCentralWidget(central_widget);
    setWindowTitle("Smart Lighting Control System");
    resize(1200, 500);
  }

  QGroupBox* CreateLeftPanel() {
    auto* group_box = new QGroupBox("Room Selection");
    auto* layout = new QVBoxLayout(group_box);

    layout->addWidget(new QLabel("Select Room:"));
    room_combo_ = new QComboBox();
    room_combo_->addItems(light_manager_.GetAllRoomNames());
    layout->addWidget(room_combo_);

    layout->addSpacing(15);
    layout->addWidget(new QLabel("Quick Presets:"));

    preset_warm_button_ = new QPushButton("Warm (3000K)");
    preset_neutral_button_ = new QPushButton("Neutral (5000K)");
    preset_cool_button_ = new QPushButton("Cool (6500K)");

    layout->addWidget(preset_warm_button_);
    layout->addWidget(preset_neutral_button_);
    layout->addWidget(preset_cool_button_);

    layout->addSpacing(15);
    save_preset_button_ = new QPushButton("💾 Save Settings");
    layout->addWidget(save_preset_button_);

    status_label_ = new QLabel("Status: Ready");
    status_label_->setStyleSheet("color: green; font-weight: bold;");
    layout->addSpacing(10);
    layout->addWidget(status_label_);

    layout->addStretch();
    return group_box;
  }

  QGroupBox* CreateMiddlePanel() {
    auto* group_box = new QGroupBox("Light Control");
    auto* layout = new QVBoxLayout(group_box);

    power_checkbox_ = new QCheckBox("Power");
    power_checkbox_->setChecked(true);
    power_checkbox_->setStyleSheet("QCheckBox { font-size: 14px; font-weight: bold; }");
    layout->addWidget(power_checkbox_);

    layout->addSpacing(10);

    // Brightness
    auto* brightness_group = new QGroupBox("Brightness");
    auto* brightness_layout = new QVBoxLayout(brightness_group);
    brightness_layout->setAlignment(Qt::AlignTop);

    auto* brightness_top = new QHBoxLayout();
    auto* min_label = new QLabel("Min");
    min_label->setAlignment(Qt::AlignLeft);
    brightness_top->addWidget(min_label);
    brightness_label_ = new QLabel("100%");
    brightness_label_->setStyleSheet("font-weight: bold; color: #ff9800;");
    brightness_label_->setAlignment(Qt::AlignCenter);
    brightness_top->addStretch();
    brightness_top->addWidget(brightness_label_);
    brightness_top->addStretch();
    auto* max_label = new QLabel("Max");
    max_label->setAlignment(Qt::AlignRight);
    brightness_top->addWidget(max_label);
    brightness_layout->addLayout(brightness_top);

    brightness_slider_ = new QSlider(Qt::Horizontal);
    brightness_slider_->setMinimum(0);
    brightness_slider_->setMaximum(100);
    brightness_slider_->setValue(100);
    brightness_slider_->setTickPosition(QSlider::TicksBelow);
    brightness_slider_->setTickInterval(10);
    brightness_layout->addWidget(brightness_slider_);

    brightness_progress_ = new QProgressBar();
    brightness_progress_->setMaximum(100);
    brightness_progress_->setValue(100);
    brightness_progress_->setStyleSheet(
        "QProgressBar { border: 1px solid #ddd; border-radius: 3px; height: 20px; }"
        "QProgressBar::chunk { background-color: #ff9800; }");
    brightness_layout->addWidget(brightness_progress_);

    layout->addWidget(brightness_group, 0, Qt::AlignTop);

    layout->addSpacing(10);

    // Color temperature
    auto* temp_group = new QGroupBox("Color Temperature (Kelvin)");
    auto* temp_layout = new QVBoxLayout(temp_group);

    auto* temp_dial_layout = new QHBoxLayout();
    temp_dial_layout->addWidget(new QLabel("Warm\n(3000K)"));

    color_dial_ = new QDial();
    color_dial_->setMinimum(3000);
    color_dial_->setMaximum(8000);
    color_dial_->setValue(5000);
    color_dial_->setNotchesVisible(true);
    color_dial_->setMaximumHeight(120);
    temp_dial_layout->addWidget(color_dial_);

    temp_dial_layout->addWidget(new QLabel("Cool\n(8000K)"));
    temp_layout->addLayout(temp_dial_layout);

    color_temp_label_ = new QLabel("5000 K");
    color_temp_label_->setAlignment(Qt::AlignCenter);
    color_temp_label_->setStyleSheet("font-weight: bold; font-size: 12px;");
    temp_layout->addWidget(color_temp_label_);

    layout->addWidget(temp_group, 0, Qt::AlignTop);
    layout->addStretch();
    return group_box;
  }

  QGroupBox* CreateRightPanel() {
    auto* group_box = new QGroupBox("Color Preview");
    auto* layout = new QVBoxLayout(group_box);
    layout->setContentsMargins(10, 10, 10, 10);
    
    color_preview_frame_ = new QFrame();
    color_preview_frame_->setStyleSheet("background-color: white; border: 2px solid #ddd; border-radius: 5px;");
    color_preview_frame_->setFrameShape(QFrame::StyledPanel);
    layout->addWidget(color_preview_frame_);

    return group_box;
  }

  void ConnectSignals() {
    connect(room_combo_, QOverload<const QString&>::of(&QComboBox::currentTextChanged), this,
            &LightingController::OnRoomChanged);
    connect(power_checkbox_, &QCheckBox::toggled, this,
            &LightingController::OnPowerToggled);
    connect(brightness_slider_, &QSlider::valueChanged, this,
            &LightingController::OnBrightnessChanged);
    connect(color_dial_, &QDial::valueChanged, this,
            &LightingController::OnColorTemperatureChanged);
    connect(preset_warm_button_, &QPushButton::clicked, this,
            &LightingController::OnWarmPresetClicked);
    connect(preset_neutral_button_, &QPushButton::clicked, this,
            &LightingController::OnNeutralPresetClicked);
    connect(preset_cool_button_, &QPushButton::clicked, this,
            &LightingController::OnCoolPresetClicked);
    connect(save_preset_button_, &QPushButton::clicked, this,
            &LightingController::OnSavePresetClicked);
  }

  void ApplyLightSettings() {
    LightState light_state = light_manager_.GetRoomState(current_room_);

    brightness_slider_->blockSignals(true);
    brightness_slider_->setValue(light_state.GetBrightness());
    brightness_slider_->blockSignals(false);

    color_dial_->blockSignals(true);
    color_dial_->setValue(light_state.GetColorTemperature());
    color_dial_->blockSignals(false);

    power_checkbox_->blockSignals(true);
    power_checkbox_->setChecked(light_state.IsEnabled());
    power_checkbox_->blockSignals(false);

    UpdateColorPreview();
  }

  void UpdateColorPreview() {
    LightState light_state = light_manager_.GetRoomState(current_room_);
    QColor color = ColorConverter::TemperatureToColor(light_state.GetColorTemperature());
    int brightness = light_state.IsEnabled() ? light_state.GetBrightness() : 30;
    color = ColorConverter::ApplyBrightness(color, brightness);

    QString style = QString("background-color: rgb(%1, %2, %3); border: 2px solid #ddd; border-radius: 5px;")
                        .arg(color.red())
                        .arg(color.green())
                        .arg(color.blue());
    color_preview_frame_->setStyleSheet(style);
  }

  void UpdateStatus(const QString& message) {
    status_label_->setText("Status: " + message);
  }

 private slots:
  void OnRoomChanged(const QString& room_name) {
    current_room_ = room_name;
    ApplyLightSettings();
    UpdateStatus(QString("Room: %1").arg(room_name));
  }

  void OnPowerToggled(bool is_checked) {
    LightState state = light_manager_.GetRoomState(current_room_);
    state.SetEnabled(is_checked);
    light_manager_.SetRoomState(current_room_, state);

    brightness_slider_->setEnabled(is_checked);
    color_dial_->setEnabled(is_checked);
    UpdateColorPreview();
    UpdateStatus(is_checked ? "Light ON" : "Light OFF");
  }

  void OnBrightnessChanged(int value) {
    LightState state = light_manager_.GetRoomState(current_room_);
    state.SetBrightness(value);
    light_manager_.SetRoomState(current_room_, state);

    brightness_label_->setText(QString::number(value) + "%");
    brightness_progress_->setValue(value);
    UpdateColorPreview();
  }

  void OnColorTemperatureChanged(int value) {
    LightState state = light_manager_.GetRoomState(current_room_);
    state.SetColorTemperature(value);
    light_manager_.SetRoomState(current_room_, state);

    color_temp_label_->setText(QString::number(value) + " K");
    UpdateColorPreview();
  }

  void OnWarmPresetClicked() {
    color_dial_->setValue(3000);
    UpdateStatus("Warm preset (3000K)");
  }

  void OnNeutralPresetClicked() {
    color_dial_->setValue(5000);
    UpdateStatus("Neutral preset (5000K)");
  }

  void OnCoolPresetClicked() {
    color_dial_->setValue(6500);
    UpdateStatus("Cool preset (6500K)");
  }

  void OnSavePresetClicked() {
    UpdateStatus(QString("✓ Settings saved for %1").arg(current_room_));
  }

 private:
  QString current_room_;
  LightManager light_manager_;

  // UI Widgets
  QComboBox* room_combo_;
  QPushButton* preset_warm_button_;
  QPushButton* preset_neutral_button_;
  QPushButton* preset_cool_button_;
  QPushButton* save_preset_button_;
  QLabel* status_label_;

  QCheckBox* power_checkbox_;
  QSlider* brightness_slider_;
  QLabel* brightness_label_;
  QProgressBar* brightness_progress_;
  QDial* color_dial_;
  QLabel* color_temp_label_;

  QFrame* color_preview_frame_;
};

int main(int argc, char* argv[]) {
  QApplication app(argc, argv);
  LightingController window;
  window.show();
  return app.exec();
}
