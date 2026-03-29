#pragma once

#include "PictureView.h"
#include "DrawingTable.h"
#include <QColorDialog>
#include <QHeaderView>
#include <QSpinBox>
#include <QLCDNumber>
#include <QToolButton>
#include <QIcon>

const QColor colors[17] = {
    QColor(255, 0, 0),
    QColor(255, 69, 0),
    QColor(255, 165, 0),
    QColor(255, 255, 0),
    QColor(154, 205, 50),
    QColor(0, 128, 0),
    QColor(0, 255, 0),
    QColor(0, 255, 127),
    QColor(0, 255, 255),
    QColor(0, 191, 255),
    QColor(0, 0, 255),
    QColor(75, 0, 130),
    QColor(128, 0, 128),
    QColor(255, 0, 255),
    QColor(255, 192, 203),
    QColor(255, 255, 255),
    QColor(0, 0, 0)
};

class MyWindow : public QWidget {
    Q_OBJECT
public:
    MyWindow(QWidget* parent = nullptr) : QWidget(parent) {
        layout = new QVBoxLayout(this);
        upLayout = new QHBoxLayout();
        colorLayout = new QHBoxLayout();

        labelY = new QLabel("Количество строк", this);
        labelY->setAlignment(Qt::AlignCenter);
        labelX = new QLabel("Количество столбцов", this);
        labelX->setAlignment(Qt::AlignCenter);
        label = new QLabel("Размер клетки", this);
        label->setAlignment(Qt::AlignCenter);

        boxY = new QSpinBox(this);
        boxX = new QSpinBox(this);
        box = new QSpinBox(this);
        boxY->setToolTip("Выбери число строк");
        boxX->setToolTip("Выбери число столбцов");
        box->setToolTip("Выбери размер клетки");
        boxY->setRange(1, 100);
        boxX->setRange(1, 100);
        box->setRange(23, 200);
        boxY->setSingleStep(1);
        boxX->setSingleStep(1);
        box->setSingleStep(1);
        boxY->setValue(n);
        boxX->setValue(m);
        box->setValue(cell_size);
        view = new PictureView();

        chosePicture = new QPushButton("Выбери рисунок", this);
        connect(chosePicture, &QPushButton::clicked, this, [this]() {
            //view = new PictureView();
            qDebug() << "here";
            view->show();
            this->activateWindow();
        });

        int i = 0;
        for (const QColor& color : colors) {
            colorButtons.push_back(new QPushButton(this));
            colorButtons.back()->setStyleSheet(QString("background-color: %1; border: 0px solid white;").arg(color.name()));
            colorButtons.back()->setFixedSize(30, 30);
            connect(colorButtons.back(), &QPushButton::clicked, this, [this, color, i]() {
                curcolor = color;
                drawingField->setColor(color);
                selectColor(i);
            });
            colorLayout->addWidget(colorButtons.back());
            i++;
        }
        selectColor(index);
        customColorButton = new QPushButton("Custom", this);
        connect(customColorButton, &QPushButton::clicked, this, [this, i](){
            QColor color = QColorDialog::getColor(curcolor, this, "Выберите цвет");
            if (color.isValid()) {
                drawingField->setColor(color);
                curcolor = color;
                selectColor(i);
            }

        });
        colorButtons.push_back(customColorButton);
        colorLayout->addWidget(customColorButton);

        clearAllButton = new QToolButton(this);
        clearAllButton->setFixedSize(30, 30);
        clearAllButton->setIcon(QIcon(":/p2.jpeg"));
        clearAllButton->setIconSize(QSize(30, 30));

        connect(clearAllButton, &QToolButton::clicked, this, &MyWindow::clearAll);
        colorLayout->addWidget(clearAllButton);

        count = new QLCDNumber(this);
        count->setStyleSheet("background-color: rgb(3,50,100); color: red; border: 2px solid black;");
        count->setDigitCount(5);
        count->display(0);

        drawingField = new DrawingTable(this, count);

        upLayout->addWidget(labelY);
        upLayout->addWidget(boxY);
        upLayout->addWidget(labelX);
        upLayout->addWidget(boxX);
        upLayout->addWidget(label);
        upLayout->addWidget(box);
        upLayout->addWidget(chosePicture);
        upLayout->addWidget(count);
        layout->addLayout(upLayout);
        layout->addLayout(colorLayout);
        layout->addWidget(drawingField);
        setLayout(layout);

        connect(boxY, QOverload<int>::of(&QSpinBox::valueChanged),this, &MyWindow::updateY);
        connect(boxX, QOverload<int>::of(&QSpinBox::valueChanged),this, &MyWindow::updateX);
        connect(box, QOverload<int>::of(&QSpinBox::valueChanged),this, &MyWindow::updateCellSize);

        updateY(n);
        updateX(m);
        updateCellSize(cell_size);
    }

    void update() {
        for (int i = 0; i < drawingField->rowCount(); i++) {
            for (int j = 0; j < drawingField->columnCount(); j++) {
                if (drawingField->item(i, j)) {
                    continue;
                }
                item = new QTableWidgetItem();
                item->setBackground(Qt::white);
                drawingField->setItem(i, j, item);
            }
        }
    }

    void selectColor(int ind) {
        colorButtons[index]->setStyleSheet(QString("background-color: %1; border: 0px solid white;").arg(index < 17 ? colors[index].name() : "white"));
        index = ind;
        if (colors[ind] != Qt::black) {
            colorButtons[index]->setStyleSheet(QString("background-color: %1; border: 2px solid black;").arg(index < 17 ? colors[index].name() : "white"));
        } else {
            colorButtons[index]->setStyleSheet(QString("background-color: %1; border: 2px solid red;").arg(index < 17 ? colors[index].name() : "white"));
        }
    }

    void setColor(const QColor& color) {
        curcolor = color;
    }

    QColor getColor() const {
        return curcolor;
    }

private Q_SLOTS:
    void updateCellSize(int size) {
        cell_size = size;
        for (int i = 0; i < drawingField->rowCount(); i++) {
            drawingField->setRowHeight(i, cell_size);
        }
        for (int j = 0; j < drawingField->columnCount(); j++) {
            drawingField->setColumnWidth(j, cell_size);
        }
    }

    void updateY(const int y) {
        if (y < n) {
            for (int i = y; i < n; i++) {
                for (int j = 0; j < m; j++) {
                     drawingField->clearColor(i, j);
                }
            }
        }
        n = y;
        drawingField->setRowCount(y);
        updateCellSize(cell_size);
        update();
    }

    void updateX(const int x) {
        if (x < m) {
            for (int j = x; j < m; j++) {
                for (int i = 0; i < n; i++) {
                     drawingField->clearColor(i, j);
                }
            }
        }
        m = x;
        drawingField->setColumnCount(x);
        updateCellSize(cell_size);
        update();
    }

    void clearAll() {
        for (int i = 0; i < n; i++) {
            for (int j = 0; j < m; j++) {
                drawingField->clearColor(i, j);
            }
        }
    }

private:
    int n = 10;
    int m = 10;
    int cell_size = 50;
    int index = 16;
    PictureView* view;
    QLCDNumber* count;
    QColor curcolor;
    QLabel* labelY;
    QLabel* labelX;
    QLabel* label;
    QSpinBox* boxY;
    QSpinBox* boxX;
    QSpinBox* box;
    std::vector<QPushButton*> colorButtons;
    QPushButton* customColorButton;
    QPushButton* chosePicture;
    QToolButton* clearAllButton;
    DrawingTable* drawingField;
    QVBoxLayout* layout;
    QHBoxLayout* upLayout;
    QHBoxLayout* colorLayout;
    QTableWidgetItem* item;
};
