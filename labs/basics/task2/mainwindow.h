#ifndef MAINWINDOW_H
#define MAINWINDOW_H
#include <QMainWindow>
#include "library.h"
#include <QTableWidget>

class MainWindow : public QMainWindow {
    Q_OBJECT
public:
    MainWindow(QWidget *parent = nullptr);
private slots:
    void manageLibrary();
    void addBook();
    void removeBook();
    void searchBook();
    void addUser();
    void removeUser();
    void lendBook();
private:
    Library library;
    QTableWidget* bookTableWidget;
    QTableWidget* userTableWidget;
    void updateBookList();
    void updateUserList();
};
#endif // MAINWINDOW_H
