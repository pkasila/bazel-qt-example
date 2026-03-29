#include "library.h"
#include <QMessageBox>

void Library::addBook(std::shared_ptr<Book> book) {
    books.push_back(book);
}
void Library::removeBook(const QString& title) {
    books.erase(std::remove_if(books.begin(), books.end(), [&](std::shared_ptr<Book> book) {
                    return book->getTitle() == title;
                }), books.end());
}
std::shared_ptr<Book> Library::findBook(const QString& title) {
    for (auto& book : books) {
        if (book->getTitle() == title)
            return book;
    }
    return nullptr;
}
std::vector<std::shared_ptr<Book>> Library::getBooks() const {
    return books;
}
std::weak_ptr<Book> Library::searchBook(const QString& title) {
    for (auto& book : books) {
        if (book->getTitle() == title)
            return book;
    }
    return std::weak_ptr<Book>();
}
void Library::addUser(std::shared_ptr<User> user) {
    users.push_back(user);
}
void Library::removeUser(int userId) {
    users.erase(std::remove_if(users.begin(), users.end(), [&](std::shared_ptr<User> user) {
                    return user->getId() == userId;
                }), users.end());
}
bool Library::lendBook(int userId, const QString& bookTitle) {
    auto book = findBook(bookTitle);
    if (book && book->getBorrowerId() == -1) {  // Книга доступна
        book->setBorrowerId(userId);
        return true;
    } else {
        QMessageBox::warning(nullptr, "Lend Book", "Book is already borrowed.");
        return false;
    }
}
std::vector<std::shared_ptr<User>> Library::getUsers() const {
    return users;
}
