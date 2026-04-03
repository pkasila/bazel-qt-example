#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QComboBox>
#include <QLabel>
#include "canvas.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        
        auto *centralWidget = new QWidget(this);
        auto *layout = new QVBoxLayout(centralWidget);

        auto *controls = new QHBoxLayout();
        auto *label = new QLabel("Рэжым:", this);
        auto *modeSelector = new QComboBox(this);
        modeSelector->addItem("Святло (Light)");
        modeSelector->addItem("Мнагакутнікі (Polygons)");
        
        controls->addWidget(label);
        controls->addWidget(modeSelector);
        controls->addStretch();
        
        layout->addLayout(controls);

        auto *canvas = new Canvas(this);
        layout->addWidget(canvas, 1);

        connect(modeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [canvas](int index){
                    canvas->setMode(index == 0 ? Canvas::LIGHT_MODE : Canvas::POLYGON_MODE);
                });

        setCentralWidget(centralWidget);
        setWindowTitle("2D Raycaster Lab");
        resize(1000, 800);
    }
};

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    MainWindow window;
    window.show();
    
    return app.exec();
}