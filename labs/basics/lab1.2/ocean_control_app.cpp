#include "ocean_control_app.h"

#include <algorithm>

#include <QCheckBox>
#include <QColor>
#include <QFont>
#include <QGridLayout>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QResizeEvent>
#include <QShortcut>
#include <QSlider>
#include <QTimer>
#include <QVBoxLayout>

namespace {

constexpr int kInitialDepth = 120;
constexpr int kMaximumDepth = 1200;
constexpr int kInitialOxygen = 100;
constexpr int kEmergencyReserve = 15;
constexpr int kMissionIntervalMs = 650;
constexpr int kKeyboardDiveStep = 60;

}

OceanControlApp::OceanControlApp(QWidget *parent)
    : QWidget(parent),
      status_label_(nullptr),
      vessel_name_edit_(nullptr),
      mission_button_(nullptr),
      stealth_checkbox_(nullptr),
      depth_slider_(nullptr),
      oxygen_bar_(nullptr),
      mission_timer_(nullptr),
      oxygen_level_(kInitialOxygen),
      mission_active_(false) {
  setMinimumSize(680, 440);
  resize(920, 600);
  setFocusPolicy(Qt::StrongFocus);
  buildUi();
  connectUi();
  applyResponsiveMetrics();
  applyTheme();
  setOxygenLevel(oxygen_level_);
  setStatusText(tr("Батискаф «%1» готов к запуску.").arg(vesselName()));
  setWindowTitle(tr("Пульт батискафа — %1").arg(vesselName()));
}

void OceanControlApp::resizeEvent(QResizeEvent *event) {
  QWidget::resizeEvent(event);
  applyResponsiveMetrics();
  setWindowTitle(tr("Пульт батискафа — %1 · %2×%3")
                     .arg(vesselName())
                     .arg(event->size().width())
                     .arg(event->size().height()));
}

void OceanControlApp::accelerateDive() {
  depth_slider_->setValue(std::min(depth_slider_->value() + kKeyboardDiveStep,
                                   depth_slider_->maximum()));

  if (!mission_active_) {
    return;
  }

  setOxygenLevel(std::max(oxygen_level_ - 3, 0));

  if (oxygen_level_ == 0) {
    stopMission(tr("Ускорение сорвало миссию: кислород закончился."));
    return;
  }

  setStatusText(tr("%1 ускорил погружение до %2 м.")
                    .arg(vesselName())
                    .arg(depth_slider_->value()));
}

void OceanControlApp::restoreOxygen() {
  setOxygenLevel(kInitialOxygen);

  if (mission_active_) {
    setStatusText(tr("Аварийный баллон подключен. Кислород восстановлен."));
    return;
  }

  setStatusText(tr("Резервный запас кислорода полностью восстановлен."));
}

void OceanControlApp::tickMission() {
  int oxygen_loss = 2 + depth_slider_->value() / 320;

  if (stealth_checkbox_->isChecked()) {
    oxygen_loss += 1;
  }

  setOxygenLevel(std::max(oxygen_level_ - oxygen_loss, 0));

  if (oxygen_level_ == 0) {
    stopMission(tr("Кислород закончился. Миссия аварийно завершена."));
    return;
  }

  if (oxygen_level_ <= kEmergencyReserve) {
    setStatusText(tr("Тревога: осталось %1%% кислорода.").arg(oxygen_level_));
    return;
  }

  if (depth_slider_->value() >= 900) {
    setStatusText(tr("%1 ушёл в сумеречную зону на глубине %2 м.")
                      .arg(vesselName())
                      .arg(depth_slider_->value()));
    return;
  }

  if (depth_slider_->value() >= 500) {
    setStatusText(tr("%1 стабильно держит курс на %2 м.")
                      .arg(vesselName())
                      .arg(depth_slider_->value()));
    return;
  }

  setStatusText(tr("%1 скользит у поверхности на глубине %2 м.")
                    .arg(vesselName())
                    .arg(depth_slider_->value()));
}

void OceanControlApp::toggleMission() {
  if (mission_active_) {
    stopMission(tr("Миссия поставлена на паузу."));
    return;
  }

  if (oxygen_level_ == 0) {
    setStatusText(tr("Сначала восстановите кислород клавишей R."));
    return;
  }

  mission_active_ = true;
  mission_timer_->start();
  mission_button_->setText(tr("Поставить на паузу"));
  setStatusText(tr("%1 начал погружение. Целевая глубина: %2 м.")
                    .arg(vesselName())
                    .arg(depth_slider_->value()));
}

void OceanControlApp::updateDepth(int depth) {
  depth_slider_->setToolTip(tr("Глубина: %1 м").arg(depth));
  applyTheme();

  if (!mission_active_) {
    setStatusText(tr("%1 готовится к маршруту на глубине %2 м.")
                      .arg(vesselName())
                      .arg(depth));
    return;
  }

  setStatusText(tr("%1 меняет курс. Новая глубина: %2 м.")
                    .arg(vesselName())
                    .arg(depth));
}

void OceanControlApp::updateStealthMode(bool enabled) {
  applyTheme();

  if (enabled) {
    setStatusText(tr("Тихий режим включён: батискаф стал менее заметным."));
    return;
  }

  setStatusText(tr("Тихий режим выключен: можно идти быстрее и ярче."));
}

void OceanControlApp::updateVesselName(const QString &name) {
  const QString trimmed_name = name.trimmed();

  setWindowTitle(tr("Пульт батискафа — %1 · %2×%3")
                     .arg(vesselName())
                     .arg(width())
                     .arg(height()));

  if (trimmed_name.isEmpty()) {
    if (!mission_active_) {
      setStatusText(tr("Безымянный батискаф ждёт команды к старту."));
    }
    return;
  }

  if (mission_active_) {
    setStatusText(tr("%1 уже в миссии и принимает новые команды.").arg(trimmed_name));
    return;
  }

  setStatusText(tr("Батискаф «%1» готов к запуску.").arg(trimmed_name));
}

void OceanControlApp::applyResponsiveMetrics() {
  const int base_size = std::clamp(std::min(width(), height()) / 30, 11, 24);

  QFont status_font = status_label_->font();
  status_font.setPointSize(base_size + 5);
  status_font.setBold(true);
  status_label_->setFont(status_font);

  QFont controls_font = vessel_name_edit_->font();
  controls_font.setPointSize(base_size);
  vessel_name_edit_->setFont(controls_font);
  mission_button_->setFont(controls_font);
  stealth_checkbox_->setFont(controls_font);
  oxygen_bar_->setFont(controls_font);

  const int control_height = std::clamp(height() / 10, 44, 80);
  vessel_name_edit_->setMinimumHeight(control_height);
  mission_button_->setMinimumHeight(control_height);
  oxygen_bar_->setMinimumHeight(std::max(control_height - 6, 36));
  depth_slider_->setMinimumHeight(std::max(control_height - 2, 40));
}

void OceanControlApp::applyTheme() {
  const int depth = depth_slider_->value();
  const bool stealth_enabled = stealth_checkbox_->isChecked();

  const QColor accent = stealth_enabled ? QColor::fromHsv(168, 170, 210)
                                        : QColor::fromHsv(204, 170, 230);
  const QColor border = oxygen_level_ <= kEmergencyReserve
                            ? QColor(QStringLiteral("#ff7a7a"))
                            : accent;
  const QColor top = stealth_enabled ? QColor::fromHsv(175, 95, 58 - depth / 55)
                                     : QColor::fromHsv(206, 110, 72 - depth / 55);
  const QColor bottom = stealth_enabled ? QColor::fromHsv(180, 130, 34 - depth / 65)
                                        : QColor::fromHsv(212, 150, 38 - depth / 65);

  setStyleSheet(QStringLiteral(
                    "QWidget {"
                    "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, stop:0 %1, stop:1 %2);"
                    "color: #f4fbff;"
                    "font-family: 'Inter', 'Segoe UI', sans-serif;"
                    "}"
                    "QLabel {"
                    "padding: 18px;"
                    "border: 2px solid %3;"
                    "border-radius: 18px;"
                    "background: rgba(255, 255, 255, 0.08);"
                    "}"
                    "QLineEdit, QPushButton, QCheckBox, QSlider, QProgressBar {"
                    "border-radius: 16px;"
                    "}"
                    "QLineEdit, QPushButton, QProgressBar {"
                    "border: 2px solid %3;"
                    "background: rgba(255, 255, 255, 0.10);"
                    "padding: 10px 14px;"
                    "}"
                    "QLineEdit:focus, QPushButton:focus {"
                    "border: 2px solid #ffffff;"
                    "}"
                    "QPushButton:hover {"
                    "background: rgba(255, 255, 255, 0.18);"
                    "}"
                    "QPushButton:pressed {"
                    "background: rgba(255, 255, 255, 0.26);"
                    "}"
                    "QCheckBox::indicator {"
                    "width: 24px;"
                    "height: 24px;"
                    "border-radius: 8px;"
                    "border: 2px solid %3;"
                    "background: rgba(0, 0, 0, 0.18);"
                    "}"
                    "QCheckBox::indicator:checked {"
                    "background: %3;"
                    "}"
                    "QSlider::groove:horizontal {"
                    "height: 12px;"
                    "border-radius: 6px;"
                    "background: rgba(255, 255, 255, 0.16);"
                    "}"
                    "QSlider::handle:horizontal {"
                    "width: 28px;"
                    "margin: -10px 0;"
                    "border-radius: 14px;"
                    "background: %3;"
                    "}"
                    "QProgressBar {"
                    "text-align: center;"
                    "}"
                    "QProgressBar::chunk {"
                    "border-radius: 12px;"
                    "background: %4;"
                    "}")
                    .arg(top.name())
                    .arg(bottom.name())
                    .arg(border.name())
                    .arg(accent.lighter(115).name()));
}

void OceanControlApp::buildUi() {
  status_label_ = new QLabel(this);
  status_label_->setAlignment(Qt::AlignCenter);
  status_label_->setWordWrap(true);
  status_label_->setTextInteractionFlags(Qt::NoTextInteraction);

  vessel_name_edit_ = new QLineEdit(this);
  vessel_name_edit_->setPlaceholderText(tr("Название батискафа"));
  vessel_name_edit_->setText(tr("Нерпа"));
  vessel_name_edit_->setClearButtonEnabled(true);

  mission_button_ = new QPushButton(tr("Запустить погружение"), this);
  mission_button_->setCursor(Qt::PointingHandCursor);

  stealth_checkbox_ = new QCheckBox(tr("Тихий режим"), this);
  stealth_checkbox_->setCursor(Qt::PointingHandCursor);

  depth_slider_ = new QSlider(Qt::Horizontal, this);
  depth_slider_->setRange(0, kMaximumDepth);
  depth_slider_->setValue(kInitialDepth);
  depth_slider_->setTickPosition(QSlider::TicksBelow);
  depth_slider_->setTickInterval(100);

  oxygen_bar_ = new QProgressBar(this);
  oxygen_bar_->setRange(0, 100);
  oxygen_bar_->setFormat(tr("Кислород: %p%"));
  oxygen_bar_->setAlignment(Qt::AlignCenter);

  mission_timer_ = new QTimer(this);
  mission_timer_->setInterval(kMissionIntervalMs);

  auto *start_shortcut = new QShortcut(QKeySequence(Qt::Key_S), this);
  auto *refill_shortcut = new QShortcut(QKeySequence(Qt::Key_R), this);
  auto *boost_shortcut = new QShortcut(QKeySequence(Qt::Key_Space), this);

  connect(start_shortcut, &QShortcut::activated, this, &OceanControlApp::toggleMission);
  connect(refill_shortcut, &QShortcut::activated, this, &OceanControlApp::restoreOxygen);
  connect(boost_shortcut, &QShortcut::activated, this, &OceanControlApp::accelerateDive);

  auto *header_layout = new QGridLayout();
  header_layout->setHorizontalSpacing(14);
  header_layout->addWidget(vessel_name_edit_, 0, 0);
  header_layout->addWidget(stealth_checkbox_, 0, 1);
  header_layout->setColumnStretch(0, 3);
  header_layout->setColumnStretch(1, 2);

  auto *root_layout = new QVBoxLayout(this);
  root_layout->setContentsMargins(26, 26, 26, 26);
  root_layout->setSpacing(18);
  root_layout->addWidget(status_label_);
  root_layout->addLayout(header_layout);
  root_layout->addWidget(depth_slider_);
  root_layout->addWidget(oxygen_bar_);
  root_layout->addStretch(1);
  root_layout->addWidget(mission_button_);
}

void OceanControlApp::connectUi() {
  connect(vessel_name_edit_, &QLineEdit::textChanged, this, &OceanControlApp::updateVesselName);
  connect(mission_button_, &QPushButton::clicked, this, &OceanControlApp::toggleMission);
  connect(stealth_checkbox_, &QCheckBox::toggled, this, &OceanControlApp::updateStealthMode);
  connect(depth_slider_, &QSlider::valueChanged, this, &OceanControlApp::updateDepth);
  connect(mission_timer_, &QTimer::timeout, this, &OceanControlApp::tickMission);
  connect(this, &OceanControlApp::oxygenLevelChanged, oxygen_bar_, &QProgressBar::setValue);
  connect(this, &OceanControlApp::statusTextChanged, status_label_, &QLabel::setText);
}

void OceanControlApp::setOxygenLevel(int value) {
  oxygen_level_ = std::clamp(value, oxygen_bar_->minimum(), oxygen_bar_->maximum());
  emit oxygenLevelChanged(oxygen_level_);
  applyTheme();
}

void OceanControlApp::setStatusText(const QString &text) {
  emit statusTextChanged(text);
}

void OceanControlApp::stopMission(const QString &message) {
  mission_active_ = false;
  mission_timer_->stop();
  mission_button_->setText(tr("Запустить погружение"));
  setStatusText(message);
}

auto OceanControlApp::vesselName() const -> QString {
  const QString trimmed_name = vessel_name_edit_->text().trimmed();

  if (trimmed_name.isEmpty()) {
    return tr("Безымянный батискаф");
  }

  return trimmed_name;
}
