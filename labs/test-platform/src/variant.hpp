#ifndef VARIANT_HPP
#define VARIANT_HPP

#include <QObject>

class Variant : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString text MEMBER text NOTIFY textChanged);
    Q_PROPERTY(int score MEMBER score NOTIFY scoreChanged);
    Q_PROPERTY(bool correct MEMBER correct NOTIFY correctChanged)

public:
    Variant(QObject* parent = nullptr) : QObject(parent) {}

signals:
    void textChanged();
    void scoreChanged();
    void correctChanged();

private:
    QString text;
    int score = 0;
    bool correct = false;
};

#endif // VARIANT_HPP
