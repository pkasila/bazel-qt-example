#include "color_mixer.h"

#include <QClipboard>
#include <QRandomGenerator>

ColorMixer::ColorMixer(QWidget* parent) : QMainWindow(parent), currentColor(Qt::red) {
    createWidgets();
    createLayouts();
    createConnections();

    setWindowTitle(tr("Color Mixer - Смешивание цветов"));
    resize(500, 400);

    onRedSliderChanged(255);
    onGreenSliderChanged(0);
    onBlueSliderChanged(0);
    onAlphaSliderChanged(255);
}

void ColorMixer::createWidgets() {
    colorLabel = new QLabel(tr("Цвет будет здесь"), this);
    colorLabel->setMinimumSize(200, 100);
    colorLabel->setAlignment(Qt::AlignCenter);
    colorLabel->setAutoFillBackground(true);
    updateColorDisplay();

    colorNameEdit = new QLineEdit(this);
    colorNameEdit->setPlaceholderText(tr("Название цвета"));

    redSlider = new QSlider(Qt::Horizontal, this);
    redSlider->setRange(0, 255);
    redSlider->setValue(255);

    greenSlider = new QSlider(Qt::Horizontal, this);
    greenSlider->setRange(0, 255);
    greenSlider->setValue(0);

    blueSlider = new QSlider(Qt::Horizontal, this);
    blueSlider->setRange(0, 255);
    blueSlider->setValue(0);

    alphaSlider = new QSlider(Qt::Horizontal, this);
    alphaSlider->setRange(0, 255);
    alphaSlider->setValue(255);

    redLcd = new QLCDNumber(3, this);
    redLcd->display(255);
    redLcd->setSegmentStyle(QLCDNumber::Filled);

    greenLcd = new QLCDNumber(3, this);
    greenLcd->display(0);
    greenLcd->setSegmentStyle(QLCDNumber::Filled);

    blueLcd = new QLCDNumber(3, this);
    blueLcd->display(0);
    blueLcd->setSegmentStyle(QLCDNumber::Filled);

    alphaLcd = new QLCDNumber(3, this);
    alphaLcd->display(255);
    alphaLcd->setSegmentStyle(QLCDNumber::Filled);

    opacityBar = new QProgressBar(this);
    opacityBar->setRange(0, 100);
    opacityBar->setValue(100);
    opacityBar->setFormat("%p%");

    QLabel* redLabel = new QLabel(tr("Красный:"), this);
    QLabel* greenLabel = new QLabel(tr("Зеленый:"), this);
    QLabel* blueLabel = new QLabel(tr("Синий:"), this);
    QLabel* alphaLabel = new QLabel(tr("Прозрачность:"), this);

    copyButton = new QPushButton(tr("Копировать цвет"), this);
    pasteButton = new QPushButton(tr("Вставить цвет"), this);
    randomButton = new QPushButton(tr("Случайный цвет"), this);

    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
}

void ColorMixer::createLayouts() {
    mainLayout = new QVBoxLayout(centralWidget);

    mainLayout->addWidget(colorLabel);
    mainLayout->addWidget(colorNameEdit);

    sliderLayout = new QGridLayout();
    sliderLayout->addWidget(new QLabel(tr("Красный:"), this), 0, 0);
    sliderLayout->addWidget(redSlider, 0, 1);
    sliderLayout->addWidget(redLcd, 0, 2);

    sliderLayout->addWidget(new QLabel(tr("Зеленый:"), this), 1, 0);
    sliderLayout->addWidget(greenSlider, 1, 1);
    sliderLayout->addWidget(greenLcd, 1, 2);

    sliderLayout->addWidget(new QLabel(tr("Синий:"), this), 2, 0);
    sliderLayout->addWidget(blueSlider, 2, 1);
    sliderLayout->addWidget(blueLcd, 2, 2);

    sliderLayout->addWidget(new QLabel(tr("Прозрачность:"), this), 3, 0);
    sliderLayout->addWidget(alphaSlider, 3, 1);
    sliderLayout->addWidget(alphaLcd, 3, 2);

    mainLayout->addLayout(sliderLayout);

    mainLayout->addWidget(opacityBar);

    buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(copyButton);
    buttonLayout->addWidget(pasteButton);
    buttonLayout->addWidget(randomButton);
    mainLayout->addLayout(buttonLayout);
}

void ColorMixer::createConnections() {
    connect(redSlider, &QSlider::valueChanged, this, &ColorMixer::onRedSliderChanged);
    connect(greenSlider, &QSlider::valueChanged, this, &ColorMixer::onGreenSliderChanged);
    connect(blueSlider, &QSlider::valueChanged, this, &ColorMixer::onBlueSliderChanged);
    connect(alphaSlider, &QSlider::valueChanged, this, &ColorMixer::onAlphaSliderChanged);

    connect(copyButton, &QPushButton::clicked, this, &ColorMixer::onCopyColor);
    connect(pasteButton, &QPushButton::clicked, this, &ColorMixer::onPasteColor);
    connect(randomButton, &QPushButton::clicked, this, &ColorMixer::onRandomColor);

    connect(colorNameEdit, &QLineEdit::textChanged, this, &ColorMixer::onColorNameChanged);
}

void ColorMixer::onRedSliderChanged(int value) {
    currentColor.setRed(value);
    redLcd->display(value);
    updateColorDisplay();
}

void ColorMixer::onGreenSliderChanged(int value) {
    currentColor.setGreen(value);
    greenLcd->display(value);
    updateColorDisplay();
}

void ColorMixer::onBlueSliderChanged(int value) {
    currentColor.setBlue(value);
    blueLcd->display(value);
    updateColorDisplay();
}

void ColorMixer::onAlphaSliderChanged(int value) {
    currentColor.setAlpha(value);
    alphaLcd->display(value);
    opacityBar->setValue(value * 100 / 255);
    updateColorDisplay();
}

void ColorMixer::onCopyColor() {
    QClipboard* clipboard = QApplication::clipboard();
    QString colorText =
        QString("#%1%2%3%4")
            .arg(currentColor.alpha(), 2, 16, QChar('0'))
            .arg(currentColor.red(), 2, 16, QChar('0'))
            .arg(currentColor.green(), 2, 16, QChar('0'))
            .arg(currentColor.blue(), 2, 16, QChar('0'));
    clipboard->setText(colorText);
}

void ColorMixer::onPasteColor() {
    QClipboard* clipboard = QApplication::clipboard();
    QString text = clipboard->text();

    QColor pastedColor(text);
    if (pastedColor.isValid()) {
        currentColor = pastedColor;

        redSlider->blockSignals(true);
        greenSlider->blockSignals(true);
        blueSlider->blockSignals(true);
        alphaSlider->blockSignals(true);

        redSlider->setValue(currentColor.red());
        greenSlider->setValue(currentColor.green());
        blueSlider->setValue(currentColor.blue());
        alphaSlider->setValue(currentColor.alpha());

        redSlider->blockSignals(false);
        greenSlider->blockSignals(false);
        blueSlider->blockSignals(false);
        alphaSlider->blockSignals(false);

        redLcd->display(currentColor.red());
        greenLcd->display(currentColor.green());
        blueLcd->display(currentColor.blue());
        alphaLcd->display(currentColor.alpha());
        opacityBar->setValue(currentColor.alpha() * 100 / 255);

        updateColorDisplay();
    }
}

void ColorMixer::onRandomColor() {
    int r = QRandomGenerator::global()->bounded(256);
    int g = QRandomGenerator::global()->bounded(256);
    int b = QRandomGenerator::global()->bounded(256);

    currentColor.setRgb(r, g, b);

    redSlider->blockSignals(true);
    greenSlider->blockSignals(true);
    blueSlider->blockSignals(true);
    alphaSlider->blockSignals(true);

    redSlider->setValue(r);
    greenSlider->setValue(g);
    blueSlider->setValue(b);

    redSlider->blockSignals(false);
    greenSlider->blockSignals(false);
    blueSlider->blockSignals(false);
    alphaSlider->blockSignals(false);

    redLcd->display(r);
    greenLcd->display(g);
    blueLcd->display(b);

    updateColorDisplay();
}

void ColorMixer::onColorNameChanged(const QString& text) {
    Q_UNUSED(text);
}

void ColorMixer::updateColorDisplay() {
    QPalette palette = colorLabel->palette();
    palette.setColor(QPalette::Window, currentColor);
    colorLabel->setPalette(palette);

    int brightness =
        (currentColor.red() * 299 + currentColor.green() * 587 + currentColor.blue() * 114) / 1000;
    if (brightness > 125) {
        colorLabel->setStyleSheet("QLabel { color: black; border: 1px solid gray; }");
    } else {
        colorLabel->setStyleSheet("QLabel { color: white; border: 1px solid gray; }");
    }

    colorLabel->setText(currentColor.name(QColor::HexArgb));
}
