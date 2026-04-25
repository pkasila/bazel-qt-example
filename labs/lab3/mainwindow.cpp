#include "mainwindow.h"
#include <QMessageBox>
#include <QKeyEvent>
#include <QMenuBar>
#include <QRadioButton>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    
    // Инициализация вопросов
    questions = {
        {false, QString::fromUtf8("Translate: Apple"), QString::fromUtf8("Яблоко"), {}, QString::fromUtf8("Это фрукт, бывает зеленым или красным")},
        {true, QString::fromUtf8("I ___ a student."), QString::fromUtf8("am"), {QString::fromUtf8("is"), QString::fromUtf8("am"), QString::fromUtf8("are")}, QString::fromUtf8("Используйте форму глагола to be для 1-го лица")},
        {false, QString::fromUtf8("Translate: Library"), QString::fromUtf8("Библиотека"), {}, QString::fromUtf8("Место, где много книг")},
        {true, QString::fromUtf8("She ___ like milk."), QString::fromUtf8("doesn't"), {QString::fromUtf8("don't"), QString::fromUtf8("doesn't"), QString::fromUtf8("isn't")}, QString::fromUtf8("Отрицание в Present Simple для 3-го лица")},
        {false, QString::fromUtf8("Translate: Computer"), QString::fromUtf8("Компьютер"), {}, QString::fromUtf8("Устройство для вычислений")}
    };

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
}

void MainWindow::setupUI() {
    setWindowTitle(QString::fromUtf8("Изучение языков - Лаб 3"));
    resize(850, 550);

    setStyleSheet(R"(
        QMainWindow { background-color: #2b2b2b; }
        QLabel { color: #e0e0e0; font-size: 15px; }
        QPushButton { 
            background-color: #4a90e2; color: white; border-radius: 5px; 
            padding: 10px; font-weight: bold; font-size: 14px;
        }
        QPushButton:hover { background-color: #357abd; }
        QLineEdit { 
            background-color: #3c3f41; color: white; border: 1px solid #555; 
            padding: 8px; border-radius: 4px; font-size: 16px;
        }
        QProgressBar { 
            border: 1px solid #555; border-radius: 5px; text-align: center; color: white; 
        }
        QProgressBar::chunk { background-color: #27ae60; }
        QRadioButton { color: #e0e0e0; font-size: 14px; padding: 5px; }
    )");

    auto *central = new QWidget();
    auto *mainLayout = new QHBoxLayout(central);

    auto *side = new QVBoxLayout();
    auto *btnStart = new QPushButton(QString::fromUtf8("Начать упражнение"));
    scoreLabel = new QLabel(QString::fromUtf8("Баллы: 0"));
    timerLabel = new QLabel(QString::fromUtf8("Время: 00:00"));
    progress = new QProgressBar();
    progress->setRange(0, 5);

    side->addWidget(btnStart);
    side->addSpacing(20);
    side->addWidget(new QLabel(QString::fromUtf8("Ваш прогресс:")));
    side->addWidget(progress);
    side->addStretch();
    side->addWidget(scoreLabel);
    side->addWidget(timerLabel);

    stack = new QStackedWidget();
    auto *welcome = new QLabel(QString::fromUtf8("Нажмите 'Начать упражнение' слева\nКлавиша 'H' - подсказка"));
    welcome->setAlignment(Qt::AlignCenter);
    stack->addWidget(welcome);

    auto *page = new QWidget();
    auto *pLayout = new QVBoxLayout(page);
    qLabel = new QLabel("");
    qLabel->setStyleSheet("font-size: 22px; color: #f1c40f; font-weight: bold; margin-bottom: 20px;");
    
    transInput = new QLineEdit();
    optWidget = new QWidget();
    optionsLayout = new QVBoxLayout(optWidget);
    gramGroup = new QButtonGroup(this);

    auto *btnSubmit = new QPushButton(QString::fromUtf8("Проверить ответ (Submit)"));
    btnSubmit->setMinimumHeight(45);

    pLayout->addWidget(qLabel);
    pLayout->addWidget(transInput);
    pLayout->addWidget(optWidget);
    pLayout->addStretch();
    pLayout->addWidget(btnSubmit);

    stack->addWidget(page);

    mainLayout->addLayout(side, 1);
    mainLayout->addWidget(stack, 3);
    setCentralWidget(central);

    menuBar()->addMenu(QString::fromUtf8("Настройки"))->addAction(QString::fromUtf8("Сложность"), this, &MainWindow::changeDifficulty);

    connect(btnStart, &QPushButton::clicked, this, &MainWindow::startExercise);
    connect(btnSubmit, &QPushButton::clicked, this, &MainWindow::checkAnswer);
}

void MainWindow::startExercise() {
    currentIdx = 0; mistakes = 0; timeLeft = 60; score = 0;
    scoreLabel->setText(QString::fromUtf8("Баллы: 0"));
    progress->setValue(0);
    stack->setCurrentIndex(1);
    showNext();
    timer->start(1000);
}

void MainWindow::showNext() {
    if (currentIdx >= questions.size()) {
        finish(QString::fromUtf8("Упражнение завершено!"), true);
        return;
    }

    progress->setValue(currentIdx);
    const auto &q = questions[currentIdx];
    qLabel->setText(q.text);

    if (!q.isGrammar) {
        transInput->show();
        transInput->clear();
        transInput->setFocus();
        optWidget->hide();
    } else {
        transInput->hide();
        optWidget->show();
        for(auto *b : gramGroup->buttons()) { gramGroup->removeButton(b); b->deleteLater(); }
        for(const auto &o : q.options) {
            auto *rb = new QRadioButton(o);
            optionsLayout->addWidget(rb);
            gramGroup->addButton(rb);
        }
    }
}

void MainWindow::checkAnswer() {
    const auto &q = questions[currentIdx];
    bool isCorrect = false;

    if (!q.isGrammar) {
        isCorrect = (transInput->text().trimmed().toLower() == q.answer.toLower());
    } else {
        if (gramGroup->checkedButton())
            isCorrect = (gramGroup->checkedButton()->text() == q.answer);
    }

    if (isCorrect) {
        currentIdx++;
        showNext();
    } else {
        mistakes++;
        if (mistakes >= 3) {
            finish(QString::fromUtf8("Слишком много ошибок! Упражнение прервано."), false);
        } else {
            QMessageBox::warning(this, QString::fromUtf8("Ошибка"), 
                QString::fromUtf8("Неверно. Осталось попыток: %1").arg(3 - mistakes));
        }
    }
}

void MainWindow::updateTimer() {
    timeLeft--;
    timerLabel->setText(QString::fromUtf8("Время: 00:%1").arg(timeLeft, 2, 10, QChar('0')));
    if (timeLeft <= 0) {
        finish(QString::fromUtf8("Время истекло!"), false);
    }
}

void MainWindow::finish(const QString &msg, bool success) {
    timer->stop();
    score = success ? 10 : 0;
    scoreLabel->setText(QString::fromUtf8("Баллы: %1").arg(score));
    QMessageBox::information(this, QString::fromUtf8("Результат"), msg + QString::fromUtf8("\nВаши баллы: %1").arg(score));
    stack->setCurrentIndex(0);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H && stack->currentIndex() == 1) {
        if (currentIdx < questions.size())
            QMessageBox::information(this, QString::fromUtf8("Подсказка"), questions[currentIdx].hint);
    }
}

void MainWindow::changeDifficulty() {
    QMessageBox::information(this, QString::fromUtf8("Сложность"), QString::fromUtf8("Уровень сложности изменен"));
}