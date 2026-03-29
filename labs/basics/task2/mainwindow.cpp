#include "mainwindow.h"
#include <QVBoxLayout>
#include <QPushButton>
#include <QInputDialog>
#include <QMessageBox>
#include <QHeaderView>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    bookTableWidget = new QTableWidget(this);
    bookTableWidget->setColumnCount(1);
    bookTableWidget->setHorizontalHeaderLabels({"Books"});
    bookTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(bookTableWidget);

    userTableWidget = new QTableWidget(this);
    userTableWidget->setColumnCount(1);
    userTableWidget->setHorizontalHeaderLabels({"Users"});
    userTableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    layout->addWidget(userTableWidget);

    QPushButton *manageButton = new QPushButton("Manage Library", this);
    layout->addWidget(manageButton);
    connect(manageButton, &QPushButton::clicked, this, &MainWindow::manageLibrary);
}

void MainWindow::manageLibrary() {
    QStringList options = {"Add Book", "Remove Book", "Search Book", "Add User", "Remove User", "Lend Book"};
    bool ok;
    QString choice = QInputDialog::getItem(this, "Manage Library", "Choose an action:", options, 0, false, &ok);
    if (ok) {
        if (choice == "Add Book") addBook();
        else if (choice == "Remove Book") removeBook();
        else if (choice == "Search Book") searchBook();
        else if (choice == "Add User") addUser();
        else if (choice == "Remove User") removeUser();
        else if (choice == "Lend Book") lendBook();
    }
}

void MainWindow::addBook() {
    QString title = QInputDialog::getText(this, "Add Book", "Title:");
    QString author = QInputDialog::getText(this, "Add Book", "Author:");
    int year = QInputDialog::getInt(this, "Add Book", "Year:");
    if (!title.isEmpty() && !author.isEmpty()) {
        auto book = std::make_shared<Book>(title, author, year);
        library.addBook(book);
        updateBookList();
    }
}
void MainWindow::removeBook() {
    QString title = QInputDialog::getText(this, "Remove Book", "Enter book title:");
    library.removeBook(title);
    updateBookList();
}
void MainWindow::searchBook() {
    QString title = QInputDialog::getText(this, "Search Book", "Enter book title:");
    std::weak_ptr<Book> foundBook = library.searchBook(title);
    if (auto book = foundBook.lock()) {
        QMessageBox::information(this, "Book Found", book->getInfo());
    } else {
        QMessageBox::warning(this, "Not Found", "Book not found");
    }
}
void MainWindow::addUser() {
    QString name = QInputDialog::getText(this, "Add User", "Name:");
    int id = QInputDialog::getInt(this, "Add User", "ID:");
    if (!name.isEmpty()) {
        auto user = std::make_shared<User>(name, id);
        library.addUser(user);
        updateUserList();
    }
}
void MainWindow::removeUser() {
    int id = QInputDialog::getInt(this, "Remove User", "Enter User ID:");
    library.removeUser(id);
    updateUserList();
}
void MainWindow::lendBook() {
    int userId = QInputDialog::getInt(this, "Lend Book", "Enter User ID:");
    QString bookTitle = QInputDialog::getText(this, "Lend Book", "Enter Book Title:");
    if (library.lendBook(userId, bookTitle)) {
        updateBookList();
        QMessageBox::information(this, "Lend Book", "Book issued successfully.");
    }
}
void MainWindow::updateBookList() {
    bookTableWidget->setRowCount(0);
    for (const auto& book : library.getBooks()) {
        int row = bookTableWidget->rowCount();
        bookTableWidget->insertRow(row);
        bookTableWidget->setItem(row, 0, new QTableWidgetItem(book->getInfo()));
    }
}
void MainWindow::updateUserList() {
    userTableWidget->setRowCount(0);
    for (const auto& user : library.getUsers()) {
        int row = userTableWidget->rowCount();
        userTableWidget->insertRow(row);
        userTableWidget->setItem(row, 0, new QTableWidgetItem(QString("%1 (ID: %2)").arg(user->getName()).arg(user->getId())));
    }
}

