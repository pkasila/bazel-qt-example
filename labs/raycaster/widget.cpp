#include "widget.h"
#include <QPen>
#include <QColor>
#include <QResizeEvent>

DrawingArea::DrawingArea(QWidget* parent) : QWidget(parent) {
    setMinimumSize(800, 600);
    setMouseTracking(true);
}

DrawingArea::~DrawingArea() = default;

void DrawingArea::resizeEvent(QResizeEvent* event) {
    QWidget::resizeEvent(event);
    if (controller_) controller_->SetBounds(QRectF(0, 0, width(), height()));
}

void DrawingArea::paintEvent(QPaintEvent*) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.fillRect(rect(), QColor(20, 20, 20));
    
    if (!controller_) return;
    drawPolygons(painter);
    if (current_mode_ == Mode::Light) drawLight(painter);
}

void DrawingArea::drawPolygons(QPainter& painter) {
    const auto& polys = controller_->GetPolygons();
    if (polys.empty()) return;
    
    painter.setPen(QPen(QColor(60, 60, 60), 2));
    painter.setBrush(Qt::NoBrush);
    const auto& border = polys[0].getVertices();
    for (size_t i = 0; i < border.size(); ++i)
        painter.drawLine(border[i], border[(i + 1) % border.size()]);
    
    QPen poly_pen(QColor(255, 100, 100), 2);
    painter.setPen(poly_pen);
    for (size_t i = 1; i < polys.size(); ++i) {
        const auto& verts = polys[i].getVertices();
        for (size_t j = 0; j < verts.size(); ++j)
            painter.drawLine(verts[j], verts[(j + 1) % verts.size()]);
        painter.setBrush(QColor(255, 100, 100));
        for (const auto& v : verts) painter.drawEllipse(v, 3, 3);
        painter.setBrush(Qt::NoBrush);
    }
}

void DrawingArea::drawLight(QPainter& painter) {
    for (const auto& poly : controller_->CreateLightAreas()) {
        const auto& verts = poly.getVertices();
        if (verts.size() < 3) continue;
        
        QPainterPath path;
        path.moveTo(verts[0]);
        for (size_t i = 1; i < verts.size(); ++i) path.lineTo(verts[i]);
        path.closeSubpath();
        
        painter.setBrush(QColor(255, 255, 50, 50));  // alpha=50 для полутени
        painter.setPen(Qt::NoPen);
        painter.drawPath(path);
    }
    
    painter.setBrush(QColor(255, 255, 0));
    painter.setPen(Qt::NoPen);
    for (const auto& light : controller_->GetLightSources()) {
        painter.drawEllipse(light, 5, 5);
    }
}

void DrawingArea::mousePressEvent(QMouseEvent* event) {
    if (current_mode_ == Mode::Polygons && event->button() == Qt::LeftButton) {
        if (!building_polygon_) {
            controller_->AddPolygon(Polygon());
            building_polygon_ = true;
        }
        controller_->AddVertexToLastPolygon(event->pos());
        update();
    } else if (current_mode_ == Mode::Polygons && event->button() == Qt::RightButton) {
        building_polygon_ = false;
    }
}

void DrawingArea::mouseMoveEvent(QMouseEvent* event) {
    if (current_mode_ == Mode::Light) {
        controller_->SetLightCluster(event->position());
        update();
    } else if (current_mode_ == Mode::Polygons && building_polygon_) {
        controller_->UpdateLastPolygon(event->pos());
        update();
    }
}

RaycasterWidget::RaycasterWidget(QWidget* parent) : QWidget(parent) {
    setMinimumSize(800, 650);
    initUI();
}

RaycasterWidget::~RaycasterWidget() = default;

void RaycasterWidget::initUI() {
    layout_ = new QVBoxLayout(this);
    layout_->setSpacing(0);
    layout_->setContentsMargins(0, 0, 0, 0);
    
    mode_combo_ = new QComboBox();
    mode_combo_->addItem("Polygons Mode");
    mode_combo_->addItem("Light Mode");
    mode_combo_->setMaximumHeight(30);
    
    connect(mode_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            [this](int idx) { switchMode(idx == 0 ? Mode::Polygons : Mode::Light); });
    
    drawing_area_ = new DrawingArea();
    drawing_area_->setController(&controller_);
    
    layout_->addWidget(mode_combo_);
    layout_->addWidget(drawing_area_, 1);
    
    controller_.SetBounds(QRectF(0, 0, 800, 600));
    controller_.SetLightCluster(QPointF(400, 300));
}

void RaycasterWidget::switchMode(Mode mode) {
    drawing_area_->setMode(mode);
}