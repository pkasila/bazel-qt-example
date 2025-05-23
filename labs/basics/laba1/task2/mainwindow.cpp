#include "mainwindow.h"

#include <QPushButton>
#include <QProgressBar>
#include <QSlider>
#include <QVBoxLayout>
#include <QGroupBox>
#include <QDial>
#include <QSplitter>
#include <QListView>
#include <QTreeView>
#include <QTextEdit>
#include <QTabWidget>
#include <QLabel>
#include <QStringListModel>
#include <QListWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    QWidget* central_widget = new QWidget;
    this->setCentralWidget(central_widget);

    this->resize(800, 500);

    QPushButton* button = new QPushButton("Close", this);
    QProgressBar* bar = new QProgressBar(this);
    QSlider* slider = new QSlider(this);
    slider->setOrientation(Qt::Orientation::Horizontal);
    QDial* dial = new QDial(this);
    tab = new QTabWidget(this);

    list_widget = new QListWidget(this);
    slider->setMaximum(10);
    bar->setMaximum(10);
    dial->setMaximum(10);

    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(button);
    layout->addWidget(bar);
    layout->addWidget(slider);
    layout->addWidget(dial);
    layout->addWidget(tab);
    layout->addWidget(list_widget);
    central_widget->setLayout(layout);

    connect(button, &QPushButton::clicked, this, &QWidget::close);
    connect(slider, &QSlider::valueChanged, bar, &QProgressBar::setValue);
    connect(slider, &QSlider::valueChanged, dial, &QDial::setValue);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::ChangeTab);
    connect(slider, &QSlider::valueChanged, this, &MainWindow::ChangeListWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::ChangeTab(int count) {
    tab->clear();

    for (int i = 0; i < count; ++i) {
        QLabel* label = new QLabel(this);
        label->setText("Label " + QString::number(i + 1));
        tab->addTab(label, "Tab " + QString::number(i + 1));
    }
}


void MainWindow::ChangeListWidget(int count) {
    list_widget->clear();

    for (int i = 0; i < count; ++i) {
        list_widget->addItem("Item" + QString::number(i + 1));
    }
}

















