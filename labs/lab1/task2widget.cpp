#include "task2widget.h"

#include <QButtonGroup>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QPixmap>
#include <QVBoxLayout>

Task2Widget::Task2Widget(QWidget* parent) : QWidget(parent) {
    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    QLabel* title = new QLabel("<b>🚀 Калькулятор шансов пережить сессию (ФПМИ Edition)</b>");
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 20px; color: #0d47a1; margin-bottom: 20px;");
    mainLayout->addWidget(title);

    QFormLayout* formLayout = new QFormLayout();

    missedMthClasses = new QSpinBox();
    missedMthClasses->setRange(0, 30);
    formLayout->addRow("Сколько пар по матану проспал?", missedMthClasses);

    QGroupBox* labGroup = new QGroupBox("Как делались лабы по проге?");
    QVBoxLayout* labLayout = new QVBoxLayout();
    didLabsMyself = new QRadioButton("Делал сам (не спал ночами)");
    didLabsMyself->setChecked(true);
    copiedFromGithub = new QRadioButton("Списывал с GitHub (git pull origin master)");
    labLayout->addWidget(didLabsMyself);
    labLayout->addWidget(copiedFromGithub);
    labGroup->setLayout(labLayout);
    formLayout->addRow(labGroup);

    stressSlider = new QSlider(Qt::Horizontal);
    stressSlider->setRange(0, 100);
    stressSlider->setValue(50);
    formLayout->addRow("Уровень накопленного стресса (0-100):", stressSlider);

    mainLayout->addLayout(formLayout);

    calculateButton = new QPushButton("Узнать свою судьбу");
    mainLayout->addWidget(calculateButton);

    expulsionChance = new QProgressBar();
    expulsionChance->setRange(0, 100);
    expulsionChance->setValue(0);
    expulsionChance->setFormat("Шанс отчисления: %p%");
    mainLayout->addWidget(expulsionChance);

    verdictText = new QTextEdit();
    verdictText->setReadOnly(true);
    verdictText->setPlaceholderText("Здесь будет вердикт деканата...");
    verdictText->setStyleSheet("background-color: #f8f9fa; border: 2px solid #dee2e6; border-radius: 8px; font-size: 24px; font-weight: bold;");
    mainLayout->addWidget(verdictText);

    catImageLabel = new QLabel();
    catImageLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(catImageLabel);

    connect(
        missedMthClasses, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &Task2Widget::updateChances);
    connect(stressSlider, &QSlider::valueChanged, this, &Task2Widget::updateChances);
    connect(didLabsMyself, &QRadioButton::toggled, this, &Task2Widget::updateChances);
    connect(calculateButton, &QPushButton::clicked, this, &Task2Widget::showVerdict);

    updateChances();
}

void Task2Widget::updateChances() {
    int chance = 0;
    chance += missedMthClasses->value() * 3;
    chance += stressSlider->value() / 4;

    if (copiedFromGithub->isChecked()) {
        chance += 30;
    }

    if (chance > 100) {
        chance = 100;
    }
    if (chance < 0) {
        chance = 0;
    }

    expulsionChance->setValue(chance);
}

void Task2Widget::showVerdict() {
    int chance = expulsionChance->value();
    QString verdict;

    if (chance == 0) {
        verdict = "Вероятность отчисления 0%! Время для классной вечеринки!";
        catImageLabel->setPixmap(
            QPixmap(":/labs/lab1/resources/cat_party.png")
                .scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (chance < 20) {
        verdict = "Невероятно! Ты либо гений, либо староста. Сессия будет сдана на изи.";
        catImageLabel->setPixmap(
            QPixmap(":/labs/lab1/resources/cat_studying.png")
                .scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (chance < 50) {
        verdict = "Шансы есть, но придется попотеть. Главное - не проспать экзамен.";
        catImageLabel->setPixmap(
            QPixmap(":/labs/lab1/resources/cat_coding.png")
                .scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else if (chance < 80) {
        verdict = "Демидович уже смотрит на тебя с осуждением. Срочно садись за учебники!";
        catImageLabel->setPixmap(
            QPixmap(":/labs/lab1/resources/cat_sleeping.png")
                .scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        verdict = "Земля пухом... Готовься к комиссии или сразу ищи военкомат на карте.";
        expulsionChance->setStyleSheet("QProgressBar::chunk {background-color: #dc3545;}");
        catImageLabel->setPixmap(
            QPixmap(":/labs/lab1/resources/cat_crying.png")
                .scaled(600, 600, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }

    if (chance < 80) {
        expulsionChance->setStyleSheet("QProgressBar::chunk {background-color: #007bff;}");
    }

    verdictText->setText(verdict);
}
