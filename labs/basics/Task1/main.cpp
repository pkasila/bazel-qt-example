#include "mainwindow.h"

#include <QApplication>

#include <QMainWindow>
#include <QSpinBox>
#include <QListWidget>
#include <QPushButton>
#include <QProgressBar>
#include <QLineEdit>
#include <QLabel>
#include <QGroupBox>
#include <QComboBox>
#include <QMap>
#include <QGridLayout>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    return a.exec();
}
