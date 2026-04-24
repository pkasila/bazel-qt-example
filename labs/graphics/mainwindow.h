#pragma once
#include <QMainWindow>
#include <QComboBox>
#include <QVBoxLayout>
#include "canvas.h"

class MainWindow : public QMainWindow {
public:
    MainWindow() {
        auto* w = new QWidget();
        auto* l = new QVBoxLayout(w);
        auto* c = new QComboBox();
        c->addItems({"Polygons", "Light"});
        auto* cv = new Canvas();
        l->addWidget(c); l->addWidget(cv);
        setCentralWidget(w); resize(1000, 800);
        connect(c, QOverload<int>::of(&QComboBox::currentIndexChanged), [cv](int i){
            cv->setMode(i == 0 ? Mode::Polygons : Mode::Light);
        });
    }
};