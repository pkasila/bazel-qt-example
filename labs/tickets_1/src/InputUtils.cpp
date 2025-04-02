#include "InputUtils.h"

bool InputUtils::validateInput(QWidget *parent, const QString &input) {
    //positive integer is required
    bool ok;
    int value = input.toInt(&ok);
    if (!ok || value <= 0) {
        QMessageBox::warning(parent, WindowTitles::validateInputWarningTitle,
            WindowTexts::validateInputWarningMessage);
        return false;
    }
    return true;
}
