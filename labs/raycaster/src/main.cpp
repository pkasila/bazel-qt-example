#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "raycaster.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<rc::Raycaster>("Raycaster.Raycaster", 1, 0, "Raycaster");

    const QUrl url("qrc:/path/qml/main.qml");
    engine.load(url);
    return app.exec();
}
