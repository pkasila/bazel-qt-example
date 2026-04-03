#include "painttoolbar.h"
#include "canvas.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QRadioButton>
#include <QLabel>
#include <QColorDialog>

PaintToolBar::PaintToolBar(QWidget *parent, Canvas *canvas) : QWidget(parent){
    color = Qt::black;
    this->canvas = canvas;
    setFixedHeight(100);
    setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout(this);

    layout->addStretch(1);

    /// Tool type
    QVBoxLayout *layoutToolGroup = new QVBoxLayout;
    buttonGroupToolType = new QButtonGroup(this);

    radioButtonPencil = new QRadioButton("Pencil", this);
    radioButtonPencil->setChecked(true);
    radioButtonBrush = new QRadioButton("Brush", this);

    connect(radioButtonBrush, &QRadioButton::clicked, [canvas](){
        canvas->setTool(Canvas::Tool::Brush);
    });

    connect(radioButtonPencil, &QRadioButton::clicked, [canvas](){
        canvas->setTool(Canvas::Tool::Pencil);
    });

    buttonGroupToolType->addButton(radioButtonBrush);
    buttonGroupToolType->addButton(radioButtonPencil);

    labelToolType = new QLabel("Tool type:", this);

    layoutToolGroup->addWidget(labelToolType);
    layoutToolGroup->addWidget(radioButtonPencil);
    layoutToolGroup->addWidget(radioButtonBrush);

    layout->addLayout(layoutToolGroup);
    layout->addStretch(1);

    /// Pen Width
    comboBoxPenWidth = new QComboBox(this);

    comboBoxPenWidth->addItem("1 px");
    comboBoxPenWidth->addItem("2 px");
    comboBoxPenWidth->addItem("5 px");
    comboBoxPenWidth->addItem("10 px");
    comboBoxPenWidth->addItem("30 px");

    comboBoxPenWidth->setFixedWidth(70);

    connect(comboBoxPenWidth,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &PaintToolBar::setPenWidth
            );

    QLabel *labelPenWidth = new QLabel("Pen width:");

    QHBoxLayout *layoutPenWidth = new QHBoxLayout;
    layoutPenWidth->addWidget(labelPenWidth);
    layoutPenWidth->addSpacing(10);
    layoutPenWidth->addWidget(comboBoxPenWidth);

    layout->addLayout(layoutPenWidth);
    layout->addStretch(1);


    /// Color
    QVBoxLayout *layoutCurrentColor = new QVBoxLayout;

    currentColor = new ColorWidget(this);
    QHBoxLayout *layoutForCurrentColorWidget = new QHBoxLayout;
    layoutForCurrentColorWidget->addStretch(1);
    layoutForCurrentColorWidget->addWidget(currentColor);
    layoutForCurrentColorWidget->addStretch(1);
    QLabel *labelCurrentColor = new QLabel("Current color", this);
    layoutCurrentColor->addStretch(1);
    layoutCurrentColor->addLayout(layoutForCurrentColorWidget);
    layoutCurrentColor->addWidget(labelCurrentColor);
    layoutCurrentColor->addStretch(1);

    QVBoxLayout *layoutRandomColor = new QVBoxLayout;
    randomColor = new ColorWidget(this);
    randomColor->random = true;
    QHBoxLayout *layoutForRandomColorWidget = new QHBoxLayout;
    layoutForRandomColorWidget->addStretch(1);
    layoutForRandomColorWidget->addWidget(randomColor);
    layoutForRandomColorWidget->addStretch(1);
    QLabel *labelRandomColor = new QLabel("Change color", this);
    layoutRandomColor->addStretch(1);
    layoutRandomColor->addLayout(layoutForRandomColorWidget);
    layoutRandomColor->addWidget(labelRandomColor);
    layoutRandomColor->addStretch(1);

    layout->addLayout(layoutCurrentColor);
    layout->addSpacing(30);
    layout->addLayout(layoutRandomColor);
    layout->addStretch(3);

    setLayout(layout);
}

void PaintToolBar::setColor() {
    QColor newColor = QColorDialog::getColor(color, this, "Choose new color:");
    if (newColor.isValid()) {
        color = newColor;
    }
    currentColor->setColor(newColor);
    canvas->setColor(newColor);
}

void PaintToolBar::setPenWidth(int newPenWidthIndex) {
    switch(newPenWidthIndex){
    case 0:
        penWidth = 1;
        break;
    case 1:
        penWidth = 2;
        break;
    case 2:
        penWidth = 5;
        break;
    case 3:
        penWidth = 10;
        break;
    case 4:
        penWidth = 30;
        break;
    default:
        penWidth = 2;
    }
    canvas->setPenWidth(penWidth);
}

void PaintToolBar::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.fillRect(rect(), QColor("#eeeeee"));
}


PaintToolBar::~PaintToolBar() {

}

/// ColorWidget
ColorWidget::ColorWidget(QWidget *parent)  : QWidget(parent) {
    setFixedSize(30, 30);
    color = Qt::black;
    toolBar = static_cast<PaintToolBar*>(parent);
}

void ColorWidget::setColor(QColor newColor) {
    color = newColor;
    update();
}

void ColorWidget::randFill() {
    QPainter painter(this);
    for (int i = 0; i < height(); i++) {
        for (int j = 0; j < width(); j++) {
            int r = rand() % 256;
            int g = rand() % 256;
            int b = rand() % 256;
            painter.setPen(QPen(QColor(r, g, b), 1));
            painter.drawPoint(QPoint(i, j));
        }
    }
}

void ColorWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        toolBar->setColor();
    }
}
