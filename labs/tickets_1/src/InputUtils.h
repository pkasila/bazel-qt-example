#ifndef INPUTUTILS_H
#define INPUTUTILS_H
#include <QWidget>
#include <QString>
#include <QMessageBox>
//
#include "ProjectConstants.h"

class InputUtils {
public:
    static bool validateInput(QWidget *parent, const QString &input);
};
#endif // INPUTUTILS_H
