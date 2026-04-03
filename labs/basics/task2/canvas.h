#ifndef CANVAS_H
#define CANVAS_H
#include <QWidget>
#include <QMouseEvent>
#include <QPainter>
#include <QColor>
#include <QPoint>
#include <QTimer>

class Canvas : public QWidget {
    Q_OBJECT
public:
    explicit Canvas(QWidget *parent = nullptr);

    enum class Tool { Pencil, Brush };

    void setTool(Tool newTool);
    void setColor(QColor newColor);
    void setPenWidth(int newPenWidth);
    bool openImage(const QString &fileName);
    bool saveImage(const QString &fileName);
    ~Canvas();
protected:
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;
private:
    void drawLineTo(const QPoint &endPoint);
    void drawPointIn(const QPoint &point);
    void resizeImage(QSize newSize);

    QImage image;
    QPoint lastPoint;
    bool drawing = false;
    QTimer *timer;
    Tool tool;
    QColor penColor;
    int penWidth;
    QWidget *parent;
};

#endif // CANVAS_H
