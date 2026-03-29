//
// Created by Zazerg on 3/31/25.
//

#include "RenderArea.h"

#include <QPainter>
#include <QMouseEvent>
#include "Controller.h"
#include "MainWindow.h"
#include <QPainterPath>

RenderArea::RenderArea(Settings& settings, QWidget* parent) : settings_(settings) {
    setMaximumSize(settings_.bgSize);
    setParent(parent);
    controller_ = {};
    controller_.addPolygon();
    controller_.addLightSource({QColor(120,120,120), QColor(60,60,60), 0, {10,10}, settings_.bg.size()});
}

void RenderArea::paintEvent(QPaintEvent* event) {
    // QPainter painter2(this);
    QRectF screenRect = QRectF(topleft_, scale_ * QSize(width(),height()));
    QRectF bgRect = QRectF({0,0},settings_.bgSize);

    QImage screenImg = QImage(QSize(width(), height()), QImage::Format_ARGB32_Premultiplied);
    QPainter screen(&screenImg);
    screen.setRenderHint(QPainter::Antialiasing, settings_.enable_aa_);
    screen.setRenderHint(QPainter::SmoothPixmapTransform, true);
#ifdef USE_OPENGL
    screen.drawPixmap(QRect(0,0,width(),height()), settings_.bg, screenRect);
#else
    screen.fillRect(screenImg.rect(), Qt::white);
#endif

    QImage shadowImg = QImage(settings_.bgSize, QImage::Format_ARGB32_Premultiplied);
    shadowImg.fill(QColor(20,20,20));
    QPainter shadow(&shadowImg);
    shadow.setCompositionMode(QPainter::CompositionMode_Plus);

    for (auto& ls : controller_.getLightSources()) {
        QImage& newShadowImg = ls.getImage();
        if (!ls.updated() || force_update_) {
            newShadowImg.fill(Qt::black);
            QPainter newShadow(&newShadowImg);
            newShadow.setRenderHint(QPainter::Antialiasing, settings_.enable_aa_);
            newShadow.setPen(Qt::NoPen);

            if (ls.getRadius()) {
                QGradient grad = QRadialGradient(ls.getPosition(), qreal(ls.getRadius()), ls.getPosition());
                grad.setColorAt(0, ls.getColor2());
                grad.setColorAt(1, Qt::black);
                newShadow.setBrush(grad);
            } else {
                newShadow.setBrush(ls.getColor2());
            }
            for (auto& p : controller_.createHalfShades(ls.getPosition(), settings_.bgSize.width(), settings_.bgSize.height())) {
                drawPolygon(newShadow, p);
            }
            if (ls.getRadius()) {
                QGradient grad = QRadialGradient(ls.getPosition(), qreal(ls.getRadius()), ls.getPosition());
                grad.setColorAt(0, ls.getColor1());
                grad.setColorAt(1, Qt::black);
                newShadow.setBrush(grad);
            } else {
                newShadow.setBrush(ls.getColor1());
            }
            drawPolygon(newShadow, controller_.fromLightSource(ls.getPosition(), settings_.bgSize.width(), settings_.bgSize.height()));
            ls.setUpdated(true);
        }
        shadow.drawImage(bgRect, newShadowImg, bgRect);
    }
    force_update_ = false;

    screen.setBrush(Qt::NoBrush);
    screen.setPen(Qt::white);
    screen.scale(1/scale_, 1/scale_);
    screen.translate(-topleft_);
    for (auto& poly : controller_.getPolygons()) {
        if (poly.length() < 2) continue;
        drawPolygon(screen, poly);
    }

    screen.resetTransform();
    screen.setCompositionMode(QPainter::CompositionMode_Multiply);
    screen.drawImage(QRect(0,0,width(),height()), shadowImg, screenRect);

    screen.scale(1/scale_, 1/scale_);
    screen.translate(-topleft_);
    screen.setCompositionMode(QPainter::CompositionMode_SourceIn);
    screen.setPen(Qt::black);
    for (auto ls : controller_.getLightSources()) {
        screen.setBrush(ls.getColor1());
        screen.drawEllipse(ls.getPosition(), 5, 5);
    }

    QPainter render = QPainter(this);
    render.drawImage(QRect(0,0,width(),height()), screenImg);
}

void RenderArea::mousePressEvent(QMouseEvent* event) {
    switch (settings_.mode) {
        case Mode::DRAW:
            if (event->button() == Qt::LeftButton) {
                controller_.addVertexToLastPolygon(event->pos() * scale_ + topleft_);
                force_update_ = true;
            }
            else if (event->button() == Qt::RightButton) {
                controller_.addPolygon();
            }
            break;
        case Mode::LIGHT:
            controller_.setLightSource(event->pos() * scale_ + topleft_, settings_.bgSize.width(), settings_.bgSize.height());
            break;
        case Mode::STATIC_LIGHT:
            controller_.addLightSource(LightSource(settings_.color1, settings_.color2, settings_.radius, event->pos() * scale_ + topleft_, settings_.bg.size()));
            break;
        case Mode::MOVE_SCREEN:
            moving_ = true;
            add_ = topleft_ + event->pos() * scale_;
            break;
    }
    repaint();
}

void RenderArea::mouseMoveEvent(QMouseEvent* event) {
    switch (settings_.mode) {
        case Mode::DRAW:
            break;
        case Mode::LIGHT:
            controller_.setLightSource(event->pos() * scale_ + topleft_, settings_.bgSize.width(), settings_.bgSize.height());
            break;
        case Mode::STATIC_LIGHT:
            break;
        case Mode::MOVE_SCREEN:
            if (moving_) {
                topleft_ = add_ - event->pos() * scale_;
                if (topleft_.x() < 0) {
                    topleft_.setX(0);
                }
                if (topleft_.y() < 0) {
                    topleft_.setY(0);
                }
                if (topleft_.x() + width() * scale_ > settings_.bgSize.width()) {
                    topleft_.setX(settings_.bgSize.width() - width() * scale_);
                }
                if (topleft_.y() + height() * scale_ > settings_.bgSize.height()) {
                    topleft_.setY(settings_.bgSize.height() - height() * scale_);
                }
            }
    }
    repaint();
}

void RenderArea::mouseReleaseEvent(QMouseEvent* event) {
    switch (settings_.mode) {
        case Mode::DRAW:
            break;
        case Mode::LIGHT:
            break;
        case Mode::STATIC_LIGHT:
            break;
        case Mode::MOVE_SCREEN:
            if (moving_) {
                moving_ = false;
                topleft_ = add_ - event->pos() * scale_;
                if (topleft_.x() < 0) {
                    topleft_.setX(0);
                }
                if (topleft_.y() < 0) {
                    topleft_.setY(0);
                }
                if (topleft_.x() + width() * scale_> settings_.bgSize.width()) {
                    topleft_.setX(settings_.bgSize.width() - width() * scale_);
                }
                if (topleft_.y() + height() * scale_ > settings_.bgSize.height()) {
                    topleft_.setY(settings_.bgSize.height() - height() * scale_);
                }
            }
            break;
    }
}

void RenderArea::resizeEvent(QResizeEvent* event) {
#ifdef USE_OPENGL
    QOpenGLWidget::resizeEvent(event);
#endif
    if (topleft_.x() < 0) {
        topleft_.setX(0);
    }
    if (topleft_.y() < 0) {
        topleft_.setY(0);
    }
    if (topleft_.x() + width() * scale_ > settings_.bgSize.width()) {
        topleft_.setX(settings_.bgSize.width() - width() * scale_);
    }
    if (topleft_.y() + height() * scale_ > settings_.bgSize.height()) {
        topleft_.setY(settings_.bgSize.height() - height() * scale_);
    }
    repaint();
}

void RenderArea::wheelEvent(QWheelEvent* event) {
    topleft_ = (topleft_ + event->position() * scale_).toPoint();
    scale_ += ((double)event->angleDelta().y()) / 1000;
    if (scale_ < .01) {
        scale_ = .01;
    }
    topleft_ = ((topleft_) - event->position() * scale_).toPoint();
    if (width() * scale_ > settings_.bgSize.width()) {
        scale_ = (((double) settings_.bgSize.width()) / width());
    }
    if (height() * scale_ > settings_.bgSize.height()) {
        scale_ = ((double) settings_.bgSize.height()) / height();
    }
    if (topleft_.x() < 0) {
        topleft_.setX(0);
    }
    if (topleft_.y() < 0) {
        topleft_.setY(0);
    }
    if (topleft_.x() + width() * scale_ > settings_.bgSize.width()) {
        topleft_.setX(settings_.bgSize.width() - width() * scale_);
    }
    if (topleft_.y() + height() * scale_ > settings_.bgSize.height()) {
        topleft_.setY(settings_.bgSize.height() - height() * scale_);
    }
    repaint();
}


void RenderArea::drawPolygon(QPainter& painter, const Polygon& poly) {
    painter.drawPolygon(poly.getVertices().data(), poly.getVertices().size());
}

Controller& RenderArea::getController() {
    return controller_;
}