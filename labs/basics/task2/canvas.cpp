#include "canvas.h"
#include <QPainter>
#include <QDebug>
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QPoint>


Canvas::Canvas(QWidget *parent) : QWidget(parent) {
    this->parent = parent;
    tool = Tool::Pencil;
    penWidth = 10;
    penColor = Qt::black;
}

void Canvas::setTool(Tool newTool) {
    tool = newTool;
}

void Canvas::setColor(QColor newColor) {
    penColor = newColor;
}

void Canvas::setPenWidth(int newWidth) {
    penWidth = newWidth;
}

bool Canvas::openImage(const QString &fileName) {
    QImage loadedImage;
    if (!loadedImage.load(fileName))
        return false;
    image = loadedImage;
    //parent->resize(image.width()+10, image.height()+10);
    //resize(image.size());

    update();
    return true;
}

bool Canvas::saveImage(const QString &fileName) {
    return image.save(fileName);
}

Canvas::~Canvas() {
}

void Canvas::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        drawPointIn(event->pos());
        drawing = true;
    }
}

void Canvas::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton) && drawing) {
        drawLineTo(QPoint(event->pos()));
    }
}

void Canvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton && drawing) {
        drawLineTo(QPoint(event->pos()));
        drawing = false;
    }
}

void Canvas::paintEvent(QPaintEvent *event) {
    QPainter painter(this);
    painter.drawImage(0, 0, image);
}

void Canvas::resizeEvent(QResizeEvent *event) {
    resizeImage(size());
    QWidget::resizeEvent(event);
}

void Canvas::drawLineTo(const QPoint &endPoint) {
    QPainter painter(&image);
    painter.setPen(QPen(QBrush(penColor), penWidth, Qt::SolidLine, Qt::RoundCap));
    painter.setRenderHint(QPainter::Antialiasing, (tool == Tool::Brush));

    painter.drawLine(lastPoint, endPoint);
    lastPoint = endPoint;
    update();
}

void Canvas::drawPointIn(const QPoint &point) {
    QPainter painter(&image);

    painter.setPen(QPen(QBrush(penColor), penWidth, Qt::SolidLine, Qt::RoundCap));
    painter.setRenderHint(QPainter::Antialiasing, (tool == Tool::Brush));

    painter.drawPoint(point);
    lastPoint = point;
    update();
}

void Canvas::resizeImage(QSize newSize) {
    QImage newImage(newSize, QImage::Format_RGB32);
    newImage.fill(Qt::white);
    QPainter painter(&newImage);
    painter.drawImage(0, 0, image);
    image = std::move(newImage);
}


