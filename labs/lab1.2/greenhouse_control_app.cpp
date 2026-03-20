#include "greenhouse_control_app.h"

#include <algorithm>
#include <array>

#include <QColor>
#include <QFont>
#include <QHBoxLayout>
#include <QMouseEvent>
#include <QStringList>

namespace {
QString DisplayName(const QString &name) {
  return name.trimmed().isEmpty() ? QString::fromUtf8("Безымянный росток")
                                  : name.trimmed();
}

QString PlantEmoji(int stage, int pose) {
  static const std::array<QStringList, 4> kPlantStates = {
      QStringList{QString::fromUtf8("🌱"), QString::fromUtf8("🌿"),
                  QString::fromUtf8("☘️")},
      QStringList{QString::fromUtf8("🪴"), QString::fromUtf8("🌵"),
                  QString::fromUtf8("🌾")},
      QStringList{QString::fromUtf8("🌳"), QString::fromUtf8("🌲"),
                  QString::fromUtf8("🌴")},
      QStringList{QString::fromUtf8("🌸"), QString::fromUtf8("🌺"),
                  QString::fromUtf8("🌼")}};
  return kPlantStates[stage][pose];
}

QColor AccentColor(const QString &biome) {
  if (biome == QString::fromUtf8("Марс")) {
    return QColor(QStringLiteral("#ff8a65"));
  }
  if (biome == QString::fromUtf8("Европа")) {
    return QColor(QStringLiteral("#6ec6ff"));
  }
  return QColor(QStringLiteral("#d1a3ff"));
}

QString BiomeDescription(const QString &biome) {
  if (biome == QString::fromUtf8("Марс")) {
    return QString::fromUtf8("сухой красный воздух");
  }
  if (biome == QString::fromUtf8("Европа")) {
    return QString::fromUtf8("ледяное сияние");
  }
  return QString::fromUtf8("туманную атмосферу");
}

QString MoodText(int light_level, int hydration, bool night_mode,
                 const QString &biome, int stage) {
  QStringList parts;
  if (light_level < 30) {
    parts << QString::fromUtf8("мало света");
  } else if (light_level > 80) {
    parts << QString::fromUtf8("слишком ярко");
  } else {
    parts << QString::fromUtf8("свет отличный");
  }

  if (hydration < 35) {
    parts << QString::fromUtf8("нужно полить");
  } else if (hydration > 80) {
    parts << QString::fromUtf8("почва очень влажная");
  } else {
    parts << QString::fromUtf8("влага в норме");
  }

  parts << QString::fromUtf8("ощущает ") + BiomeDescription(biome);

  if (night_mode) {
    parts << QString::fromUtf8("расслабляется в ночном режиме");
  }

  if (stage == 3) {
    parts << QString::fromUtf8("достигло звездного цветения");
  }

  return parts.join(QString::fromUtf8(", "));
}
}

GreenhouseControlApp::GreenhouseControlApp(QWidget *parent)
    : QWidget(parent),
      root_layout_(nullptr),
      control_panel_(nullptr),
      scene_panel_(nullptr),
      control_layout_(nullptr),
      scene_layout_(nullptr),
      title_label_(nullptr),
      name_label_(nullptr),
      name_edit_(nullptr),
      biome_label_(nullptr),
      biome_box_(nullptr),
      light_label_(nullptr),
      light_dial_(nullptr),
      night_mode_box_(nullptr),
      grow_button_(nullptr),
      plant_label_(nullptr),
      status_label_(nullptr),
      hint_label_(nullptr),
      plant_name_(QString::fromUtf8("Лира")),
      biome_(QString::fromUtf8("Марс")),
      growth_stage_(0),
      light_level_(52),
      hydration_(55),
      pose_index_(0),
      night_mode_(false) {
  buildInterface();
  connectSignals();
  updateInterface();
  setFocusPolicy(Qt::StrongFocus);
  setMinimumSize(860, 560);
  setWindowTitle(QString::fromUtf8("Лаба 2 — Космическая оранжерея"));
}

void GreenhouseControlApp::buildInterface() {
  root_layout_ = new QBoxLayout(QBoxLayout::LeftToRight, this);
  root_layout_->setContentsMargins(20, 20, 20, 20);
  root_layout_->setSpacing(18);

  control_panel_ = new QWidget(this);
  scene_panel_ = new QWidget(this);

  control_layout_ = new QVBoxLayout(control_panel_);
  control_layout_->setContentsMargins(18, 18, 18, 18);
  control_layout_->setSpacing(14);

  scene_layout_ = new QVBoxLayout(scene_panel_);
  scene_layout_->setContentsMargins(18, 18, 18, 18);
  scene_layout_->setSpacing(14);

  title_label_ = new QLabel(QString::fromUtf8("Космическая оранжерея"), this);
  title_label_->setAlignment(Qt::AlignCenter);

  name_label_ = new QLabel(QString::fromUtf8("Имя растения"), control_panel_);
  name_edit_ = new QLineEdit(control_panel_);
  name_edit_->setText(plant_name_);
  name_edit_->setPlaceholderText(QString::fromUtf8("Введите имя"));

  biome_label_ = new QLabel(QString::fromUtf8("Биом"), control_panel_);
  biome_box_ = new QComboBox(control_panel_);
  biome_box_->addItems({QString::fromUtf8("Марс"), QString::fromUtf8("Европа"),
                        QString::fromUtf8("Титан")});

  light_label_ = new QLabel(control_panel_);
  light_dial_ = new QDial(control_panel_);
  light_dial_->setRange(0, 100);
  light_dial_->setNotchesVisible(true);
  light_dial_->setValue(light_level_);

  night_mode_box_ = new QCheckBox(
      QString::fromUtf8("Мягкий ночной режим"), control_panel_);
  grow_button_ = new QPushButton(QString::fromUtf8("Вырастить"), control_panel_);

  plant_label_ = new QLabel(scene_panel_);
  plant_label_->setAlignment(Qt::AlignCenter);
  plant_label_->installEventFilter(this);

  status_label_ = new QLabel(scene_panel_);
  status_label_->setAlignment(Qt::AlignCenter);
  status_label_->setWordWrap(true);

  hint_label_ = new QLabel(
      QString::fromUtf8(
          "Кликните по растению, чтобы сменить позу. Пробел поливает, R "
          "сбрасывает оранжерею."),
      scene_panel_);
  hint_label_->setAlignment(Qt::AlignCenter);
  hint_label_->setWordWrap(true);

  control_layout_->addWidget(title_label_);
  control_layout_->addSpacing(8);
  control_layout_->addWidget(name_label_);
  control_layout_->addWidget(name_edit_);
  control_layout_->addWidget(biome_label_);
  control_layout_->addWidget(biome_box_);
  control_layout_->addWidget(light_label_);
  control_layout_->addWidget(light_dial_, 0, Qt::AlignHCenter);
  control_layout_->addWidget(night_mode_box_);
  control_layout_->addWidget(grow_button_);
  control_layout_->addStretch(1);

  scene_layout_->addWidget(plant_label_, 1);
  scene_layout_->addWidget(status_label_);
  scene_layout_->addWidget(hint_label_);

  root_layout_->addWidget(control_panel_, 1);
  root_layout_->addWidget(scene_panel_, 1);
}

void GreenhouseControlApp::connectSignals() {
  connect(name_edit_, &QLineEdit::textChanged, this,
          [this](const QString &text) {
            plant_name_ = text;
            updateInterface();
          });

  connect(biome_box_, &QComboBox::currentTextChanged, this,
          [this](const QString &text) {
            biome_ = text;
            hydration_ = std::clamp(hydration_ + 3, 0, 100);
            updateInterface();
          });

  connect(light_dial_, &QDial::valueChanged, this, [this](int value) {
    light_level_ = value;
    updateInterface();
  });

  connect(night_mode_box_, &QCheckBox::toggled, this, [this](bool checked) {
    night_mode_ = checked;
    hydration_ = std::clamp(hydration_ + (checked ? 4 : -4), 0, 100);
    updateInterface();
  });

  connect(grow_button_, &QPushButton::clicked, this,
          [this]() { growPlant(); });
}

bool GreenhouseControlApp::eventFilter(QObject *watched, QEvent *event) {
  if (watched == plant_label_ && event->type() == QEvent::MouseButtonPress) {
    pose_index_ = (pose_index_ + 1) % 3;
    hydration_ = std::clamp(hydration_ + 5, 0, 100);
    updateInterface();
    return true;
  }
  return QWidget::eventFilter(watched, event);
}

void GreenhouseControlApp::keyPressEvent(QKeyEvent *event) {
  if (event->key() == Qt::Key_Space) {
    waterPlant();
    event->accept();
    return;
  }
  if (event->key() == Qt::Key_R) {
    resetGreenhouse();
    event->accept();
    return;
  }
  if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
    growPlant();
    event->accept();
    return;
  }
  QWidget::keyPressEvent(event);
}

void GreenhouseControlApp::resizeEvent(QResizeEvent *event) {
  root_layout_->setDirection(width() < 760 ? QBoxLayout::TopToBottom
                                           : QBoxLayout::LeftToRight);

  const int title_size = std::clamp(width() / 26, 18, 32);
  const int plant_size = std::clamp(std::min(width(), height()) / 4, 56, 128);
  const int text_size = std::clamp(width() / 55, 10, 15);

  QFont title_font = title_label_->font();
  title_font.setPointSize(title_size);
  title_font.setBold(true);
  title_label_->setFont(title_font);

  QFont plant_font = plant_label_->font();
  plant_font.setPointSize(plant_size);
  plant_label_->setFont(plant_font);

  QFont text_font = status_label_->font();
  text_font.setPointSize(text_size);
  name_label_->setFont(text_font);
  biome_label_->setFont(text_font);
  light_label_->setFont(text_font);
  status_label_->setFont(text_font);
  hint_label_->setFont(text_font);

  plant_label_->setMinimumHeight(std::max(height() / 3, 180));
  QWidget::resizeEvent(event);
}

void GreenhouseControlApp::growPlant() {
  const bool good_light = light_level_ >= 35 && light_level_ <= 80;
  const bool enough_water = hydration_ >= 35;

  if (good_light && enough_water) {
    growth_stage_ = std::min(growth_stage_ + 1, 3);
    hydration_ = std::clamp(hydration_ - 12, 0, 100);
  } else {
    hydration_ = std::clamp(hydration_ - 4, 0, 100);
  }

  updateInterface();
}

void GreenhouseControlApp::waterPlant() {
  hydration_ = std::clamp(hydration_ + (night_mode_ ? 18 : 14), 0, 100);
  updateInterface();
}

void GreenhouseControlApp::resetGreenhouse() {
  plant_name_ = QString::fromUtf8("Лира");
  biome_ = QString::fromUtf8("Марс");
  growth_stage_ = 0;
  light_level_ = 52;
  hydration_ = 55;
  pose_index_ = 0;
  night_mode_ = false;

  name_edit_->setText(plant_name_);
  biome_box_->setCurrentText(biome_);
  light_dial_->setValue(light_level_);
  night_mode_box_->setChecked(night_mode_);
  updateInterface();
}

void GreenhouseControlApp::updateInterface() {
  const QColor accent = AccentColor(biome_);
  const QString display_name = DisplayName(plant_name_);
  const QString stage_text = [this]() {
    if (growth_stage_ == 0) {
      return QString::fromUtf8("росток");
    }
    if (growth_stage_ == 1) {
      return QString::fromUtf8("саженец");
    }
    if (growth_stage_ == 2) {
      return QString::fromUtf8("дерево");
    }
    return QString::fromUtf8("цветение");
  }();

  title_label_->setText(display_name + QString::fromUtf8(" в биоме ") + biome_);
  plant_label_->setText(PlantEmoji(growth_stage_, pose_index_));
  light_label_->setText(QString::fromUtf8("Освещение: %1%").arg(light_level_));

  status_label_->setText(
      QString::fromUtf8("Стадия: %1\nВлажность: %2%\nСостояние: %3")
          .arg(stage_text)
          .arg(hydration_)
          .arg(MoodText(light_level_, hydration_, night_mode_, biome_,
                        growth_stage_)));

  const QString button_text =
      growth_stage_ == 3 ? QString::fromUtf8("Поддержать цветение")
                         : QString::fromUtf8("Вырастить");
  grow_button_->setText(button_text);

  const QString common_style = QString::fromUtf8(
      "QWidget { background-color: #0c1624; color: #edf6ff; }"
      "QLineEdit, QComboBox { background-color: #132238; border: 1px solid %1; "
      "border-radius: 10px; padding: 8px; }"
      "QPushButton { background-color: %1; color: #091018; border: none; "
      "border-radius: 12px; padding: 10px 14px; font-weight: 600; }"
      "QPushButton:hover { background-color: #ffd8c5; }"
      "QCheckBox { spacing: 10px; }"
      "QDial { background-color: transparent; }")
                                  .arg(accent.name());

  setStyleSheet(common_style);

  control_panel_->setStyleSheet(QString::fromUtf8(
      "background-color: #101c2d; border: 2px solid %1; border-radius: 20px;")
                                    .arg(accent.name()));

  scene_panel_->setStyleSheet(QString::fromUtf8(
      "background-color: #132238; border: 2px solid %1; border-radius: 20px;")
                                  .arg(accent.name()));

  plant_label_->setStyleSheet(QString::fromUtf8(
      "background-color: #182b44; border: 2px dashed %1; border-radius: 22px;")
                                  .arg(accent.name()));

  status_label_->setStyleSheet(QString::fromUtf8(
      "background-color: #16273c; border-radius: 16px; padding: 12px;"));

  hint_label_->setStyleSheet(QString::fromUtf8("color: %1;").arg(accent.name()));
}
