#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPainter>

RenderWidget::RenderWidget(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true); 
    std::vector<QPointF> borders = {
        {-10, -10}, {2000, -10}, {2000, 2000}, {-10, 2000}
    };
    m_controller.AddPolygon(Polygon(borders));
}

void RenderWidget::setMode(Mode mode) {
    m_mode = mode;
    m_isDrawingPolygon = false; 
}

void RenderWidget::mousePressEvent(QMouseEvent* event) {
    if (m_mode == Mode::Polygons) {
        if (event->button() == Qt::LeftButton) {
            if (!m_isDrawingPolygon) {
                std::vector<QPointF> start_pts = {event->position(), event->position()};
                m_controller.AddPolygon(Polygon(start_pts));
                m_isDrawingPolygon = true;
            } else {
                m_controller.AddVertexToLastPolygon(event->position());
            }
        } else if (event->button() == Qt::RightButton) {
            if (m_isDrawingPolygon) {
                m_controller.RemoveLastVertexFromLastPolygon();
                m_isDrawingPolygon = false;
            }
        }
    } else if (m_mode == Mode::Light) {
        if (event->button() == Qt::LeftButton) {
            m_controller.AddStaticLight(event->position());
        }
    }
    update();
}

void RenderWidget::mouseMoveEvent(QMouseEvent* event) {
    if (m_mode == Mode::Polygons && m_isDrawingPolygon) {
        m_controller.UpdateLastPolygon(event->position());
        update();
    } else if (m_mode == Mode::Light) {
        m_controller.setLightSource(event->position());
        update();
    }
}

void RenderWidget::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    
    painter.fillRect(rect(), Qt::black); 

    std::vector<Polygon> lightAreas = m_controller.CreateLightAreas();
    painter.setBrush(QColor(255, 255, 100, 100)); 
    painter.setPen(Qt::NoPen);

    for (const auto& area : lightAreas) {
        const auto& light_verts = area.getVertices();
        if (!light_verts.empty()) {
            painter.drawPolygon(light_verts.data(), light_verts.size());
        }
    }

    painter.setPen(QPen(Qt::white, 2));
    painter.setBrush(Qt::NoBrush); 
    
    const auto& polygons = m_controller.GetPolygons();
    for (size_t i = 1; i < polygons.size(); ++i) { 
        const auto& verts = polygons[i].getVertices();
        if (verts.size() > 1) {
            for (size_t j = 0; j < verts.size() - 1; ++j) {
                painter.drawLine(verts[j], verts[j+1]);
            }
            if (i != polygons.size() - 1 || !m_isDrawingPolygon) {
                painter.drawLine(verts.back(), verts.front());
            }
        }
    }

    painter.setBrush(QColor(255, 150, 0));
    painter.setPen(Qt::NoPen);
    for (const auto& staticLight : m_controller.GetStaticLights()) {
        painter.drawEllipse(staticLight, 5, 5);
    }

    painter.setBrush(Qt::yellow);
    painter.drawEllipse(m_controller.getLightSource(), 5, 5);
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget* central = new QWidget(this);
    setCentralWidget(central);
    
    QVBoxLayout* layout = new QVBoxLayout(central);
    layout->setContentsMargins(0, 0, 0, 0); 

    QHBoxLayout* topLayout = new QHBoxLayout();
    
    modeComboBox = new QComboBox(this);
    modeComboBox->addItems({"Режим: Свет (ЛКМ - новый источник)", "Режим: Многоугольники (ЛКМ - точки, ПКМ - завершить)"});
    
    clearButton = new QPushButton("Очистить всё", this);

    topLayout->addWidget(modeComboBox, 1);
    topLayout->addWidget(clearButton);
    
    renderWidget = new RenderWidget(this);
    
    layout->addLayout(topLayout);
    layout->addWidget(renderWidget, 1); 

    connect(modeComboBox, &QComboBox::currentIndexChanged, this, &MainWindow::onModeChanged);
    connect(clearButton, &QPushButton::clicked, this, &MainWindow::onClearClicked);
}

void MainWindow::onClearClicked() {
    renderWidget->clearPolygons();
}

void MainWindow::onModeChanged(int index) {
    if (index == 0) {
        renderWidget->setMode(RenderWidget::Mode::Light);
    } else {
        renderWidget->setMode(RenderWidget::Mode::Polygons);
    }
}

void RenderWidget::clearPolygons() {
    m_controller.ClearPolygons();
    m_isDrawingPolygon = false;
    update();
}