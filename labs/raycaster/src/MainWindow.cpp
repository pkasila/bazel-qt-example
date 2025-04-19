#include "MainWindow.h"

MainWindow::MainWindow(QWidget *parent): QMainWindow(parent) {
    setWindowTitle(Titles::mainWindowTitle);
    resize(Proportions::mainWindowSize);
    setMinimumSize(Proportions::minMainWindowSize);

    QWidget *central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout *mainLayout = new QHBoxLayout(central);

    //Toolbar layout
    QVBoxLayout *toolbarLayout = new QVBoxLayout();
    toolbarLayout->setAlignment(Qt::AlignTop);

    lightButton = new QToolButton(this);
    lightButton->setFixedSize(Proportions::lightButtonSize);
    lightButton->setIcon(QIcon(Paths::lightIconPath));
    lightButton->setIconSize(Proportions::lightIconSize);
    lightButton->setToolTip(Texts::lightButtonComment);
    lightButton->setCheckable(true);
    lightButton->setStyleSheet(Styles::lightButtonStyle);

    polygonButton = new QToolButton(this);
    polygonButton->setFixedSize(Proportions::polygonButtonSize);
    polygonButton->setIcon(QIcon(Paths::polygonIconPath));
    polygonButton->setIconSize(Proportions::polygonIconSize);
    polygonButton->setToolTip(Texts::polygonButtonComment);
    polygonButton->setCheckable(true);
    polygonButton->setStyleSheet(Styles::lightButtonStyle);

    toolbarLayout->addWidget(lightButton);
    toolbarLayout->addWidget(polygonButton);

    //Drawing area frame
    QFrame *drawingAreaFrame = new QFrame(this);
    drawingAreaFrame->setFrameShape(QFrame::Box);
    drawingAreaFrame->setStyleSheet(Styles::drawingAreaFrameStyle);

    //Drawing area
    drawingArea = new DrawingArea(drawingAreaFrame);
    drawingArea->setMinimumSize(Proportions::drawingAreaMinSize);

    QVBoxLayout *drawingLayout = new QVBoxLayout(drawingAreaFrame);
    drawingLayout->addWidget(drawingArea);
    drawingLayout->setContentsMargins(Proportions::drawingAreaFrameMargins);
    drawingAreaFrame->setLayout(drawingLayout);

    mainLayout->addLayout(toolbarLayout);
    mainLayout->addWidget(drawingAreaFrame);

    connect(lightButton, &QToolButton::clicked, this, [=]() {
        drawingArea->setMode(Mode::Light);
        updateModeButtons(Mode::Light);
    });
    connect(polygonButton, &QToolButton::clicked, this, [=]() {
        drawingArea->setMode(Mode::Polygons);
        updateModeButtons(Mode::Polygons);
    });

    updateModeButtons(Mode::Polygons);
}

void MainWindow::updateModeButtons(Mode mode) {
    lightButton->setChecked(mode == Mode::Light);
    polygonButton->setChecked(mode == Mode::Polygons);
}
