#include "CursorChangingButton.h"

CursorChangingButton::CursorChangingButton(const QString& name,
        QWidget *parent) : QPushButton(name, parent) {
    setCursor(Qt::PointingHandCursor);
}
