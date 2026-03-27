#pragma once

#include "Controller.h"

#include <QImage>
#include <QLabel>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QMouseEvent>
#include <QPainter>
#include <QPolygon>
#include <QWidget>
// #include <iostream>
#include <QComboBox>
#include <QGraphicsScene>
#include <QtOpenGLWidgets/QOpenGLWidget>

class MyWidget;
class Area;

class Area : public QOpenGLWidget {
   public:
    explicit Area(
        QWidget* parent = nullptr, QComboBox* mode = nullptr, QLabel* label = nullptr,
        QString* menu = nullptr);
    void CreatePolygon(QMouseEvent* event);

    void DrawPolygons(QPainter* painter);

    void ChangeLight(QMouseEvent* event);

    void CastRay(QPainter* p);

    void SetStaticRadius(int r) {
        static_radius_ = r;
    }

    void SetStaticIntensity(int i) {
        static_intensity_ = i;
    }

    void CalculateStatic();

    [[nodiscard]] int GetStaticRadius() const {
        return static_radius_;
    }

    [[nodiscard]] int GetStaticIntensity() const {
        return static_intensity_;
    }

    //    public:
    // NOLINTNEXTLINE
    // void resizeEvent(QResizeEvent* event) override {
    //     this->repaint();
    // }

    void mouseMoveEvent(QMouseEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    // NOLINTNEXTLINE
    void paintEvent(QPaintEvent* e) override;

   private:
    Controller* controller_;
    bool flag_ = false;
    QImage image_;
    QImage light_image_;
    QComboBox* mode_;
    QLabel* label_;
    QString* menu_;
    QImage static_image_;
    int static_radius_ = 100;
    int static_intensity_ = 0;
};

// NOLINTNEXTLINE
class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    explicit MainWindow(QWidget* parent = nullptr);

    ~MainWindow() override = default;

    // NOLINTNEXTLINE

    void Action1() {
        *menu_ = "with images";
        image_->repaint();
    }

    void Action2() {
        *menu_ = "without images";
        image_->repaint();
    }

   protected:
    // void mouseMoveEvent(QMouseEvent* event) override {
    //     image_->mouseMoveEvent(event);
    // }

    // void mousePressEvent(QMouseEvent* event) override {
    //     image_->mousePressEvent(event);
    // }

    void contextMenuEvent(QContextMenuEvent* event) override;

    // NOLINTNEXTLINE
    void resizeEvent(QResizeEvent* event) override {
        image_->repaint();
    }

    // NOLINTNEXTLINE
    void paintEvent(QPaintEvent* e) override;

   private:
    // Controller* controller_;
    // bool flag_ = false;
    QLabel* label_;
    QComboBox* mode_;
    QMenu* menu_bar_;
    QString* menu_;
    Area* image_;
    // QImage* image_;
};
