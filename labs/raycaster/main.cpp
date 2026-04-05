#include <QApplication>
#include <QMainWindow>
#include <QVBoxLayout>
#include <QComboBox>
#include "canvas.h"

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    QMainWindow window;
    window.setWindowTitle("2D Raycasting Light System");
    window.resize(1000, 700);

    // Основной виджет и верстка
    QWidget *central = new QWidget(&window);
    QVBoxLayout *layout = new QVBoxLayout(central);
    
    // Панель управления
    QComboBox *modeSelector = new QComboBox();
    modeSelector->addItem("Режим рисования (ЛКМ - точка, ПКМ - завершить)", Canvas::Polygons);
    modeSelector->addItem("Режим освещения (Двигайте мышью)", Canvas::Light);
    
    Canvas *canvas = new Canvas();
    
    layout->addWidget(modeSelector);
    layout->addWidget(canvas);
    
    // Смена режима через Signal/Slot
    QObject::connect(modeSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), 
                     [canvas, modeSelector](int index){
        canvas->setMode(static_cast<Canvas::Mode>(modeSelector->itemData(index).toInt()));
    });

    // Устанавливаем темную тему для контраста
    QPalette pal = window.palette();
    pal.setColor(QPalette::Window, Qt::black);
    window.setPalette(pal);

    window.setCentralWidget(central);
    window.show();

    return a.exec();
}