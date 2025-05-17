#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QMenu>
#include <QStackedWidget>
#include "translation.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void GetResults();
private:

    void setupUI();
    void setupMenu();
    void ChooseMain();
    void ChooseTranslation();
    void ChooseWriting();
    QMenu* startMenu;
    QWidget* startPage;
    Translation* translationPage;
    QWidget* writingPage;
    QStackedWidget*  windowStack;
    QVBoxLayout* main_layout;
    QLabel* translationResultText = nullptr;
};
#endif // MAINWINDOW_H
