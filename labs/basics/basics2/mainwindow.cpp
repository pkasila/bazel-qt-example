#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <algorithm>

//NOLINTBEGIN

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui_ (new Ui::MainWindow)
    , widget_ (new QWidget)
    , label_ (new CustomLabel)
    , question_(new QLineEdit)
    , button_(new QPushButton("Click to reply"))
    , answer_(new Answer) {
    setCentralWidget(widget_);
    this->Action();
    options_ = menuBar()->addMenu(tr("&Options"));
    auto* action = new QAction("&Mode 1");
    auto* action2 = new QAction("&Mode 2");
    options_->addAction(action);
    options_->addAction(action2);
    connect(action, &QAction::triggered, this, &MainWindow::Action);
    connect(action2, &QAction::triggered, this, &MainWindow::Action2);
    setWindowTitle(tr("The Omniscient app"));
    setMinimumSize(160, 160);
    resize(960, 640);
}

void MainWindow::Action() {
    this->clear();
    widget_ = new QWidget;
    setCentralWidget(widget_);
    label_ = new CustomLabel;
    question_ = new QLineEdit;
    button_ = new QPushButton("Click to reply");
    answer_ = new Answer;
    QVBoxLayout* layout = new QVBoxLayout;
    QVBoxLayout* layout2 = new QVBoxLayout;
    label_->setText("Enter your question");
    label_->setMaximumHeight(40);
    label_->setAlignment(Qt::AlignCenter);
    answer_->resize(200,200);
    answer_->setStyleSheet("background-color: gray;");
    answer_->setAlignment(Qt::AlignCenter);
    layout2->addWidget(label_);
    layout2->addWidget(question_);
    layout2->addWidget(button_);
    layout->addLayout(layout2);
    layout->addWidget(answer_);
    widget_->setLayout(layout);
    // options_ = menuBar()->addMenu(tr("&Options"));
    // auto* action = new QAction("&Mode 1");
    // auto* action2 = new QAction("Mode 2");
    // options_->addAction(action);
    // options_->addAction(action2);
    connect(button_, &QPushButton::clicked, answer_, &Answer::GenerateAnswer);
    connect (button_, &QPushButton::clicked, label_, &CustomLabel::ChangeText1);
    connect (question_, &QLineEdit::editingFinished, label_, &CustomLabel::ChangeText2);
}

void MainWindow::Action2() {
    this->clear();
    widget_ = new QWidget;
    setCentralWidget(widget_);
    label2_ = new QLabel("Enter your numbers");
    label2_->setMaximumHeight(30);
    line2_ = new QLineEdit;
    line2_->setMinimumHeight(200);
    line2_->setAlignment(Qt::AlignLeft);
    line2_->setAlignment(Qt::AlignTop);
    combo_box2_ = new QComboBox;
    combo_box2_->addItem("sequence");
    combo_box2_->addItem("one number");
    button2_ = new QPushButton ("Click to randomize");
    sequence_ = new RandomField(this);
    sequence_->setMinimumHeight(200);
    sequence_->setStyleSheet("background-color: gray; color: white");
    sequence_->setAlignment(Qt::AlignCenter);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addWidget(label2_);
    layout->addWidget(line2_);
    layout->addWidget(combo_box2_);
    layout->addWidget(button2_);
    layout->addWidget(sequence_);
    widget_->setLayout(layout);
    connect (button2_, &QPushButton::clicked, sequence_, &RandomField::Random);
}

void RandomField::Random() {
    auto elements = parent_->Sequence()->text().split(" ");
    std::random_shuffle(elements.begin(), elements.end());
    if (parent_->Status()->currentText() == "sequence") {
        QString str = "";
        for (auto& c : elements) {
            str += c + " ";
        }
        this->setText(str);
    } else {
        this->setText(elements.first());
    }
}

MainWindow::~MainWindow()
{
    delete ui_;
    ui_ = nullptr;
}

void MainWindow::clear() {
    delete label_;
    label_ = nullptr;
    delete question_;
    question_= nullptr;
    delete button_;
    button_= nullptr;
    delete answer_;
    answer_= nullptr;
    delete combo_box2_;
    combo_box2_= nullptr;
    delete label2_;
    label2_ = nullptr;
    delete line2_;
    line2_= nullptr;
    delete sequence_;
    sequence_= nullptr;
    delete button2_;
    button2_ = nullptr;
}

void Answer::GenerateAnswer() {
    std::srand(std::time(0));
    int i = std::rand() % 2;
    if (i == 0) {
        this->setStyleSheet("background-color: green; color: white; font-size: 30pt");
        this->setText("Yes");
    } else {
        this->setStyleSheet("background-color: red; color: white; font-size: 30pt");
        this->setText("No");
    }
}

void CustomLabel::ChangeText1() {
    this->setText("Enter your question");
}

void CustomLabel::ChangeText2() {
    this->setText("Let's find out the answer");
}

//NOLINTEND
