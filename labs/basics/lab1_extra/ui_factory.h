#ifndef LAB_UI_FACTORY_H_
#define LAB_UI_FACTORY_H_

#include <QGroupBox>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QSlider>
#include <QDial>
#include <QCheckBox>
#include <QComboBox>
#include <QLCDNumber>
#include <QProgressBar>
#include <QFrame>

/**
 * Factory for creating UI components and layouts.
 * Centralizes UI element creation to maintain consistency and simplify changes.
 */
class UIFactory {
 public:
  struct LeftPanelWidgets {
    QComboBox* room_combo;
    QPushButton* preset_warm_button;
    QPushButton* preset_neutral_button;
    QPushButton* preset_cool_button;
    QPushButton* save_preset_button;
    QLabel* status_label;
  };

  struct MiddlePanelWidgets {
    QCheckBox* power_checkbox;
    QSlider* brightness_slider;
    QLabel* brightness_label;
    QProgressBar* brightness_progress;
    QDial* color_dial;
    QLabel* color_temp_label;
  };

  struct RightPanelWidgets {
    QFrame* color_preview_frame;
    QLCDNumber* brightness_lcd;
    QLabel* info_label;
  };

  static QGroupBox* CreateLeftPanel(LeftPanelWidgets& out_widgets) {
    auto* group_box = new QGroupBox("Room Selection");
    auto* layout = new QVBoxLayout(group_box);

    layout->addWidget(new QLabel("Select Room:"));
    out_widgets.room_combo = new QComboBox();
    out_widgets.room_combo->addItems(
        {"Living Room", "Bedroom", "Kitchen", "Bathroom", "Office"});
    layout->addWidget(out_widgets.room_combo);

    layout->addSpacing(15);
    layout->addWidget(new QLabel("Quick Presets:"));

    out_widgets.preset_warm_button = new QPushButton("Warm (3000K)");
    out_widgets.preset_neutral_button = new QPushButton("Neutral (5000K)");
    out_widgets.preset_cool_button = new QPushButton("Cool (6500K)");

    layout->addWidget(out_widgets.preset_warm_button);
    layout->addWidget(out_widgets.preset_neutral_button);
    layout->addWidget(out_widgets.preset_cool_button);

    layout->addSpacing(15);
    out_widgets.save_preset_button = new QPushButton("💾 Save Current Settings");
    layout->addWidget(out_widgets.save_preset_button);

    out_widgets.status_label = new QLabel("Status: Ready");
    out_widgets.status_label->setStyleSheet("color: green; font-weight: bold;");
    layout->addSpacing(10);
    layout->addWidget(out_widgets.status_label);

    layout->addStretch();

    return group_box;
  }

  static QGroupBox* CreateMiddlePanel(MiddlePanelWidgets& out_widgets) {
    auto* group_box = new QGroupBox("Light Control");
    auto* layout = new QVBoxLayout(group_box);

    // Power toggle
    out_widgets.power_checkbox = new QCheckBox("Power");
    out_widgets.power_checkbox->setChecked(true);
    out_widgets.power_checkbox->setStyleSheet(
        "QCheckBox { font-size: 14px; font-weight: bold; }");
    layout->addWidget(out_widgets.power_checkbox);

    layout->addSpacing(10);

    // Brightness control
    auto* brightness_group = new QGroupBox("Brightness");
    auto* brightness_layout = new QVBoxLayout(brightness_group);

    auto* brightness_top = new QHBoxLayout();
    brightness_top->addWidget(new QLabel("0%"));
    out_widgets.brightness_label = new QLabel("100%");
    out_widgets.brightness_label->setStyleSheet("font-weight: bold; color: #ff9800;");
    brightness_top->addStretch();
    brightness_top->addWidget(out_widgets.brightness_label);
    brightness_top->addWidget(new QLabel("100%"));
    brightness_layout->addLayout(brightness_top);

    out_widgets.brightness_slider = new QSlider(Qt::Horizontal);
    out_widgets.brightness_slider->setMinimum(0);
    out_widgets.brightness_slider->setMaximum(100);
    out_widgets.brightness_slider->setValue(100);
    out_widgets.brightness_slider->setTickPosition(QSlider::TicksBelow);
    out_widgets.brightness_slider->setTickInterval(10);
    brightness_layout->addWidget(out_widgets.brightness_slider);

    out_widgets.brightness_progress = new QProgressBar();
    out_widgets.brightness_progress->setMaximum(100);
    out_widgets.brightness_progress->setValue(100);
    out_widgets.brightness_progress->setStyleSheet(
        "QProgressBar { border: 1px solid #ddd; border-radius: 3px; }"
        "QProgressBar::chunk { background-color: #ff9800; }");
    brightness_layout->addWidget(out_widgets.brightness_progress);

    layout->addWidget(brightness_group);

    layout->addSpacing(10);

    // Color temperature control
    auto* temp_group = new QGroupBox("Color Temperature (Kelvin)");
    auto* temp_layout = new QVBoxLayout(temp_group);

    auto* temp_dial_layout = new QHBoxLayout();
    temp_dial_layout->addWidget(new QLabel("Warm\n(3000K)"));

    out_widgets.color_dial = new QDial();
    out_widgets.color_dial->setMinimum(3000);
    out_widgets.color_dial->setMaximum(8000);
    out_widgets.color_dial->setValue(5000);
    out_widgets.color_dial->setNotchesVisible(true);
    temp_dial_layout->addWidget(out_widgets.color_dial);

    temp_dial_layout->addWidget(new QLabel("Cool\n(8000K)"));
    temp_layout->addLayout(temp_dial_layout);

    out_widgets.color_temp_label = new QLabel("5000 K");
    out_widgets.color_temp_label->setAlignment(Qt::AlignCenter);
    out_widgets.color_temp_label->setStyleSheet("font-weight: bold; font-size: 12px;");
    temp_layout->addWidget(out_widgets.color_temp_label);

    layout->addWidget(temp_group);

    return group_box;
  }

  static QGroupBox* CreateRightPanel(RightPanelWidgets& out_widgets) {
    auto* group_box = new QGroupBox("Status & Preview");
    auto* layout = new QVBoxLayout(group_box);

    // Color preview frame
    layout->addWidget(new QLabel("Color Preview:"));
    out_widgets.color_preview_frame = new QFrame();
    out_widgets.color_preview_frame->setMinimumHeight(100);
    out_widgets.color_preview_frame->setStyleSheet(
        "background-color: white; border: 2px solid #ddd;");
    out_widgets.color_preview_frame->setFrameShape(QFrame::StyledPanel);
    layout->addWidget(out_widgets.color_preview_frame);

    layout->addSpacing(10);

    // Brightness display
    layout->addWidget(new QLabel("Brightness Display:"));
    out_widgets.brightness_lcd = new QLCDNumber();
    out_widgets.brightness_lcd->setDigitCount(3);
    out_widgets.brightness_lcd->setSegmentStyle(QLCDNumber::Filled);
    out_widgets.brightness_lcd->display("100");
    layout->addWidget(out_widgets.brightness_lcd);

    layout->addSpacing(10);

    // Information display
    layout->addWidget(new QLabel("Information:"));
    out_widgets.info_label = new QLabel();
    out_widgets.info_label->setWordWrap(true);
    out_widgets.info_label->setStyleSheet(
        "background-color: #f5f5f5; padding: 8px; border-radius: 3px;");
    layout->addWidget(out_widgets.info_label);

    layout->addStretch();

    return group_box;
  }
};

#endif  // LAB_UI_FACTORY_H_
