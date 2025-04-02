#ifndef CURSORCHANGINGBUTTON_H
#define CURSORCHANGINGBUTTON_H
#include <QWidget>
#include <QPushButton>
#include <QString>

class CursorChangingButton : public QPushButton {
    Q_OBJECT

public:
    explicit CursorChangingButton(const QString& name, QWidget *parent);
};

#endif // CURSORCHANGINGBUTTON_H
