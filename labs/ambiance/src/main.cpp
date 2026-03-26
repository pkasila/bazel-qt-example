#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <ambiance.hpp>

int main(int argc, char* argv[]) {
    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<amb::Ambiance>("Ambiance.Ambiance", 1, 0, "Ambiance");

    const QUrl url("qrc:/path/qml/main.qml");
    engine.load(url);
    return app.exec();
}
