#ifndef TEST_PLATFORM_HPP
#define TEST_PLATFORM_HPP

#include <QQuickItem>
#include <QObjectList>
#include "test.hpp"

class TestPlatform : public QQuickItem {
    Q_OBJECT
    Q_PROPERTY(QObjectList tests MEMBER tests NOTIFY testsChanged)

public:
    Q_INVOKABLE void loadTestsList();

signals:
    void testsChanged();

private:
    QObjectList tests;
};

#endif // TEST_PLATFORM_HPP
