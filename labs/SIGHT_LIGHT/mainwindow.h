#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>

// Вместо forward declaration используем полное включение
#include "controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event);

private slots:
    void onModeChanged(int index);
    void onMousePressed(QPointF pos, Qt::MouseButton button);
    void onMouseMoved(QPointF pos);

private:
    void setupUI();
    void updateScene();
    void drawLightArea(const QPointF& source);
    void drawPolygons();
    void drawLightSource(const QPointF& light);

    void drawLightAreas();
    void updateLightSources(const QPointF& mainPos);
    void drawLightSources();

    void drawBoundingBox();

    std::vector<QGraphicsPolygonItem*> m_lightAreaItems;
    std::vector<QGraphicsEllipseItem*> m_lightSourceItems;
    std::vector<QGraphicsEllipseItem*> m_vertexItems;

    struct {
        qreal lightSourceRadius = 3.5;
        qreal lightSpreadRadius = 18.0;
        int lightSourceCount = 9;
        QColor lightAreaColor = QColor(255, 240, 150, 40);
    } m_visualSettings;

    QGraphicsScene* m_scene;
    QGraphicsView* m_view;
    QComboBox* m_modeComboBox;
    Controller* m_controller;
    bool m_drawingPolygon = false;
    int m_currentMode = 0;
    int m_sceneWidth = 800;
    int m_sceneHeight = 600;
};

#endif // MAINWINDOW_H
