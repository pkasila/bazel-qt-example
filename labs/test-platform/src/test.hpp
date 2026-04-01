#ifndef TEST_HPP
#define TEST_HPP

#include <QObjectList>
#include <toml.hpp>

class Test : public QObject {
    Q_OBJECT
    Q_PROPERTY(QString name MEMBER name NOTIFY nameChanged)
    Q_PROPERTY(QObjectList questions MEMBER questions NOTIFY questionsChanged)

signals:
    void nameChanged();
    void questionsChanged();

public:
    Test(QObject *parent = nullptr) : QObject(parent) {}
    bool load(const std::filesystem::path& path);

private:
    void loadFromToml(const toml::value& table, const std::filesystem::path& path);

    QString name;
    QObjectList questions;
};

#endif // TEST_HPP
