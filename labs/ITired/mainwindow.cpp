#include "mainwindow.h"
//#include "./ui_mainwindow.h"
#include <QMediaPlayer>
#include <QSoundEffect>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
//, ui(new Ui::MainWindow)
{
    //ui->setupUi(this);

    QHBoxLayout* mainL = new QHBoxLayout(this);

    stackWidgets = new QStackedWidget();
    leftW = new LeftMenu();
    //leftW->show();

    hello = new QWidget();
    hello->setStyleSheet(
        "background-color: #f8f9fa;"
        "border-radius: 10px;"
        "border: 1px solid #e0e0e0;"
    );

    QVBoxLayout* helloLayout = new QVBoxLayout(hello);
    helloLayout->setContentsMargins(40, 40, 40, 40);
    helloLayout->setSpacing(25);

    QLabel* welcomeLabel = new QLabel("Welcome to Language App!", hello);
    welcomeLabel->setStyleSheet(
        "QLabel {"
        "   color: #333333;"
        "   font-size: 24px;"
        "   font-weight: bold;"
        "   padding: 5px;"
        "}"
    );
    welcomeLabel->setAlignment(Qt::AlignCenter);
    helloLayout->addStretch();
    helloLayout->addWidget(welcomeLabel);


    translateW = new Translate();
    translateW->setupDiffculty(indexDifficulty);

    grammarW = new Grammar();
    grammarW->setupDiffculty(indexDifficulty);

    stackWidgets->addWidget(hello);
    stackWidgets->addWidget(translateW);
    stackWidgets->addWidget(grammarW);

    connect(leftW, &LeftMenu::trButClicked, this, [&]() {
        stackWidgets->setCurrentIndex(1);
        translateW->startExercise();
        leftW->endGr();
    });

    connect(leftW, &LeftMenu::grButClicked, this, [&]() {
        stackWidgets->setCurrentIndex(2);
        grammarW->startExercise();
        leftW->endTr();
    });

    connect(translateW, &Translate::end, this, [&]() {
        leftW->globalScore += translateW->getScore();
        leftW->updateLabel(leftW->globalScore);
        translateW->nullScore();
        stackWidgets->setCurrentIndex(0);
        leftW->stGr();
    });

    connect(grammarW, &Grammar::end, this, [&]() {
        leftW->globalScore += grammarW->getScore();
        leftW->updateLabel(leftW->globalScore);
        grammarW->nullScore();
        stackWidgets->setCurrentIndex(0);
        leftW->stTr();
    });

    mainL->addWidget(leftW);
    mainL->addWidget(stackWidgets);


    QMenuBar *menuBar = new QMenuBar(this);
    QMenu *menu = new QMenu("Menu", this);
    menuBar->addMenu(menu);
    setMenuBar(menuBar);

    QAction *beginnerAction = new QAction("Beginner", this);
    QAction *intermediateAction = new QAction("Intermediate", this);
    QAction *advancedAction = new QAction("Advanced", this);

    menu->addAction(beginnerAction);
    menu->addAction(intermediateAction);
    menu->addAction(advancedAction);

    connect(beginnerAction, &QAction::triggered, this, &MainWindow::showLevelDialogB);
    connect(intermediateAction, &QAction::triggered, this, &MainWindow::showLevelDialogI);
    connect(advancedAction, &QAction::triggered, this, &MainWindow::showLevelDialogA);

    QWidget *dummyWidget = new QWidget();
    dummyWidget->setLayout(mainL);
    setCentralWidget(dummyWidget);

}

MainWindow::~MainWindow()
{
    //delete ui;
}
