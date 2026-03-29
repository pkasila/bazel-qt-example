#include "board.h"
#include "ray.h"
#include <numbers>

board::board(QWidget* parent) : QWidget(parent) {
    setMouseTracking(true);
    setFocusPolicy(Qt::StrongFocus);
    ctrl_.Mode() = 0;
    
}

void board::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);
    QRect rect = this->rect();
    painter.fillRect(rect, Qt::black);
    if (ctrl_.GetPolygons().empty()){
        ctrl_.AddPolygon(Polygon({rect.topLeft(), rect.topRight(),
            rect.bottomRight(), rect.bottomLeft()}));
        ctrl_.AddPolygon(Polygon({}));
    }

    QPoint ppp = mapFromGlobal(QCursor::pos());

    // лучи
    // if (ctrl_.Mode() == 0){
    //     painter.setPen(QPen(QColor("#FF80A0"), 2)); 
    //     std::vector<Ray> vr = ctrl_.CastRays();
    //     ctrl_.IntersectRays(&vr);
    //     ctrl_.RemoveAdjacentRays(&vr);
    //     for (const auto & i : vr){
    //         painter.drawLine(i.GetBegin(),i.GetEnd());
    //     }

    // }

    int c=8;

    QPointF mainlight = ctrl_.GetLight();
    std::vector<QPointF> lights;
    lights.push_back(mainlight);

    lights.push_back({mainlight.x()+c,mainlight.y()+c});
    lights.push_back({mainlight.x()+c,mainlight.y()-c});
    lights.push_back({mainlight.x()-c,mainlight.y()+c});
    lights.push_back({mainlight.x()-c,mainlight.y()-c});

    lights.push_back({mainlight.x()+2*c,mainlight.y()});
    lights.push_back({mainlight.x()-2*c,mainlight.y()});
    lights.push_back({mainlight.x(),mainlight.y()+2*c});
    lights.push_back({mainlight.x(),mainlight.y()-2*c});

    
    //свет static lights
    {
        painter.setBrush(QColor("#30d883e6"));
        painter.setPen(Qt::NoPen);
        auto sl = ctrl_.GetStaticLights();
        for (auto lt : sl){
            ctrl_.SetLight(lt);
            if (lt.x() > 1 && lt.y() > 1 && lt.x() + 1 < width() && lt.y() + 1 < height()){
                Polygon p = ctrl_.CreateLightArea();
                const std::vector<QPointF> vv = p.Get();
                painter.drawPolygon(vv.data(), vv.size(), Qt::OddEvenFill);
            }
            
        }
        ctrl_.SetLight(mainlight);
    }

    // свет
    if (ctrl_.Mode() == 0){
        painter.setBrush(QColor("#2F8092B4"));
        painter.setPen(Qt::NoPen);
        for (auto lt : lights){
            ctrl_.SetLight(lt);
            if (lt.x() > 1 && lt.y() > 1 && lt.x() + 1 < width() && lt.y() + 1 < height()){
                Polygon p = ctrl_.CreateLightArea();
                const std::vector<QPointF> vv = p.Get();
                painter.drawPolygon(vv.data(), vv.size(), Qt::OddEvenFill);
            }
            
        }
        ctrl_.SetLight(mainlight);
    }

    // многоугольники
    painter.setPen(QPen(Qt::red, 2)); 
    std::vector<Polygon> vp = ctrl_.GetPolygons();
    for (int j=0; j<vp.size();j++){
        std::vector<QPointF> vts = vp[j].Get();
        for (int i=0; (i+1)<vts.size(); i++){
            painter.drawLine(vts[i],vts[i+1]);
        }
        if (vts.size()>=1 && j != vp.size()-1){
            painter.drawLine(vts[vts.size()-1],vts[0]);
        }   
        if (vts.size()>=1 && j == vp.size()-1 && this->rect().contains(ppp)){
            painter.drawLine(vts[vts.size()-1], ppp);
        }
    }

    // источник света
    if (ctrl_.Mode() == 0){
        painter.setBrush(Qt::red);
        painter.setPen(Qt::NoPen);
        int r = 5;
        for (auto lt : lights){
            if (lt.x() > 1 && lt.y() > 1 && lt.x() + 1 < width() && lt.y() + 1 < height()){
                painter.drawEllipse(lt, r, r);
            }
        }
    }

    // static lights
    {
        painter.setBrush(Qt::green);
        painter.setPen(Qt::NoPen);
        int r = 5;
        auto sl = ctrl_.GetStaticLights();
        for (auto lt : sl){
            if (lt.x() > 1 && lt.y() > 1 && lt.x() + 1 < width() && lt.y() + 1 < height()){
                painter.drawEllipse(lt, r, r);
            }
        }
    }
    
}

void board::mouseMoveEvent(QMouseEvent* event) {
    if (ctrl_.Mode() == 0) {
        ctrl_.SetLight(event->pos());
    }
    if (ctrl_.Mode() == 1 || (ctrl_.GetLight().x() > 1 && ctrl_.GetLight().y() > 1 && ctrl_.GetLight().x() + 1 < width() && ctrl_.GetLight().y() + 1 < height())){
        update();
    }
}

void board::mousePressEvent(QMouseEvent* event) {
    if (ctrl_.Mode() == 1) {
        if (event->button() == Qt::LeftButton) {
            ctrl_.AddVertexToLastPolygon(event->pos());
        }else if (event->button() == Qt::RightButton) {
            std::vector<Polygon> vp = ctrl_.GetPolygons();
            if (vp[vp.size()-1].Get().size()>0){
                QPointF pf = vp[vp.size()-1].Get()[0];
                ctrl_.AddVertexToLastPolygon(pf);
                //std::cout << ctrl_.GetPolygons().back().Get().size() << std::endl;
                ctrl_.AddPolygon(Polygon({}));
            }
        }
    }
    if (ctrl_.Mode() == 2){
        if (event->button() == Qt::LeftButton) {
            ctrl_.AddStaticLight(event->pos());
        }else if (event->button() == Qt::RightButton) {
            std::vector<QPointF>& sl = ctrl_.GetStaticLights();
            std::vector<QPointF> ans; ans.clear();
            int c = 7;
            for (auto l : sl){
                if (abs(event->pos().x() - l.x()) > c || abs(event->pos().y() - l.y()) > c){
                    ans.push_back(l);
                }
            }
            sl = ans;
        }
    }
    update();
}


void board::resizeEvent(QResizeEvent* event){
    QRect rect = this->rect();
    if (ctrl_.GetPolygons().empty()){
        return;
    }
    if (!ctrl_.GetPolygons().back().Get().empty()){
        std::vector<Polygon> vp = ctrl_.GetPolygons();
        if (vp[vp.size()-1].Get().size()>0){
            QPointF pf = vp[vp.size()-1].Get()[0];
            ctrl_.AddVertexToLastPolygon(pf);
            ctrl_.AddPolygon(Polygon({}));
        }
    }

    ctrl_.AddVertexToLastPolygon(rect.topLeft());
    ctrl_.AddVertexToLastPolygon(rect.topRight());
    ctrl_.AddVertexToLastPolygon(rect.bottomRight());
    ctrl_.AddVertexToLastPolygon(rect.bottomLeft());
    ctrl_.AddVertexToLastPolygon(rect.topLeft());
    
    ctrl_.GetPolygons()[0] = ctrl_.GetPolygons().back();
    ctrl_.GetPolygons().back() = Polygon({}); 
    update();
    
}