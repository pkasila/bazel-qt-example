#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QScrollArea>
#include <QList>
#include <QPointer>
#include <QShortcut>
#include <QSpinBox>
#include <QCalendarWidget>
#include <QDate>
#include <QFile>
#include <QDir>
#include <QCoreApplication>
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QMessageBox>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onButton1Clicked();
    void onButton2Clicked();
    void onButton3Clicked();
    void showMenu();

    void stage1Clicked();

    void addInputPairRow();
    void removeInputPairRow(QWidget* rowWidgetToRemove);
    void crt();

    void stage3Clicked();

    void showHelpDialog();

private:
    void setupStWidget();
    void setupMenu();
    void setupScene1Widget();
    void setupScene2Widget();
    void setupScene3Widget();
    void setupMenuBar();

    QStackedWidget *stackedWidget;
    QWidget *menuWidget;
    QWidget *Scene1Widget;
    QWidget *Scene2Widget;
    QWidget *Scene3Widget;

    QPushButton *button1;
    QPushButton *button2;
    QPushButton *button3;

    QLineEdit *numberInputLineEdit;
    QLabel *resultTextLabel;


    QVBoxLayout *pairsLayout;
    QLabel *resultLabelForScene2;
    struct InputPairRow {
        QPointer<QWidget> rowContainerWidget;
        QPointer<QLineEdit> lineEditA;
        QPointer<QLineEdit> lineEditB;
        QPointer<QPushButton> removeButton;
    };
    QList<InputPairRow> inputPairRowsList;


    QSpinBox *scene3SpinBox;
    QCalendarWidget *scene3Calendar;
    QLabel *scene3ResultLabel;


    QMenu *fileMenu;
    QAction *helpAction;
};

#endif // MAINWINDOW_H