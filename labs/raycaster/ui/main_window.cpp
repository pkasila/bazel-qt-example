#include "labs/raycaster/ui/main_window.h"

#include "labs/raycaster/ui/render_widget.h"

#include <QCheckBox>
#include <QComboBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QWidget>

namespace raycaster {

MainWindow::MainWindow()
    : render_widget_(new RenderWidget(this))
    , soft_shadows_check_box_(new QCheckBox("Полутени", this))
    , cat_check_box_(new QCheckBox("Котофон", this))
    , mode_combo_box_(new QComboBox(this))
    , hint_label_(new QLabel(this)) {
    auto* central = new QWidget(this);
    auto* root_layout = new QVBoxLayout(central);
    auto* controls_layout = new QHBoxLayout();

    auto* title_label = new QLabel("Режим:", this);
    mode_combo_box_->addItem("light");
    mode_combo_box_->addItem("polygons");
    mode_combo_box_->addItem("static-lights");

    soft_shadows_check_box_->setChecked(true);
    cat_check_box_->setChecked(false);
    render_widget_->SetSoftShadowsEnabled(true);
    render_widget_->SetCatOverlayEnabled(false);

    auto* clear_scene_button = new QPushButton("Очистить сцену", this);
    auto* clear_static_lights_button = new QPushButton("Очистить статические источники", this);

    hint_label_->setWordWrap(true);

    controls_layout->addWidget(title_label);
    controls_layout->addWidget(mode_combo_box_);
    controls_layout->addSpacing(12);
    controls_layout->addWidget(soft_shadows_check_box_);
    controls_layout->addWidget(cat_check_box_);
    controls_layout->addSpacing(16);
    controls_layout->addWidget(clear_scene_button);
    controls_layout->addWidget(clear_static_lights_button);
    controls_layout->addStretch();

    root_layout->addLayout(controls_layout);
    root_layout->addWidget(hint_label_);
    root_layout->addWidget(render_widget_, 1);

    setCentralWidget(central);
    resize(1100, 800);
    setWindowTitle("Qt Raycaster");

    render_widget_->SetStatusCallback(
        [this](const QString& message) { statusBar()->showMessage(message, 4000); });

    connect(mode_combo_box_, &QComboBox::currentIndexChanged, this, [this](int index) {
        render_widget_->SetMode(ModeFromIndex(index));
        UpdateHintText();
        statusBar()->showMessage(
            QString("Текущий режим: %1").arg(mode_combo_box_->itemText(index)), 2000);
    });

    connect(soft_shadows_check_box_, &QCheckBox::toggled, this, [this](bool checked) {
        render_widget_->SetSoftShadowsEnabled(checked);
        UpdateHintText();
        statusBar()->showMessage(
            checked ? "Полутени включены: движется группа близких источников."
                    : "Полутени выключены: активен один управляемый источник.",
            3000);
    });

    connect(cat_check_box_, &QCheckBox::toggled, this, [this](bool checked) {
        render_widget_->SetCatOverlayEnabled(checked);
        UpdateHintText();
        statusBar()->showMessage(
            checked ? "Включен тёплый рыжий котофон." : "Котофон скрыт, интерфейс снова строгий.",
            3000);
    });

    connect(clear_scene_button, &QPushButton::clicked, this, [this]() {
        render_widget_->GetController().Clear();
        render_widget_->update();
        statusBar()->showMessage("Сцена очищена.", 3000);
    });

    connect(clear_static_lights_button, &QPushButton::clicked, this, [this]() {
        render_widget_->GetController().ClearStaticLights();
        render_widget_->update();
        statusBar()->showMessage("Статические источники очищены.", 3000);
    });

    UpdateHintText();
    statusBar()->showMessage("Готово к работе.", 2000);
}

InteractionMode MainWindow::ModeFromIndex(int index) {
    switch (index) {
        case 1:
            return InteractionMode::kPolygons;
        case 2:
            return InteractionMode::kStaticLights;
        case 0:
        default:
            return InteractionMode::kLight;
    }
}

void MainWindow::UpdateHintText() {
    QString text;
    switch (render_widget_->GetMode()) {
        case InteractionMode::kLight:
            text =
                render_widget_->AreSoftShadowsEnabled()
                    ? "light: веди мышью | полутени включены: двигается группа близких источников"
                    : "light: веди мышью | полутени выключены: активен один источник света";
            break;
        case InteractionMode::kPolygons:
            text =
                "polygons: ЛКМ — вершина, ПКМ — завершить | нельзя пересекать препятствия и "
                "накрывать источники";
            break;
        case InteractionMode::kStaticLights:
            text =
                "static-lights: ЛКМ — добавить, ПКМ — очистить | дополнительные голубые источники "
                "неподвижны";
            break;
    }

    if (cat_check_box_->isChecked()) {
        text += " | котофон включен";
    }

    hint_label_->setText(text);
}

}  // namespace raycaster
