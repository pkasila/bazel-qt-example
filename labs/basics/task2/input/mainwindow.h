#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow> //
#include <QTimer>

QT_BEGIN_NAMESPACE
class QPushButton;
class QLabel;
class QSlider;
class QSpinBox;
class QVBoxLayout;
class QHBoxLayout;
class QWidget;
class QFormLayout;
QT_END_NAMESPACE

class LifeWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void toggleSimulation();
    void stepOnce();
    void clearGrid();
    void updateSpeed(int value);
    void updateGenerationLabel(int generation);
    void applyNewGridSize();

private:
    void setupUi();
    void connectSignalsSlots();

    LifeWidget *lifeWidget;
    QPushButton *startButton;
    QPushButton *stepButton;
    QPushButton *clearButton;
    QLabel *generationLabel;
    QSlider *speedSlider;
    QSpinBox *speedSpinBox;

    QSpinBox *rowsSpinBox;
    QSpinBox *colsSpinBox;
    QPushButton *resizeButton;

    QTimer *timer;
    bool isRunning;
};
#endif // MAINWINDOW_H