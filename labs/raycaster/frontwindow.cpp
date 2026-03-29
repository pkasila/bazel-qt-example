#include "frontwindow.h"

#include "canvas.h"
#include "utils.h"

#include <QComboBox>
#include <QHBoxLayout>
#include <QPushButton>
#include <QVBoxLayout>

QWidget* createMainWindow() {
    QWidget* mainWin = new QWidget;
    QVBoxLayout* mainLayout = new QVBoxLayout(mainWin);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    QWidget* topPanel = new QWidget(mainWin);
    topPanel->setFixedHeight(GlobalConfig::TOP_PANEL_HEIGHT);
    QHBoxLayout* topLayout = new QHBoxLayout(topPanel);
    topLayout->setContentsMargins(10, 10, 10, 5);
    topLayout->setSpacing(10);

    QComboBox* modeSwitcher = new QComboBox(topPanel);
    modeSwitcher->addItem("Light");
    modeSwitcher->addItem("Polygons");
    topLayout->addWidget(modeSwitcher, 0, Qt::AlignLeft);

    topLayout->addStretch();

    QPushButton* fpsIndicator = new QPushButton("FPS: N/A", topPanel);
    fpsIndicator->setEnabled(false);
    topLayout->addWidget(fpsIndicator, 0, Qt::AlignRight);
    mainLayout->addWidget(topPanel);

    QWidget* drawArea = new QWidget(mainWin);
    QVBoxLayout* drawLayout = new QVBoxLayout(drawArea);
    drawLayout->setContentsMargins(10, 5, 10, 10);
    drawLayout->setSpacing(0);

    CanvasWidget* canvas = new CanvasWidget(drawArea);
    drawLayout->addWidget(canvas);
    mainLayout->addWidget(drawArea);

    QObject::connect(
        modeSwitcher, QOverload<int>::of(&QComboBox::currentIndexChanged), [canvas](int index) {
            if (index == 0) {
                canvas->setRenderMode(RenderMode::Light);
            } else {
                canvas->setRenderMode(RenderMode::Polygons);
            }
        });

    return mainWin;
}
