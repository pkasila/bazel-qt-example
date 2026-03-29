//
// Created by Zazerg on 3/31/25.
//

#include "MainWindow.h"

#include <QPainter>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QPushButton>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSpinBox>
#include <QSlider>
#include <QTabWidget>
#include <QLabel>
#include <QStyle>

#include "RenderArea.h"

MainWindow::MainWindow(QWidget *parent) {
  setParent(parent);
  auto* layout = new QVBoxLayout();

  QHBoxLayout *hbox = new QHBoxLayout();
  QVBoxLayout *vbox = new QVBoxLayout();

  QTabWidget* tabWidget = new QTabWidget(this);
  tabWidget->setFixedHeight(150);


  group = new QButtonGroup(this);
  QRadioButton* poly = new QRadioButton("Polygons");
  poly->setParent(this);
  QRadioButton* light = new QRadioButton("Light");
  light->setParent(this);
  QRadioButton* staticLight = new QRadioButton("Static light");
  staticLight->setParent(this);
  QRadioButton* moveScreen = new QRadioButton("Move screen");
  moveScreen->setParent(this);
  group->addButton(poly);
  group->addButton(light);
  group->addButton(staticLight);
  group->addButton(moveScreen);
  group->setId(poly, 0);
  group->setId(light, 1);
  group->setId(staticLight, 2);
  group->setId(moveScreen, 3);
  group->setExclusive(true);
  poly->setChecked(true);
  vbox->addWidget(poly);
  vbox->addWidget(light);
  vbox->addWidget(staticLight);
  vbox->addWidget(moveScreen);

  hbox->addLayout(vbox);

  QWidget *tab2 = new QWidget();
  QVBoxLayout* tab2Layout = new QVBoxLayout(tab2);

  auto makeColor = [&] (int start, std::function<void(int)> slot) {
    auto spinBox = new QSpinBox(this);
    spinBox->setMinimum(0);
    spinBox->setMaximum(255);
    auto slider = new QSlider(Qt::Horizontal, this);
    slider->setMinimum(0);
    slider->setMaximum(255);
    connect(spinBox, &QSpinBox::valueChanged, slider, &QSlider::setValue);
    connect(slider, &QSlider::valueChanged, spinBox, &QSpinBox::setValue);
    connect(spinBox, &QSpinBox::valueChanged,  slot);
    slider->setValue(start);
    QHBoxLayout* color = new QHBoxLayout();
    color->addWidget(spinBox);
    color->addWidget(slider);
    return color;
  };

  QHBoxLayout* colorLayout = new QHBoxLayout();

  QHBoxLayout* color1Layout = new QHBoxLayout();
  QVBoxLayout* select1 = new QVBoxLayout();
  QLabel* label1 = new QLabel(this);
  label1->setStyleSheet("background-color: rgb(120, 60, 60);");
  select1->addWidget(new QLabel("Main color", this));
  select1->addLayout(makeColor(120, [label1, this] (int x) {settings_.color1.setRed(x); label1->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(settings_.color1.red()).arg(settings_.color1.green()).arg(settings_.color1.blue())); label1->style()->polish(label1); label1->update();}));
  select1->addLayout(makeColor(60, [label1, this] (int x) {settings_.color1.setGreen(x); label1->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(settings_.color1.red()).arg(settings_.color1.green()).arg(settings_.color1.blue())); label1->style()->polish(label1); label1->update();}));
  select1->addLayout(makeColor(60, [label1, this] (int x) {settings_.color1.setBlue(x); label1->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(settings_.color1.red()).arg(settings_.color1.green()).arg(settings_.color1.blue())); label1->style()->polish(label1); label1->update();}));
  color1Layout->addLayout(select1);
  color1Layout->addWidget(label1);
  colorLayout->addLayout(color1Layout);

  QHBoxLayout* color2Layout = new QHBoxLayout();
  QVBoxLayout* select2 = new QVBoxLayout();
  QLabel* label2 = new QLabel(this);
  label2->setStyleSheet("background-color: rgb(60, 30, 30);");
  select2->addWidget(new QLabel("Half shadow color", this));
  select2->addLayout(makeColor(60, [label2, this] (int x) {settings_.color2.setRed(x); label2->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(settings_.color2.red()).arg(settings_.color2.green()).arg(settings_.color2.blue())); label2->style()->polish(label2); label2->update();}));
  select2->addLayout(makeColor(30, [label2, this] (int x) {settings_.color2.setGreen(x); label2->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(settings_.color2.red()).arg(settings_.color2.green()).arg(settings_.color2.blue())); label2->style()->polish(label2); label2->update();}));
  select2->addLayout(makeColor(30, [label2, this] (int x) {settings_.color2.setBlue(x); label2->setStyleSheet(QString("background-color: rgb(%1, %2, %3)").arg(settings_.color2.red()).arg(settings_.color2.green()).arg(settings_.color2.blue())); label2->style()->polish(label2); label2->update();}));
  color2Layout->addLayout(select2);
  color2Layout->addWidget(label2);
  colorLayout->addLayout(color2Layout);


  QSpinBox* radiusSpinBox = new QSpinBox(this);
  radiusSpinBox->setMinimum(1);
  radiusSpinBox->setMaximum(500);
  auto radiusSlider = new QSlider(Qt::Vertical, this);
  radiusSlider->setMinimum(1);
  radiusSlider->setMaximum(500);
  connect(radiusSpinBox, &QSpinBox::valueChanged, radiusSlider, &QSlider::setValue);
  connect(radiusSlider, &QSlider::valueChanged, radiusSpinBox, &QSpinBox::setValue);
  connect(radiusSpinBox, &QSpinBox::valueChanged, [this] (int x) {settings_.radius = x;});
  radiusSpinBox->setValue(400);
  QRadioButton* enableRadius = new QRadioButton("Radius", this);
  enableRadius->setChecked(true);
  connect(enableRadius, &QRadioButton::clicked, [this, radiusSlider, radiusSpinBox] (bool x) {radiusSlider->setEnabled(x); radiusSpinBox->setEnabled(x); if (!x) {settings_.radius = 0;} else {settings_.radius = radiusSlider->value();}});
  QVBoxLayout* radiusLayout = new QVBoxLayout();
  radiusLayout->addWidget(radiusSpinBox);
  radiusLayout->addWidget(radiusSlider);
  radiusLayout->addWidget(enableRadius);
  colorLayout->addLayout(radiusLayout);

  tab2Layout->addLayout(colorLayout);

  tab2->setLayout(tab2Layout);
  tabWidget->addTab(tab2, "Colors");

  hbox->addWidget(tabWidget);

  layout->addLayout(hbox);
  // QRadioButton* clipping = new QRadioButton("clipping");
  // layout->addWidget(clipping);
  settings_ = Settings();
  render_area_ = new RenderArea(settings_, this);
  layout->addWidget(render_area_);

  QWidget* widget = new QWidget(this);
  widget->setLayout(layout);
  setCentralWidget(widget);

  connect(group, &QButtonGroup::buttonToggled, this, &MainWindow::checked);
  // connect(red, &QSpinBox::valueChanged, this, &MainWindow::colorEdit);
  // connect(green, &QSpinBox::valueChanged, this, &MainWindow::colorEdit);
  // connect(blue, &QSpinBox::valueChanged, this, &MainWindow::colorEdit);
  // connect(red1, &QSlider::valueChanged, this, &MainWindow::colorEdit1);
  // connect(green1, &QSlider::valueChanged, this, &MainWindow::colorEdit1);
  // connect(blue1, &QSlider::valueChanged, this, &MainWindow::colorEdit1);

  // connect(clipping, &QRadioButton::toggled, this, &MainWindow::clippingChecked);
}

void MainWindow::checked() {
  settings_.mode=Mode(group->checkedId());
}

void MainWindow::colorEdit() {
  settings_.color1 = QColor(red->value(), green->value(), blue->value());
}

void MainWindow::colorEdit1() {
  settings_.color2 = QColor(red1->value(), green1->value(), blue1->value());
}


