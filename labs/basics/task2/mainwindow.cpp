#include "mainwindow.h"
#include "Results.h"

#include <QWidget>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QLineEdit>
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{


    setWindowTitle("BSU Admission");


    QWidget *centralWidget = new QWidget(this);


    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    groupBox = new QGroupBox("Personal info", this);
    QGridLayout *groupBoxLayout = new QGridLayout(groupBox);


    label1 = new QLabel("Your name : ", groupBox);
    groupBoxLayout->addWidget(label1, 0, 0);

    label2 = new QLabel("BEL/RUS : ", groupBox);
    groupBoxLayout->addWidget(label2, 1, 0);

    label3 = new QLabel("MATH : ", groupBox);
    groupBoxLayout->addWidget(label3, 2, 0);

    label4 = new QLabel("PHYS : ", groupBox);
    groupBoxLayout->addWidget(label4, 3, 0);

    label5 = new QLabel("GPA : ", groupBox);
    groupBoxLayout->addWidget(label5, 4, 0);

    textEdit = new QTextEdit(groupBox);
    connect(textEdit, &QTextEdit::textChanged, this, &MainWindow::changeName);
    textEdit->setFixedHeight(textEdit->fontMetrics().height() + 10);
    groupBoxLayout->addWidget(textEdit, 0, 1);

    spinBox1 = new QSpinBox(groupBox);
    spinBox1->setMinimum(30);
    spinBox1->setMaximum(100);
    groupBoxLayout->addWidget(spinBox1, 1, 1);
    connect(spinBox1, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSpinBox1ValueChanged);


    spinBox2 = new QSpinBox(groupBox);
    spinBox2->setMinimum(30);
    spinBox2->setMaximum(100);
    connect(spinBox2, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSpinBox2ValueChanged);
    groupBoxLayout->addWidget(spinBox2, 2, 1);


    spinBox3 = new QSpinBox(groupBox);
    spinBox3->setMinimum(30);
    spinBox3->setMaximum(100);
    connect(spinBox3, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSpinBox3ValueChanged);
    groupBoxLayout->addWidget(spinBox3, 3, 1);

    spinBox4 = new QSpinBox(groupBox);
    spinBox4->setMinimum(30);
    spinBox4->setMaximum(100);
    connect(spinBox4, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSpinBox4ValueChanged);
    groupBoxLayout->addWidget(spinBox4, 4, 1);

    layout->addWidget(groupBox);


    checkBox = new QCheckBox("I am a BSU Lyceum graduate / an olympiad winner! ", this);
    connect(checkBox, &QCheckBox::stateChanged, this, &MainWindow::giveBenefits);
    layout->addWidget(checkBox);


    radioButton1 = new QRadioButton("FAMCS!", this);
    radioButton2 = new QRadioButton("PhF!", this);
    radioButton3 = new QRadioButton("MMF!", this);
    radioButton4 = new QRadioButton("FRCT!", this);

    connect(radioButton1, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
    connect(radioButton2, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
    connect(radioButton3, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);
    connect(radioButton4, &QRadioButton::clicked, this, &MainWindow::onRadioButtonClicked);

    radioGroup = new QButtonGroup(this);
    QHBoxLayout *radioLayout = new QHBoxLayout();
    radioGroup->addButton(radioButton1);
    radioGroup->addButton(radioButton2);
    radioGroup->addButton(radioButton3);
    radioGroup->addButton(radioButton4);

    radioLayout->addWidget(radioButton1);
    radioLayout->addWidget(radioButton2);
    radioLayout->addWidget(radioButton3);
    radioLayout->addWidget(radioButton4);

    layout->addLayout(radioLayout);

    centralWidget->setLayout(layout);

    resultsButton = new QPushButton("Get the results!", this);
    resultsButton->setEnabled(false);
    connect(resultsButton, &QPushButton::clicked, this, &MainWindow::showResultsDialog);
    layout->addWidget(resultsButton);

    setCentralWidget(centralWidget);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::changeName() {
    QString newName = textEdit->toPlainText();
    MainWindow::name = newName;

    if (!name.isEmpty() && facultyChosen) {
        resultsButton->setEnabled(true);
    }

    if (name.isEmpty()) {
        resultsButton->setEnabled(false);
    }
}

void MainWindow::onRadioButtonClicked() {
    MainWindow::facultyChosen = true;
    if (!name.isEmpty() && facultyChosen) {
        resultsButton->setEnabled(true);
    }

    if (radioButton1->isChecked()) {
        facultyPicked = 0;
    } else if (radioButton2->isChecked()) {
        facultyPicked = 1;
    } else if (radioButton3->isChecked()) {
        facultyPicked = 2;
    } else if (radioButton4->isChecked()) {
        facultyPicked = 3;
    }
}

void MainWindow::onSpinBox1ValueChanged(int newValue) {
    MainWindow::langPoints = newValue;
}

void MainWindow::onSpinBox2ValueChanged(int newValue) {
    MainWindow::mathPoints = newValue;
}

void MainWindow::onSpinBox3ValueChanged(int newValue) {
    MainWindow::physPoints = newValue;
}

void MainWindow::onSpinBox4ValueChanged(int newValue) {
    MainWindow::gpaPoints = newValue;
}

void MainWindow::giveBenefits(int val) {
    hasBenefits = (val == Qt::Checked);
}

void MainWindow::showResultsDialog() {
    Results *resultsDialog = new Results(this);
    resultsDialog->setData(name, facultyPicked, langPoints, mathPoints, physPoints, gpaPoints, hasBenefits);

    resultsDialog->exec();

}


