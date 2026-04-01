#ifndef CHECKBOX_HPP
#define CHECKBOX_HPP

#include <QObject>

class Checkbox : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER text NOTIFY textChanged)
    Q_PROPERTY(bool need MEMBER need NOTIFY needChanged)

public:
    Checkbox(QObject *parent = nullptr) : QObject(parent) {}

signals:
    void textChanged();
    void needChanged();

private:
    QString text;
    bool need = false;
};

#endif // CHECKBOX_HPP
