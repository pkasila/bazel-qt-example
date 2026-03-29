#ifndef BOOK_H
#define BOOK_H
#include <QString>
#include <memory>

class Book {
public:
    Book(const QString& title, const QString& author, int year);
    QString getTitle() const;
    QString getAuthor() const;
    int getYear() const;
    QString getInfo() const;
    int getBorrowerId() const;
    void setBorrowerId(int id);

private:
    QString title;
    QString author;
    int year;
    int borrowerID = -1;
};
#endif // BOOK_H
