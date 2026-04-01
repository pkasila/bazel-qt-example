#include "error.hpp"

void showErrorDialog(QString error) {
    for(auto* window : QGuiApplication::topLevelWindows()) {
        if(QQuickWindow* quickWindow = qobject_cast<QQuickWindow*>(window)) {
            QObject* rootObject = quickWindow->contentItem()->parent();
            if(rootObject) {
                QObject* errorDialog = rootObject->property("errorDialogExport").value<QObject*>();
                if(errorDialog) {
                    QMetaObject::invokeMethod(errorDialog, "show", Q_ARG(QString, error));
                    return;
                }
            }
        }
    }
}
