#include "mainwindow.h"
#include "translation.h"


#include <QActionGroup>
#include <QApplication>
#include <QLabel>
#include <QMenuBar>
#include <QMessageBox>
#include <QStatusBar>
#include <QVBoxLayout>
#include <QStackedWidget>
#include <QDebug>
#include <QImageReader>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{

    setWindowTitle(tr("English learning application"));
    resize(900, 700);
    setMinimumSize(640, 480);
    setupUI();
    setupMenu();
    connect(translationPage, &Translation::EndGame, this, &MainWindow::GetResults);
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI(){
    this->setStyleSheet("background-color: #3b3535; color: white;");
    startPage = new QWidget(this);
    translationPage = new Translation(this);
    writingPage = new QWidget(this);
    main_layout = new QVBoxLayout(startPage);
    main_layout->setContentsMargins(20, 20, 20, 20);
    QLabel* welcome_label = new QLabel("Welcome in Duolingo!!!", startPage);


    QLabel *imageLabel = new QLabel(this);
    QPixmap pixmap("/home/nikita/Duolingo/media/landscape-lockup.svg"); // Путь к изображению
    imageLabel->setPixmap(pixmap);
    imageLabel->setAlignment(Qt::AlignCenter);
    imageLabel->setPixmap(pixmap.scaled({this->size().height() - 100, this->size().width() - 100}, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    main_layout->addWidget(imageLabel);

    welcome_label->setAlignment(Qt::AlignCenter);
    welcome_label->setStyleSheet("font-size: 28pt; font-weight: bold; color: white; margin-bottom: 20px;text-align: center;");
    main_layout->addWidget(welcome_label);
    startPage->setLayout(main_layout);
    startPage->setStyleSheet("background-color: #3b3535;");
    translationPage->setStyleSheet("background-color: #3b3535;");

    windowStack = new QStackedWidget(this);
    windowStack->addWidget(startPage);
    windowStack->addWidget(translationPage);
    windowStack->addWidget(writingPage);

    setCentralWidget(windowStack);
}

void MainWindow::setupMenu(){
    startMenu = menuBar()->addMenu("Упражнения");
    auto main = new QAction("MainPage", this);
    auto translation = new QAction("Translation", this);
    auto writing = new QAction("Writing", this);
    startMenu->addAction(main);
    startMenu->addAction(translation);
    startMenu->addAction(writing);
    connect(main, &QAction::triggered, this, &MainWindow::ChooseMain);
    connect(translation, &QAction::triggered, this, &MainWindow::ChooseTranslation);
    connect(writing, &QAction::triggered, this, &MainWindow::ChooseWriting);
}

void MainWindow::ChooseMain(){
    windowStack->setCurrentWidget(startPage);
}

void MainWindow::ChooseTranslation(){
    translationPage->SetMode(Mode::TRANSLATION);
    windowStack->setCurrentWidget(translationPage);
}

void MainWindow::ChooseWriting(){
    translationPage->SetMode(Mode::WRITING);
    windowStack->setCurrentWidget(translationPage);
}



void MainWindow::GetResults(){
    ChooseMain();
    if(translationPage->GetRightAnswers() != -1){
        if(translationResultText == nullptr){
            translationResultText = new QLabel("Your current score in translation is " + QString::number(translationPage->GetRightAnswers()) + "/" + QString::number(translationPage->GetQuestionsQuantity()));
            translationResultText->setStyleSheet("font-size: 28pt; font-weight: bold; color: white; margin-bottom: 7px;text-align: center;");
            translationResultText->setAlignment(Qt::AlignCenter);
            main_layout->addWidget(translationResultText);
        } else{
            translationResultText->setText("Your current score in translation is " + QString::number(translationPage->GetRightAnswers()) + "/" + QString::number(translationPage->GetQuestionsQuantity()));
        }

    }

}
