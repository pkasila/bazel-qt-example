#include "canvas.h"

Canvas::Canvas(QWidget* p) : QWidget(p) { setMouseTracking(true); setFocusPolicy(Qt::StrongFocus); }
void Canvas::setMode(Mode m) { m_mode = m; update(); }
void Canvas::resizeEvent(QResizeEvent*) { m_controller.UpdateFrame(width(), height()); }
void Canvas::keyPressEvent(QKeyEvent* e) { if (e->key() == Qt::Key_Escape) { m_controller.ClearPolygons(); update(); } }

void Canvas::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), Qt::black);
    QPointF base = m_controller.getLightSource();

    for (int i = 0; i < 10; ++i) {
        QPointF off(base.x() + i * 0.3, base.y() + i * 0.3);
        Polygon area = m_controller.CreateLightArea(off);
        auto v = area.getVertices();
        if (v.size() > 2) {
            painter.setBrush(QColor(255, 255, 180, 20));
            painter.setPen(Qt::NoPen);
            painter.drawPolygon(v.data(), v.size());
        }
    }

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush);
    const auto& polys = m_controller.GetPolygons();
    for (size_t i = 1; i < polys.size(); ++i) {
        auto v = polys[i].getVertices();
        if (v.size() > 1) painter.drawPolygon(v.data(), v.size());
    }
}

void Canvas::mousePressEvent(QMouseEvent* e) {
    if (m_mode == Mode::Polygons) {
        if (e->button() == Qt::LeftButton) {
            if (!m_drawing) { m_controller.AddPolygon(Polygon()); m_drawing = true; }
            m_controller.AddVertexToLastPolygon(e->position());
        } else if (e->button() == Qt::RightButton) m_drawing = false;
    }
    update();
}

void Canvas::mouseMoveEvent(QMouseEvent* e) {
    if (m_mode == Mode::Light) m_controller.setLightSource(e->position());
    else if (m_drawing) m_controller.UpdateLastPolygon(e->position());
    update();
}