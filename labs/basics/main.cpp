#include <QApplication>
#include "okno.h"
#include "ticket.h"

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    QPalette palette = qApp->palette();
    palette.setColor(QPalette::Highlight, QColor(0, 0, 200, 128));
    qApp->setPalette(palette);

    okno mainwindow;
    mainwindow.resize(960, 640);
    mainwindow.setWindowTitle("прокрастинатор");
    mainwindow.show();

    return QApplication::exec();
}
// 42% при нуле билетов - не баг а фича
