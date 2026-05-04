#include "factorizer.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);
    
    app.setApplicationName("Prime Factorizer");
    app.setApplicationDisplayName("Prime Factorizer");
    
    PrimeFactorizer window;
    window.show();
    
    return app.exec();
}