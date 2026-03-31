#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QGraphicsScene>
#include <QGraphicsView>
#include <QGraphicsPolygonItem>
#include <QGraphicsEllipseItem>
#include "controller.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    bool eventFilter(QObject* watched, QEvent* event) override;

private slots:
    void onModeChanged(int index);
    void handleViewportMousePress(QMouseEvent* event);
    void handleViewportMouseMove(QMouseEvent* event);

private:
    void setupUserInterface();
    void redrawSceneContent();
    void drawLightAreaVisuals();
    void drawObstaclePolygons();
    void drawLightSourceMarkers();
    void updateControllerLightSourcePositions(const QPointF& main_cursor_pos);
    void drawSceneAABB();

    QGraphicsScene* m_mainGraphicsScene;
    QGraphicsView* m_mainGraphicsView;
    QComboBox* m_applicationModeComboBox;
    Controller* m_logicController;

    std::vector<QGraphicsPolygonItem*> m_renderedLightAreaItems;
    std::vector<QGraphicsEllipseItem*> m_renderedLightSourceMarkerItems;

    struct VisualStyleSettings {
        qreal lightMarkerRadius = 3.5;
        qreal secondaryLightSpreadRadius = 15.0;
        int numberOfSecondaryLights = 4;
        QColor lightPolygonFillColor = QColor(255, 255, 150, 80);
    } m_visualStyle;

    bool m_isCurrentlyDrawingPolygon;
    int m_currentApplicationMode;

    const int m_sceneWidthValue = 800;
    const int m_sceneHeightValue = 600;
};

#endif // MAINWINDOW_H

