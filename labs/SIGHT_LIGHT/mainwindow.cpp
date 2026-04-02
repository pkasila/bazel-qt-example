#include "mainwindow.h"
#include <QVBoxLayout>
#include <QGraphicsEllipseItem>
#include <QGraphicsPolygonItem>
#include <QMouseEvent>

#include <QPalette>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_scene(new QGraphicsScene(this)),
    m_view(new QGraphicsView(m_scene, this)),
    m_modeComboBox(new QComboBox(this)),
    m_controller(new Controller()),
    m_drawingPolygon(false),
    m_currentMode(0)
{
    setupUI();
    setFixedSize(800, 600);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
}

MainWindow::~MainWindow()
{
    delete m_controller;
}

void MainWindow::setupUI()
{
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* layout = new QVBoxLayout(centralWidget);

    m_modeComboBox = new QComboBox(this);
    m_modeComboBox->addItem("Light Mode");
    m_modeComboBox->addItem("Polygons Mode");
    connect(m_modeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

   // m_view = new QGraphicsView(m_scene, this);
    m_view->setRenderHint(QPainter::Antialiasing);
    m_view->setSceneRect(0, 0, 800, 600);
    m_view->setBackgroundBrush(QPalette().base());
    // m_view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    // m_view->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    m_view->viewport()->installEventFilter(this);

    layout->addWidget(m_modeComboBox);
    layout->addWidget(m_view);

    QPoint startPos(300, 300);

    QCursor::setPos(startPos);

    setCentralWidget(centralWidget);

    updateScene();
}

void MainWindow::onModeChanged(int index)
{
    m_currentMode = index;
    updateScene();
}

bool MainWindow::eventFilter(QObject* watched, QEvent* event)
{
    if (watched == m_view->viewport()) {
        if (event->type() == QEvent::MouseButtonPress) {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF scenePos = m_view->mapToScene(mouseEvent->pos());
            onMousePressed(scenePos, mouseEvent->button());
            return true;
        }
        else if (event->type() == QEvent::MouseMove) {
            auto mouseEvent = static_cast<QMouseEvent*>(event);
            QPointF scenePos = m_view->mapToScene(mouseEvent->pos());
            onMouseMoved(scenePos);
            return true;
        }
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainWindow::onMousePressed(QPointF pos, Qt::MouseButton button)
{
    if (m_currentMode == 1) { // Polygons mode
        QPointF point(pos.x(), pos.y());

        if (button == Qt::LeftButton) {
            if (!m_drawingPolygon) {
                // Start new polygon
                m_controller->addPolygon(Polygon());
                m_controller->addVertexToLastPolygon(point);
                m_drawingPolygon = true;
            } else {
                // Add vertex to current polygon
                m_controller->addVertexToLastPolygon(point);
            }
        } else if (button == Qt::RightButton && m_drawingPolygon) {
            // Finish current polygon
            m_drawingPolygon = false;
        }
    }

    updateScene();
}

void MainWindow::onMouseMoved(QPointF pos)
{
    if (m_currentMode == 0) { // Light mode
        m_controller->setLightSource(QPointF(pos.x(), pos.y()));
        updateScene();
    } else if (m_currentMode == 1 && m_drawingPolygon) { // Polygons mode and drawing
        QPoint point(pos.x(), pos.y());
        m_controller->updateLastPolygon(point);
        updateScene();
    }
}

void MainWindow::updateScene()
{
    m_scene->clear();
    drawPolygons();

    //drawLightSources();
    QVector<QPointF> vertices = {
        QPointF(1, 1),
        QPointF(799, 1),
        QPointF(799, 599),
        QPointF(1, 599)
    };
    QGraphicsPolygonItem* item = m_scene->addPolygon(
        QPolygonF(vertices),
        QPen(Qt::black),
        QBrush(Qt::NoBrush)
        );
        item->setZValue(1);
    if (m_currentMode == 0) {


        auto vertices = m_controller->getLightSources();
        for(const auto& point : vertices) {
            drawLightSource(point);
            drawLightArea(point);
        }
    }
}

void MainWindow::drawLightArea(const QPointF& source)
{
    auto lightPoints = m_controller->calculateLightAreaForSource(source);
     const auto& vertices = lightPoints.getVertices();

    if (lightPoints.vertexCount() < 3) return;

    // Создаем QPolygonF вручную из точек
    QVector<QPointF> polygonPoints;
    polygonPoints.reserve(vertices.size());

    for (const auto& point : vertices) {
        polygonPoints.append(QPointF(point));
    }

    QGraphicsPolygonItem* item = m_scene->addPolygon(
        QPolygonF(polygonPoints),
        QPen(Qt::NoPen),
        QBrush(QColor(255, 150, 150, 50))
        );
    item->setZValue(1);

    // const int vertexRadius = 5;
    // for (const auto& vertex : vertices) {
    //     QGraphicsEllipseItem* vertexItem = m_scene->addEllipse(
    //         vertex.x() - vertexRadius,
    //         vertex.y() - vertexRadius,
    //         vertexRadius * 2,
    //         vertexRadius * 2,
    //         QPen(Qt::NoPen),     // Без контура
    //         QBrush(Qt::red)      // Красная заливка
    //         );
    //     vertexItem->setZValue(2); // Вершины поверх многоугольника

    // }
}



// void MainWindow::drawLightArea()
// {
//     auto lightAreas = m_controller->createLightAreas();

//     for (size_t i = 0; i < lightAreas.size(); i++) {
//         std::vector<QPointF> vertices = lightAreas[i].getVertices();
//         QVector<QPointF> polygonPoints = {};
//         polygonPoints.reserve(vertices.size());

//         for (const auto& point : vertices) {
//             polygonPoints.append(QPointF(point));
//         }
//         if (vertices.size() >= 3) {
//             QGraphicsPolygonItem* item = m_scene->addPolygon(
//                 QPolygonF(polygonPoints),
//                 QPen(Qt::NoPen),
//                 QBrush(QColor(QColor(171, 36, 36, 100)))
//                 );

//         }
//     }
// }

void MainWindow::drawLightAreas() {
    // Очистка предыдущих элементов


    // Получаем все световые области
    auto lightAreas = m_controller->calculateAllLightAreas();

    // Настройки визуализации
    const QColor lightColor(255, 255, 0, 50); // Полупрозрачный желтый
    const QPen outlinePen(Qt::NoPen); // Без контура

    // Рисуем каждую область
    for (const auto& area : lightAreas) {
        QVector<QPointF> points;
        for (const auto& p : area.getVertices()) {
            points << QPointF(p);
        }

        auto* areaItem = m_scene->addPolygon(
            QPolygonF(points),
            outlinePen,
            QBrush(lightColor)
            );
        areaItem->setZValue(1);
        m_lightAreaItems.push_back(areaItem);
    }
}



void MainWindow::updateLightSources(const QPointF& mainPos) {
    std::vector<QPointF> sources;
    const int radius = 10; // Радиус рассеивания
    const int count = 5;   // Количество источников

    for (int i = 0; i < count; ++i) {
        double angle = 2 * M_PI * i / count;
        QPoint offset(radius * cos(angle), radius * sin(angle));
        sources.push_back(mainPos + offset);
    }

    m_controller->setLightSources(sources);
    drawLightAreas();
}

void MainWindow::drawLightSources() {
    // Очистка старых источников
    for (auto* item : m_lightSourceItems) {
        m_scene->removeItem(item);
        delete item;
    }
    m_lightSourceItems.clear();

    // Рисование новых
    const int radius = 30;
    for (const auto& source : m_controller->getLightSources()) {
        auto* item = m_scene->addEllipse(
            source.x() - radius,
            source.y() - radius,
            radius * 2,
            radius * 2,
            QPen(Qt::black),
            QBrush(Qt::yellow)
            );
        item->setZValue(3);
        m_lightSourceItems.push_back(item);
    }
}


void MainWindow::drawPolygons()
{
    for (const auto& polygon : m_controller->getPolygons()) {
        auto vertices = polygon.getVertices();

        if (vertices.size() < 2) continue;

        QPolygonF qpolygon;
        for (const auto& vertex : vertices) {
            qpolygon << QPointF(vertex);
        }

        QGraphicsPolygonItem* item = m_scene->addPolygon(qpolygon, QPen(Qt::black), QBrush(Qt::gray));
        item->setZValue(0);
    }
}

// В MainWindow
void MainWindow::mouseMoveEvent(QMouseEvent* event) {
    updateLightSources(event->pos());
    drawLightSources(); // Обновляем позиции источников
    QMainWindow::mouseMoveEvent(event);
}

void MainWindow::drawLightSource(const QPointF& light)
{
    QGraphicsEllipseItem* item = m_scene->addEllipse(light.x() - 3, light.y() - 3, 6, 6,
                                                     QPen(Qt::black), QBrush(Qt::yellow));
    item->setZValue(2);
}
