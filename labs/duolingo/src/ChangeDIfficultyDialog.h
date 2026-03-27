#ifndef CHANGEDIFFICULTYDIALOG_H
#define CHANGEDIFFICULTYDIALOG_H

#include "Settings.h"

#include <QDialog>
#include <memory>

class ChangeDIfficultyDialog : public QDialog {
    Q_OBJECT
   public:
    explicit ChangeDIfficultyDialog(const std::shared_ptr<Settings>&, QWidget* parent = nullptr);
};

#endif  // CHANGEDIFFICULTYDIALOG_H
