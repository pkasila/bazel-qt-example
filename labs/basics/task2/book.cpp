#include "book.h"

Book::Book(const QString& title, const QString& author, int year) : title(title), author(author), year(year) {}
QString Book::getTitle() const { return title; }
QString Book::getAuthor() const { return author; }
int Book::getYear() const { return year; }
QString Book::getInfo() const {
    if (borrowerID == -1) {
        return QString("%1, %2, %3 (Available)").arg(title).arg(author).arg(year);
    } else {
        return QString("%1, %2, %3 (Borrowed by User ID: %4)").arg(title).arg(author).arg(year).arg(borrowerID);
    }
}int Book::getBorrowerId() const { return borrowerID; }
void Book::setBorrowerId(int id) { borrowerID = id; }