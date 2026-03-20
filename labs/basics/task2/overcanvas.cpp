#include "overcanvas.h"
#include "canvas.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

OverCanvas::OverCanvas(QWidget *parent) : QWidget(parent){
    resize(200, 200);
    setGeometry(0, 100, 200, 200);
    itsParent = parent;
    setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);

    canvas = new Canvas(this);
    canvas->resize(400, 400);

    bottomButton = new ColoredWidget(Qt::black, this);
    rightmButton = new ColoredWidget(Qt::black, this);

    QHBoxLayout *vButtonLayout = new QHBoxLayout;
    //vButtonLayout->setContentsMargins(0, 0, 0, 0);
    vButtonLayout->addStretch(1);
    vButtonLayout->addWidget(bottomButton);
    vButtonLayout->addStretch(1);

    QVBoxLayout *hButtonLayout = new QVBoxLayout;
    //hButtonLayout->setContentsMargins(0, 0, 0, 0);
    hButtonLayout->addStretch(1);
    hButtonLayout->addWidget(rightmButton);
    hButtonLayout->addStretch(1);

    QVBoxLayout *vCanvasLayout = new QVBoxLayout;
    //vCanvasLayout->setContentsMargins(0, 0, 0, 0);
    vCanvasLayout->addWidget(canvas);
    vCanvasLayout->addLayout(vButtonLayout);

    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    //vCanvasLayout->setContentsMargins(1, 1, 0, 0);
    mainLayout->addLayout(vCanvasLayout);
    mainLayout->addLayout(hButtonLayout);
}

bool OverCanvas::openImage(const QString& fileName) {
    return canvas->openImage(fileName);
}

bool OverCanvas::saveImage(const QString& fileName) {
    return canvas->saveImage(fileName);
}

Canvas* OverCanvas::getCanvas() {
    return canvas;
}

void OverCanvas::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (bottomButton->geometry().contains(event->pos())) {
            resizeY = true;
        } else if(rightmButton->geometry().contains(event->pos())) {
            resizeX = true;
        }
        lastPoint = event->pos();
    }
}

void OverCanvas::mouseMoveEvent(QMouseEvent *event) {
    if ((event->buttons() & Qt::LeftButton)) {
        if (resizeY) {
            int dy = event->pos().y() - lastPoint.y();
            if (canvas->height() + dy > 0 && itsParent->height() > height() + y() + dy + 15) {
                this->setFixedHeight(this->height() + dy);
            }
        } else if(resizeX) {
            int dx = event->pos().x() - lastPoint.x();
            if (canvas->width() + dx > 0 && itsParent->width() > width() + x() + dx) {
                this->setFixedWidth(this->width() + dx);
            }
        }
        lastPoint = event->pos();
    }
}

void OverCanvas::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        if (resizeY) {
            int dy = event->pos().y() - lastPoint.y();
            if (canvas->height() + dy > 0 && itsParent->height() > height() + y() + dy) {
                this->setFixedHeight(this->height() + dy);
            }
        } else if(resizeX) {
            int dx = event->pos().x() - lastPoint.x();
            if (canvas->width() + dx > 0 && itsParent->width() > width() + x() + dx) {
                this->setFixedWidth(this->width() + dx);
            }
        }
        resizeY = false;
        resizeX = false;
    }
}

OverCanvas::~OverCanvas() {
}


