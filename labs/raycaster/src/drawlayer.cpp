#include "drawlayer.h"
#include <QDebug>
#include "controller.h"
#include <QComboBox>
#include <QFrame>

std::vector<QPointF> points;
std::vector<Controller*> controllers = {new Controller(QPointF(0, 0), QColor(255, 255, 255, 150)), new Controller(QPointF(0, 0), QColor(255, 255, 255, 150)), new Controller(QPointF(0, 0), QColor(255, 255, 255, 150)), new Controller(QPointF(0, 0), QColor(255, 255, 255, 150)), new Controller(QPointF(0, 0), QColor(255, 255, 255, 150))};
DrawLayer::DrawLayer(QWidget* parent)
    : QWidget(parent)
{
}
void DrawLayer::paintEvent(QPaintEvent* event) {
    QPainter painter(this);
    painter.setPen(QPen(Qt::black, 2));
    painter.drawRect(rect().adjusted(1, 1, -2, -2));
        painter.setBrush(Qt::black);

    for(auto& polygon : controllers.front()->GetPolygons()){
            if(polygon.GetVertices().size() > 1){
                auto vertices = polygon.GetVertices();
                for(int i = 1; i != vertices.size(); ++i){
                    painter.drawPoint(vertices[i]);
                    painter.setPen(QPen(Qt::blue, 2));
                    painter.drawLine(vertices[i - 1], vertices[i]);
                }
                if(polygon.IsComplete()){
                    painter.drawLine(vertices.front(), vertices.back());
                }
            }
        }
    if(mode == "Light"){
        QPainter painter(this);
        painter.setBrush(QColor(255, 255, 255, 100));
        painter.setPen(Qt::NoPen);
        for(auto controller : controllers){
            auto lightZone = controller->CreateLightArea();
            for(int i = 0; i != lightZone.GetVertices().size(); ++i){
                auto p1 = lightZone.GetVertices()[i];
                auto p2 = lightZone.GetVertices()[(i + 1) % lightZone.GetVertices().size()];
                QPointF triangle[3] = {controller->GetSource(), p1, p2};
                painter.drawPolygon(triangle, 3);
            }
        }
        painter.setCompositionMode(QPainter::CompositionMode_SourceOver);
        for(auto controller : controllers){
                painter.setBrush(Qt::red);
            painter.drawEllipse(controller->GetSource(), 5, 5);
        }
    }

}

void DrawLayer::mousePressEvent(QMouseEvent* event){
    if(mode == "Polygons"){
        if(event->button() == Qt::LeftButton){
            if(new_polygon == true){
                const auto clickpos = event->pos();
                    auto pol = new Polygon({});
                    pol->AddVertex(clickpos);
                    for(auto controller : controllers){
                        controller->AddPolygon(*pol);
                    }
                new_polygon = false;
            } else{
                auto clickpos = event->pos();
                for(auto controller : controllers){
                    if(!controller->GetPolygons().empty()){
                        controller->GetPolygons().back().AddVertex(clickpos);
                    }
                }
            }
            update();
        }


        if(event->button() == Qt::RightButton){
            for(auto controller : controllers){
                controller->GetPolygons().back().Complete();
                new_polygon = true;
            }
            update();
        }
    }
}

void DrawLayer::mouseMoveEvent(QMouseEvent* event){
    if(mode == "Light"){
        const auto current_pos = event->pos();
        controllers[0]->SetSource(current_pos);
        controllers[0]->SetSize(this->width(), this->height());
        controllers[1]->SetSource(QPointF(current_pos.x() + 20, current_pos.y()));
        controllers[1]->SetSize(this->width(), this->height());
        controllers[2]->SetSource(QPointF(current_pos.x() - 20, current_pos.y()));
        controllers[2]->SetSize(this->width(), this->height());
        controllers[3]->SetSource(QPointF(current_pos.x(), current_pos.y() + 20));
        controllers[3]->SetSize(this->width(), this->height());
        controllers[4]->SetSource(QPointF(current_pos.x(), current_pos.y() - 20));
        controllers[4]->SetSize(this->width(), this->height());
        update();
    }
}

void DrawLayer::ChangeMode(QString s){
    mode = s;
}


