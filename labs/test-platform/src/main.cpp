#include <wasm_runtime_common.h>
#include <QQmlApplicationEngine>
#include "test_platform.hpp"

int main(int argc, char* argv[]) {
    wasm_runtime_init();

    QGuiApplication app(argc, argv);
    QQmlApplicationEngine engine;

    qmlRegisterType<TestPlatform>("TestPlatform.TestPlatform", 1, 0, "TestPlatform");

    const QUrl url("qrc:/path/qml/main.qml");
    engine.load(url);
    return app.exec();
}
