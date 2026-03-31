#include "mainwindow.h"
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QPalette>
#include <QDebug>
#include <cmath>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
    m_mainGraphicsScene(new QGraphicsScene(this)),
    m_mainGraphicsView(new QGraphicsView(m_mainGraphicsScene, this)),
    m_applicationModeComboBox(new QComboBox(this)),
    m_logicController(new Controller(m_sceneWidthValue, m_sceneHeightValue)),
    m_isCurrentlyDrawingPolygon(false),
    m_currentApplicationMode(0)
{
    setupUserInterface();
    int top_ui_elements_height = m_applicationModeComboBox->sizeHint().height() +
                                 (layout() ? layout()->contentsMargins().top() + layout()->contentsMargins().bottom() : 0) + 20;
    setFixedSize(m_sceneWidthValue + 20, m_sceneHeightValue + top_ui_elements_height);
    setWindowFlag(Qt::WindowMaximizeButtonHint, false);
    updateControllerLightSourcePositions(QPointF(m_sceneWidthValue / 2.0, m_sceneHeightValue / 2.0));
}

MainWindow::~MainWindow()
{
    delete m_logicController;
}

void MainWindow::setupUserInterface()
{
    QWidget* central_widget = new QWidget(this);
    QVBoxLayout* root_layout = new QVBoxLayout(central_widget);

    m_applicationModeComboBox->addItem("Режим: Свет");
    m_applicationModeComboBox->addItem("Режим: Полигоны");
    connect(m_applicationModeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onModeChanged);

    m_mainGraphicsView->setRenderHint(QPainter::Antialiasing);
    m_mainGraphicsView->setSceneRect(0, 0, m_sceneWidthValue, m_sceneHeightValue);
    m_mainGraphicsView->setBackgroundBrush(QPalette().color(QPalette::Base));
    m_mainGraphicsView->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_mainGraphicsView->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    if (m_mainGraphicsView->viewport()) {
        m_mainGraphicsView->viewport()->installEventFilter(this);
    }

    root_layout->addWidget(m_applicationModeComboBox);
    root_layout->addWidget(m_mainGraphicsView);
    setCentralWidget(central_widget);

    if (m_mainGraphicsView->viewport()) {
        QCursor::setPos(m_mainGraphicsView->mapToGlobal(QPoint(m_sceneWidthValue / 2, m_sceneHeightValue / 2)));
    }
    redrawSceneContent();
}

void MainWindow::onModeChanged(int index)
{
    m_currentApplicationMode = index;
    m_isCurrentlyDrawingPolygon = false;
    redrawSceneContent();
}

bool MainWindow::eventFilter(QObject* watched_object, QEvent* event_obj)
{
    if (watched_object == m_mainGraphicsView->viewport()) {
        if (event_obj->type() == QEvent::MouseButtonPress) {
            handleViewportMousePress(static_cast<QMouseEvent*>(event_obj));
            return true;
        } else if (event_obj->type() == QEvent::MouseMove) {
            handleViewportMouseMove(static_cast<QMouseEvent*>(event_obj));
            return true;
        }
    }
    return QMainWindow::eventFilter(watched_object, event_obj);
}

void MainWindow::handleViewportMousePress(QMouseEvent* mouse_event)
{
    QPointF scene_click_pos = m_mainGraphicsView->mapToScene(mouse_event->pos());
    if (m_currentApplicationMode == 1) {
        if (mouse_event->button() == Qt::LeftButton) {
            if (!m_isCurrentlyDrawingPolygon) {
                m_logicController->addPolygon(Polygon());
                m_logicController->addVertexToLastPolygon(scene_click_pos);
                m_logicController->addVertexToLastPolygon(scene_click_pos);
                m_isCurrentlyDrawingPolygon = true;
            } else {
                m_logicController->updateLastPolygon(scene_click_pos);
                m_logicController->addVertexToLastPolygon(scene_click_pos);
            }
        } else if (mouse_event->button() == Qt::RightButton && m_isCurrentlyDrawingPolygon) {
            m_isCurrentlyDrawingPolygon = false;
        }
    }
    redrawSceneContent();
}

void MainWindow::handleViewportMouseMove(QMouseEvent* mouse_event)
{
    QPointF scene_cursor_pos = m_mainGraphicsView->mapToScene(mouse_event->pos());
    if (m_currentApplicationMode == 0) {
        updateControllerLightSourcePositions(scene_cursor_pos);
        redrawSceneContent();
    } else if (m_currentApplicationMode == 1 && m_isCurrentlyDrawingPolygon) {
        m_logicController->updateLastPolygon(scene_cursor_pos);
        redrawSceneContent();
    }
}

void MainWindow::redrawSceneContent()
{
    m_renderedLightAreaItems.clear();
    m_renderedLightSourceMarkerItems.clear();
    m_mainGraphicsScene->clear();

    drawSceneAABB();
    drawObstaclePolygons();

    if (m_currentApplicationMode == 0) {
        drawLightSourceMarkers();
        drawLightAreaVisuals();
    }
}

void MainWindow::drawLightAreaVisuals() {
    std::vector<Polygon> light_polys = m_logicController->calculateAllLightAreasForMultipleSources();
    for (const auto& area_poly_data : light_polys) {
        const auto& verts = area_poly_data.getVertices();
        if (verts.size() < 3) continue;
        QPolygonF q_poly;
        for (const auto& v_pt : verts) q_poly << v_pt;
        QGraphicsPolygonItem* g_item = m_mainGraphicsScene->addPolygon(q_poly, QPen(Qt::NoPen), QBrush(m_visualStyle.lightPolygonFillColor));
        if (g_item) {
            g_item->setZValue(1);
            m_renderedLightAreaItems.push_back(g_item);
        }
    }
}

void MainWindow::updateControllerLightSourcePositions(const QPointF& main_cursor_pos) {
    std::vector<QPointF> new_sources;
    new_sources.push_back(main_cursor_pos);

    for (int i = 0; i < m_visualStyle.numberOfSecondaryLights; ++i) {
        double angle_rad = 2 * M_PI * i / m_visualStyle.numberOfSecondaryLights;
        QPointF offset_vec(m_visualStyle.secondaryLightSpreadRadius * std::cos(angle_rad),
                           m_visualStyle.secondaryLightSpreadRadius * std::sin(angle_rad));
        new_sources.push_back(main_cursor_pos + offset_vec);
    }
    m_logicController->setLightSources(new_sources);
    m_logicController->setLightSource(main_cursor_pos);
}

void MainWindow::drawLightSourceMarkers() {
    const auto& current_sources = m_logicController->getLightSources();
    for (const auto& src_pos : current_sources) {
        QGraphicsEllipseItem* marker_item = m_mainGraphicsScene->addEllipse(
            src_pos.x() - m_visualStyle.lightMarkerRadius, src_pos.y() - m_visualStyle.lightMarkerRadius,
            m_visualStyle.lightMarkerRadius * 2, m_visualStyle.lightMarkerRadius * 2,
            QPen(Qt::black), QBrush(Qt::yellow));
        if (marker_item) {
            marker_item->setZValue(2);
            m_renderedLightSourceMarkerItems.push_back(marker_item);
        }
    }
}

void MainWindow::drawObstaclePolygons() {
    for (const auto& poly_data : m_logicController->getPolygons()) {
        const auto& verts = poly_data.getVertices();
        if (verts.empty()) continue;
        QPolygonF q_poly;
        for (const auto& v_pt : verts) q_poly << v_pt;

        if (m_isCurrentlyDrawingPolygon && !m_logicController->getPolygons().empty() && &poly_data == &m_logicController->getPolygons().back()) {
            if (verts.size() >= 1) {
                m_mainGraphicsScene->addPolygon(q_poly, QPen(Qt::blue, 1.5), QBrush(Qt::NoBrush))->setZValue(0);
            }
            for(const auto& v_pt : verts) {
                m_mainGraphicsScene->addEllipse(v_pt.x()-2.5, v_pt.y()-2.5, 5, 5, QPen(Qt::black), QBrush(Qt::blue))->setZValue(3);
            }
        } else if (verts.size() >= 3) {
            m_mainGraphicsScene->addPolygon(q_poly, QPen(Qt::black), QBrush(Qt::gray))->setZValue(0);
        } else if (verts.size() == 2) {
            m_mainGraphicsScene->addLine(QLineF(verts[0], verts[1]), QPen(Qt::black, 2))->setZValue(0);
        }
    }
}

void MainWindow::drawSceneAABB() {
    QPolygonF box_poly_qf;
    const auto& bbox_vertices_std_vec = m_logicController->getBoundingBoxVertices();
    if (bbox_vertices_std_vec.size() < 3) return;
    for(const QPointF& v_qpointf : bbox_vertices_std_vec) {
        box_poly_qf << v_qpointf;
    }

    QGraphicsPolygonItem* box_item = m_mainGraphicsScene->addPolygon(
        box_poly_qf, QPen(Qt::DashLine), QBrush(Qt::NoBrush) );
    if(box_item) box_item->setZValue(-1);
    qreal vertex_marker_radius = 2.5;
    QColor vertex_marker_color = Qt::red;
    QPen vertex_marker_pen(Qt::black, 0.5);
    QBrush vertex_marker_brush(vertex_marker_color);

    for(const QPointF& vertex_qpointf : bbox_vertices_std_vec) {
        QGraphicsEllipseItem* vertex_marker_item = m_mainGraphicsScene->addEllipse(
            vertex_qpointf.x() - vertex_marker_radius,
            vertex_qpointf.y() - vertex_marker_radius,
            vertex_marker_radius * 2,
            vertex_marker_radius * 2,
            vertex_marker_pen,
            vertex_marker_brush
            );
        if(vertex_marker_item) vertex_marker_item->setZValue(5);
    }
}

