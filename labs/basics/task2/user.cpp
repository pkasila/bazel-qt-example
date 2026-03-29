#include "user.h"

User::User(const QString& name, int id) : name(name), id(id) {}
QString User::getName() const { return name; }
int User::getId() const { return id; }
