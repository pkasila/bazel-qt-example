#ifndef USER_H
#define USER_H
#include <QString>
#include <memory>

class User {
public:
    User(const QString& name, int id);
    QString getName() const;
    int getId() const;
private:
    QString name;
    int id;
};
#endif // USER_H
