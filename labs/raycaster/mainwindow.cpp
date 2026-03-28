#include "mainwindow.h"
#include <QApplication>
#include <QMenuBar>
#include <QStatusBar>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , central_widget_(nullptr)
    , render_widget_(nullptr)
    , mode_combo_(nullptr)
    , clear_button_(nullptr)
    , status_label_(nullptr)
    , main_layout_(nullptr)
    , control_layout_(nullptr) {
    
    setupUI();
    setupLayout();
    
    setWindowTitle("2D Raycaster");
    resize(800, 600);
}

MainWindow::~MainWindow() = default;

void MainWindow::setupUI() {
    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);
    
    render_widget_ = new RenderWidget(&controller_, this);
    
    mode_combo_ = new QComboBox(this);
    mode_combo_->addItem("Light", static_cast<int>(InteractionMode::Light));
    mode_combo_->addItem("Polygons", static_cast<int>(InteractionMode::Polygons));
    mode_combo_->addItem("Static Lights", static_cast<int>(InteractionMode::StaticLights));
    
    clear_button_ = new QPushButton("Clear All", this);
    
    status_label_ = new QLabel("Mode: Light", this);

    connect(mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int index) { onModeChanged(); });
    connect(clear_button_, &QPushButton::clicked,
            [this]() { render_widget_->clearAll(); });
}

void MainWindow::setupLayout() {
    main_layout_ = new QVBoxLayout(central_widget_);
    
    control_layout_ = new QHBoxLayout();
    control_layout_->addWidget(new QLabel("Mode:", this));
    control_layout_->addWidget(mode_combo_);
    control_layout_->addWidget(clear_button_);
    control_layout_->addWidget(status_label_);
    control_layout_->addStretch();
    
    main_layout_->addLayout(control_layout_);
    main_layout_->addWidget(render_widget_, 1);
}

void MainWindow::onModeChanged() {
    int mode_data = mode_combo_->currentData().toInt();
    InteractionMode mode = static_cast<InteractionMode>(mode_data);
    
    render_widget_->setMode(mode);
    
    QString mode_text;
    switch (mode) {
        case InteractionMode::Light:
            mode_text = "Light";
            break;
        case InteractionMode::Polygons:
            mode_text = "Polygons";
            break;
        case InteractionMode::StaticLights:
            mode_text = "Static Lights";
            break;
    }
    
    status_label_->setText("Mode: " + mode_text);
}
