#ifndef CUSTOMGRAPHICSVIEW_H
#define CUSTOMGRAPHICSVIEW_H

#include <QGraphicsView>
#include <QPainter>
#include <vector>
#include <QPointF>

class CustomGraphicsView : public QGraphicsView {
    Q_OBJECT

public:
    CustomGraphicsView(QWidget *parent = nullptr);
    void setPolygons(const std::vector<std::vector<QPointF>>& polygons);
    void setCurrentPolygon(const std::vector<QPointF>& polygon);
    void setLight(const QPointF& pos);
    void setMode(bool isLightMode);

    std::vector<std::vector<QPointF>> polygons_;
    std::vector<QPointF> currentPolygon_;
    std::vector<QPointF> lightArea;
    QPointF lightPos_;
    bool isLightMode_;

signals:
    void sceneMousePressed(QPointF pos, Qt::MouseButton button);
    void sceneMouseMoved(QPointF pos);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;

private:
    void drawPolygon(QPainter &painter, const std::vector<QPointF> &vertices,
                     const QColor &borderColor = Qt::black,
                     const QColor &fillColor = Qt::transparent,
                     bool dashed = false);
};

#endif // CUSTOMGRAPHICSVIEW_H
