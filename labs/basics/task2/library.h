#ifndef LIBRARY_H
#define LIBRARY_H
#include "book.h"
#include "user.h"
#include <vector>
#include <memory>

class Library {
public:
    void addBook(std::shared_ptr<Book> book);
    void removeBook(const QString& title);
    std::shared_ptr<Book> findBook(const QString& title);
    std::vector<std::shared_ptr<Book>> getBooks() const;
    std::weak_ptr<Book> searchBook(const QString& title);
    void addUser(std::shared_ptr<User> user);
    void removeUser(int userId);
    bool lendBook(int userId, const QString& bookTitle);
    std::vector<std::shared_ptr<User>> getUsers() const;
private:
    std::vector<std::shared_ptr<Book>> books;
    std::vector<std::shared_ptr<User>> users;
};
#endif // LIBRARY_H
