#include <QMainWindow>
#include <QVBoxLayout>
#include <QComboBox>
#include "canvas.h"

class MainWindow : public QMainWindow {
public:
    MainWindow(QWidget *parent = nullptr) : QMainWindow(parent) {
        QWidget *central = new QWidget(this);
        QVBoxLayout *layout = new QVBoxLayout(central);

        QComboBox *modeSelector = new QComboBox(this);
        modeSelector->addItem("<light>");
        modeSelector->addItem("<polygons>");

        Canvas *canvas = new Canvas(this);
        
        layout->addWidget(modeSelector);
        layout->addWidget(canvas, 1); // 1 = stretch factor

        connect(modeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged),
                [canvas](int index){
                    canvas->setMode(index == 0 ? Canvas::LIGHT_MODE : Canvas::POLYGON_MODE);
                });

        setCentralWidget(central);
        resize(800, 600);
        setWindowTitle("2D Raycaster");
    }
};