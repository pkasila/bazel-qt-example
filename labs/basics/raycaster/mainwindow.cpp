#include "mainwindow.h"
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    setCentralWidget(centralWidget);

    modeComboBox_ = new QComboBox(this);
    modeComboBox_->addItem("Light Mode");
    modeComboBox_->addItem("Polygon Mode");
    connect(modeComboBox_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    graphicsView_ = new CustomGraphicsView(this);
    graphicsView_->setFixedSize(800, 600);
    layout->addWidget(modeComboBox_);
    layout->addWidget(graphicsView_);

    connect(graphicsView_, &CustomGraphicsView::sceneMousePressed, this,
            [this](QPointF scenePos, Qt::MouseButton button) {
                if (isLightMode_) {
                    lightPos_ = scenePos;
                    controller_.setLightSource(lightPos_);
                } else {
                    handlePolygonDrawing(scenePos, button);
                }
                updateView();
            });

    connect(graphicsView_, &CustomGraphicsView::sceneMouseMoved, this,
            [this](QPointF scenePos) {
                if (isLightMode_) {
                    lightPos_ = scenePos;
                    controller_.setLightSource(lightPos_);
                } else if (isDrawingPolygon_ && !currentPolygon_.empty()) {
                    currentPolygon_.back() = scenePos;
                }
                updateView();
            });

    std::vector<QPointF> border = {{0, 0}, {800, 0}, {800, 600}, {0, 600}};
    controller_.addPolygon(Polygon(border));

    lightPos_ = QPointF(400, 300);
    controller_.setLightSource(lightPos_);
    updateView();
}

MainWindow::~MainWindow() {}

void MainWindow::onModeChanged(int index) {
    isLightMode_ = (index == 0);
    isFirstClick_ = true;
    isDrawingPolygon_ = false;
    currentPolygon_.clear();
    updateView();
}

void MainWindow::updateView() {
    std::vector<std::vector<QPointF>> allPolys;
    for (const auto& p : controller_.getPolygons()) {
        allPolys.push_back(p.getVertices());
    }

    graphicsView_->setPolygons(allPolys);
    graphicsView_->setCurrentPolygon(currentPolygon_);
    graphicsView_->setLight(lightPos_);
    graphicsView_->setMode(isLightMode_);

    if (isLightMode_) {
        graphicsView_->lightArea = controller_.createLightArea().getVertices();
    } else {
        graphicsView_->lightArea.clear();
    }

    graphicsView_->viewport()->update();
}

void MainWindow::handlePolygonDrawing(QPointF scenePos, Qt::MouseButton button) {
    if (button == Qt::LeftButton) {
        if (isFirstClick_) {
            currentPolygon_.clear();
            currentPolygon_.push_back(scenePos);
            isFirstClick_ = false;
            isDrawingPolygon_ = true;
        } else {
            currentPolygon_.push_back(scenePos);
        }
    } else if (button == Qt::RightButton && isDrawingPolygon_) {
        if (currentPolygon_.size() > 2) {
            controller_.addPolygon(Polygon(currentPolygon_));
        }
        currentPolygon_.clear();
        isDrawingPolygon_ = false;
        isFirstClick_ = true;
    }
    updateView();
}
