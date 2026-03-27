
#include "Raycaster.h"

#include "Polygon.h"

#include <QDialog>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainterPath>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

// NOLINTNEXTLINE
QImage back;
// NOLINTNEXTLINE
QImage light;

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , label_(new QLabel(this))
    , mode_(new QComboBox(this))
    , menu_bar_(new QMenu(this))
    , menu_(new QString("with images"))
    , image_(new Area(this, mode_, label_, menu_)) {
    // NOLINTBEGIN
    setMinimumSize(160, 160);
    resize(1280, 740);
    QWidget* widget = new QWidget(this);
    this->setCentralWidget(widget);
    mode_->addItem("Polygons");
    mode_->addItem("Light");
    mode_->addItem("Static-light");
    QVBoxLayout* layout = new QVBoxLayout;
    QHBoxLayout* layouth = new QHBoxLayout;
    layouth->addWidget(mode_);
    label_->setMaximumWidth(100);
    layouth->addWidget(label_);
    layout->addLayout(layouth);
    layout->addWidget(image_, 1);
    widget->setLayout(layout);
    menu_bar_ = menuBar()->addMenu(tr("&Options"));
    auto* action1 = new QAction("&with images");
    auto* action2 = new QAction("&without images");
    menu_bar_->addAction(action1);
    menu_bar_->addAction(action2);
    connect(action1, &QAction::triggered, this, &MainWindow::Action1);
    connect(action2, &QAction::triggered, this, &MainWindow::Action2);
    // NOLINTEND
    this->update();
    image_->repaint();
}

// NOLINTNEXTLINE
void MainWindow::paintEvent(QPaintEvent* event) {
    image_->update();
}

// NOLINTNEXTLINE
void MainWindow::contextMenuEvent(QContextMenuEvent* event) {
    if (mode_->currentText() == "Static-light") {
        // NOLINTBEGIN
        QDialog* dialog(new QDialog(this));
        QLabel* label1 = new QLabel("Radius", this);
        QLabel* label2 = new QLabel("Intensity", this);
        QSlider* radius_slider = new QSlider(Qt::Horizontal, this);
        QSlider* intencity_slider = new QSlider(Qt::Horizontal, this);
        radius_slider->setMaximum(500);
        radius_slider->setMinimum(0);
        radius_slider->setValue(image_->GetStaticRadius());
        // intencity_slider->setMaximum(255);
        // intencity_slider->setMinimum(0);
        // intencity_slider->setValue(image_->GetStaticIntensity());
        QVBoxLayout* layout = new QVBoxLayout;
        QHBoxLayout* layout1 = new QHBoxLayout;
        label1->setMaximumWidth(60);
        layout1->addWidget(label1);
        layout1->addWidget(radius_slider);
        QHBoxLayout* layout2 = new QHBoxLayout;
        label2->setMaximumWidth(60);
        layout2->addWidget(label2);
        layout2->addWidget(intencity_slider);
        layout->addLayout(layout1);
        // layout->addLayout(layout2);
        dialog->setLayout(layout);
        // NOLINTEND
        dialog->resize(600, 80);
        dialog->exec();
        // image_->SetStaticIntensity(intencity_slider->value());
        image_->SetStaticRadius(radius_slider->value());
        image_->update();
    }
}

Area::Area(QWidget* parent, QComboBox* mode, QLabel* label, QString* menu_bar)
    : QOpenGLWidget(parent)
    , controller_(new Controller)
    , image_(QImage(this->width(), this->height(), QImage::Format::Format_ARGB32_Premultiplied))
    , mode_(mode)
    , label_(label)
    , menu_(menu_bar) {
    // this->setStyleSheet("background-image:url(/home/Rvi/Downloads/maxresdefault.png);
    // background-repeat: norepeat");
    Polygon basic(
        {QPointF(1, 1), QPointF(this->width() - 1, 1),
         QPointF(this->width() - 1, this->height() - 1), QPointF(1, this->height() - 1)});
    controller_->AddPolygon(basic);
    controller_->SetLight(QPointF(1, 1));
    static_image_ =
        QImage(this->width(), this->height(), QImage::Format::Format_ARGB32_Premultiplied);
    static_image_.fill(QColor(0, 0, 0, 0));
    // this->setObjectName("Main");
    this->setStyleSheet("background-color : black");
    // this->setAutoFillBackground(false);
}

void Area::DrawPolygons(QPainter* p) {
    // NOLINTNEXTLINE
    QPen pen(QColor(255, 192, 203), 3, Qt::SolidLine);
    p->setPen(pen);
    // sp->setPen(pen);
    // p->setBrush(Qt::NoBrush);
    // QBrush brush(QPixmap("/home/Rvi/Downloads/maxresdefault(1).png"));
    // QBrush brush(QColor(0, 0, 0, 255));
    p->setBrush(Qt::NoBrush);
    // sp->setBrush(back);
    const auto& polygons = controller_->GetPolygons();
    // std::cout << polygons.size() << " ";
    // int i = 0;
    for (const auto& v : polygons) {
        p->drawPolygon(v.GetVertices().data(), static_cast<int>(v.GetVertices().size()));
        // if (i != 0) {
        //     sp->drawPolygon(v.GetVertices().data(), static_cast<int>(v.GetVertices().size()));
        // }
        // i++;
    }
}

void Area::mousePressEvent(QMouseEvent* event) {
    // std::cout << "1" << " ";
    if (mode_->currentText() == "Polygons") {
        // std::cout << "3";
        CreatePolygon(event);
    }
    if (mode_->currentText() == "Static-light") {
        if (event->button() == Qt::LeftButton) {
            controller_->AddStaticLight(event->pos());
            this->update();
        }
    }
}

void Area::CalculateStatic() {
}

// NOLINTNEXTLINE
void Area::paintEvent(QPaintEvent* event) {
    QElapsedTimer timer;
    timer.start();
    light = QImage(this->width(), this->height(), QImage::Format::Format_ARGB32_Premultiplied);
    back = QImage(this->width(), this->height(), QImage::Format::Format_ARGB32_Premultiplied);
    if (*menu_ == "with images") {
        light = QImage(":images/maxresdefault.png");
        back = QImage(":images/maxresdefault(1).png");

    } else {
        light.fill(Qt::white);
        back.fill(Qt::black);
    }
    back = back.scaled(image_.width(), image_.height());
    light = light.scaled(image_.width(), image_.height());
    // std::cout << this->width() << " " << this->height() << " ";
    image_ = back;
    image_ = image_.scaled(this->width(), this->height());
    light_image_ =
        QImage(this->width(), this->height(), QImage::Format::Format_ARGB32_Premultiplied);
    light_image_.fill(QColor(0, 0, 0, 0));

    static_image_ = static_image_.scaled(this->width(), this->height());

    // image_.fill(QColor(0, 0, 0, 255));
    controller_->Polygons()[0] = Polygon(
        {QPointF(1, 1), QPointF(image_.width() - 1, 1),
         QPointF(image_.width() - 1, image_.height() - 1), QPointF(1, image_.height() - 1)});
    // std::cout << "2";
    // NOLINTNEXTLINE
    QPainter* painter = new QPainter(&image_);
    painter->setRenderHint(QPainter::Antialiasing);

    // NOLINTNEXTLINE
    QPainter* light_painter = new QPainter(&light_image_);
    light_painter->setRenderHint(QPainter::Antialiasing);

    // NOLINTNEXTLINE
    // painter->setRenderHint(QPainter::RenderHint::SmoothPixmapTransform);
    // if (mode_->currentText() == "Polygons") {

    if ((mode_->currentText() == "Static-light") || flag_) {
        static_image_ =
            QImage(this->width(), this->height(), QImage::Format::Format_ARGB32_Premultiplied);
        static_image_.fill(QColor(0, 0, 0, 0));
        // NOLINTNEXTLINE
        QPainter* static_painter = new QPainter(&static_image_);
        static_painter->setRenderHint(QPainter::Antialiasing);
        static_painter->setPen(Qt::NoPen);
        if (controller_->StaticLights().size() > 0) {
            auto polygons = controller_->CreateStatic();
            int i = 0;
            for (const auto& c : polygons) {
                // static_painter->setBrush(QColor(0,0,0,255));
                QRadialGradient gradient(controller_->StaticLights()[i], static_radius_);
                gradient.setColorAt(0, QColor(0, 0, 0, 255));
                gradient.setColorAt(1, QColor(0, 0, 0, static_intensity_));
                // gradient.setColorAt(2, QColor(0, 0, 0, 0));
                static_painter->setBrush(gradient);
                static_painter->drawPolygon(
                    c.GetVertices().data(), static_cast<int>(c.GetVertices().size()));
                i++;
                // static_painter->drawEllipse(c, static_radius_, static_radius_);
            }
        }
        static_painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
        static_painter->fillRect(static_image_.rect(), QBrush(light));
        // NOLINTNEXTLINE
        delete static_painter;
    }
    // NOLINTNEXTLINE
    QPainter* static_painter = new QPainter(&static_image_);
    static_painter->setRenderHint(QPainter::Antialiasing);
    static_painter->setPen(Qt::NoPen);
    static_painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
    static_painter->fillRect(static_image_.rect(), QBrush(light));
    // NOLINTNEXTLINE
    delete static_painter;
    DrawPolygons(painter);
    if (mode_->currentText() == "Light") {
        // CastRay(painter);
        CastRay(light_painter);
        // QPixmap map ("/home/Rvi/Downloads/maxresdefault.png");
    }
    // QPixmap light(("/home/Rvi/Downloads/kvxo4dgdesgb1.png"));
    // QPixmap back(QPixmap("/home/Rvi/Downloads/1677779056124715038.png"));
    light_painter->setCompositionMode(QPainter::CompositionMode_SourceIn);
    light_painter->fillRect(light_image_.rect(), QBrush(light));
    // painter->setCompositionMode(QPainter::CompositionMode_SourceOut);
    // painter->fillRect(image_.rect(), QBrush (QPixmap("/home/Rvi/Downloads/maxresdefault.png")));
    // NOLINTNEXTLINE
    QPainter* render = new QPainter(this);
    render->setRenderHint(QPainter::Antialiasing);
    painter->drawImage(this->rect(), static_image_);
    painter->drawImage(this->rect(), light_image_);
    render->fillRect(this->rect(), QBrush(image_));
    // render->drawImage(this->rect(), static_image_);
    // render->drawImage(this->rect(), light_image_);
    // NOLINTNEXTLINE
    delete painter;
    // NOLINTNEXTLINE
    delete render;
    // NOLINTNEXTLINE
    delete light_painter;

    qint64 frame_time = timer.nsecsElapsed();
    QString s;
    s.setNum(1e9 / static_cast<double>(frame_time));
    s = "FPS: " + s;
    label_->setText(s);
}

void Area::mouseMoveEvent(QMouseEvent* event) {
    if (mode_->currentText() == "Light") {
        ChangeLight(event);
    }
}

void Area::CreatePolygon(QMouseEvent* event) {
    if (event->button() == Qt::LeftButton) {
        auto pos = event->pos();
        if (!flag_) {
            flag_ = true;
            controller_->AddPolygon(Polygon({pos}));
            return;
        }
        controller_->AddVertexToLastPolygon(pos);
        // std::cout << "4";
        // mode_->setCurrentText("Static-light");
        this->repaint();
        // mode_->setCurrentText("Polygons");
    } else {
        flag_ = false;
    }
}

void Area::ChangeLight(QMouseEvent* event) {
    controller_->SetLight(event->pos());
    this->update();
}

void Area::CastRay(QPainter* p) {
    // auto rays = controller_->CastRays();
    // controller_->IntersectRays(rays);
    // QPen pen2(Qt::red, 5, Qt::SolidLine);
    // p->setPen(pen2);
    // int i = 0;
    // for (auto& c : rays) {
    //   p->setPen(pen2);
    //     p->drawLine(c.GetBegin(), c.GetEnd());
    //     QString s;
    //     s.setNum(i);
    //     QPen pen(Qt::black, 10, Qt::SolidLine);
    //     p->setPen(pen);
    //     p->drawText(c.GetEnd(), s);
    //     i++;
    // }
    // p->backgroundMode();
    p->setPen(Qt::NoPen);
    const auto& polygon = controller_->CreateArea();
    const auto& shadow = controller_->Shadow();
    QBrush brush2(QColor(0, 0, 0, 20));
    // QBrush brush2(QColor(255, 192, 203, 50));
    // QPixmap map ("/home/Rvi/Downloads/maxresdefault.png");
    // QBitmap mask = map.createMaskFromColor(QColor(255,0,255,128), Qt::MaskInColor);
    // map.setMask(mask);
    // brush2.setTexture(map);
    p->setBrush(brush2);

    for (const auto& c : shadow) {
        p->drawPolygon(c.GetVertices().data(), static_cast<int>(c.GetVertices().size()));
    }
    // QBrush brush(Qt::white);
    // QBrush brush(QPixmap("/home/Rvi/Downloads/maxresdefault.png"));
    // QBrush brush(gradient);
    QBrush brush(QColor(0, 0, 0, 255));
    // brush.setStyle(Qt::BrushStyle::RadialGradientPattern);
    p->setBrush(brush);
    p->drawPolygon(polygon.GetVertices().data(), static_cast<int>(polygon.GetVertices().size()));
    // QPainterPath path;
    // path.addPolygon(QPolygonF(list));
    // p->fillPath(path, QBrush(Qt::white));
}
