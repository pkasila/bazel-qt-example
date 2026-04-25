#pragma once

#include <QtWidgets>
#include <vector>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class CustomLineEdit : public QLineEdit {
    Q_OBJECT

    public:
       CustomLineEdit(QWidget* parent) : QLineEdit(parent) {
       }

    signals:
       void hitEnter(int index, QString text);

    protected:
       void keyPressEvent(QKeyEvent* event) override;
};

class CustomComboBox : public QComboBox {
    Q_OBJECT

    public:
       CustomComboBox(QWidget* parent) : QComboBox(parent) {
       }

    signals:
       void indexChangedWithSender(int box_index);
};

class StartupMenu : public QWidget {
    Q_OBJECT
public:
    StartupMenu();

signals:
    void startClicked();
};