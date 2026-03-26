#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "reviser.hpp"

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<Reviser>("Reviser.Reviser", 1, 0, "Reviser");

    const QUrl url("qrc:/path/qml/main.qml");
    engine.load(url);
    return app.exec();
}
