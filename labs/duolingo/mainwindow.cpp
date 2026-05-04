#include "mainwindow.h"
#include "difficultydialog.h"
#include <QMenuBar>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QKeyEvent>
#include <QApplication>
#include <algorithm>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUi();
    applyTheme();
    
    exerciseTimer = new QTimer(this);
    connect(exerciseTimer, &QTimer::timeout, this, &MainWindow::timeOut);
    
    soundEffect = new QSoundEffect(this);
}

void MainWindow::setupTasks() {
    // Задания для перевода
    translationTasks = {
        {"The shipment arrives at midnight.", "Груз прибудет в полночь", {}, "Проверь доки. Полночь - это midnight."},
        {"Hide the briefcase under the seat.", "Спрячь портфель под сиденьем", {}, "Briefcase - это портфель."}
        // Добавь остальные...
    };

    // Задания для грамматики
    grammarTasks = {
        {"Yesterday, the boss ___ me to the warehouse.", "sent", {"send", "sent", "sending"}, "Past Simple: send -> sent"},
        {"There is ___ unmarked van outside.", "an", {"a", "an", "the"}, "Используй 'an' перед гласными."}
        // Добавь остальные...
    };
}

void MainWindow::setupUi() {
    resize(800, 600);
    setWindowTitle("Vice City Lingo");

    QMenu *settingsMenu = menuBar()->addMenu("Options");
    QAction *diffAction = settingsMenu->addAction("Turf Difficulty");
    connect(diffAction, &QAction::triggered, this, &MainWindow::changeDifficulty);

    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    // --- СТРАНИЦА МЕНЮ ---
    menuPage = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuPage);
    
    QLabel *title = new QLabel("VICE CITY LINGO", this);
    title->setAlignment(Qt::AlignCenter);
    title->setObjectName("titleLabel");
    
    scoreLabel = new QLabel("Street Cred: 0", this);
    scoreLabel->setAlignment(Qt::AlignCenter);

    QPushButton *btnTrans = new QPushButton("Wiretap (Translation)", this);
    QPushButton *btnGramm = new QPushButton("Street Slang (Grammar)", this);
    connect(btnTrans, &QPushButton::clicked, this, &MainWindow::startTranslation);
    connect(btnGramm, &QPushButton::clicked, this, &MainWindow::startGrammar);

    menuLayout->addStretch();
    menuLayout->addWidget(title);
    menuLayout->addWidget(scoreLabel);
    menuLayout->addWidget(btnTrans);
    menuLayout->addWidget(btnGramm);
    menuLayout->addStretch();

    // --- ОБЩАЯ ПАНЕЛЬ ДЛЯ УПРАЖНЕНИЙ ---
    progressBar = new QProgressBar(this);
    timerLabel = new QLabel("Time: 00", this);
    timerLabel->setObjectName("timerLabel");

    // --- СТРАНИЦА ПЕРЕВОДА ---
    translationPage = new QWidget();
    QVBoxLayout *transLayout = new QVBoxLayout(translationPage);
    transWordLabel = new QLabel("Word", this);
    transWordLabel->setAlignment(Qt::AlignCenter);
    transInput = new QTextEdit(this);
    transInput->setMaximumHeight(50);
    QPushButton *btnTransSubmit = new QPushButton("Push", this);
    connect(btnTransSubmit, &QPushButton::clicked, this, &MainWindow::checkTranslation);
    
    transLayout->addWidget(progressBar);
    transLayout->addWidget(timerLabel);
    transLayout->addWidget(transWordLabel);
    transLayout->addWidget(transInput);
    transLayout->addWidget(btnTransSubmit);

    // --- СТРАНИЦА ГРАММАТИКИ ---
    grammarPage = new QWidget();
    QVBoxLayout *grammLayout = new QVBoxLayout(grammarPage);
    grammQuestionLabel = new QLabel("Question", this);
    radioGroup = new QButtonGroup(this);
    QWidget *radioContainer = new QWidget(this);
    radioLayout = new QVBoxLayout(radioContainer);
    
    QPushButton *btnGrammSubmit = new QPushButton("Push", this);
    connect(btnGrammSubmit, &QPushButton::clicked, this, &MainWindow::checkGrammar);

    grammLayout->addWidget(progressBar);
    grammLayout->addWidget(timerLabel);
    grammLayout->addWidget(grammQuestionLabel);
    grammLayout->addWidget(radioContainer);
    grammLayout->addWidget(btnGrammSubmit);

    stackedWidget->addWidget(menuPage);
    stackedWidget->addWidget(translationPage);
    stackedWidget->addWidget(grammarPage);
}

void MainWindow::applyTheme() {
    // QSS: Неоновый стиль 80-х
    QString qss = R"(
        QMainWindow, QWidget { background-color: #0d0e15; color: #00f0ff; font-family: 'Courier New', monospace; font-size: 16px; }
        QLabel#titleLabel { font-size: 36px; color: #ff007f; font-weight: bold; }
        QLabel#timerLabel { color: #ffeb3b; font-weight: bold; }
        QPushButton { background-color: #1a1c29; border: 2px solid #ff007f; border-radius: 5px; padding: 10px; color: #ff007f; font-weight: bold; }
        QPushButton:hover { background-color: #ff007f; color: #0d0e15; }
        QTextEdit { background-color: #1a1c29; border: 1px solid #00f0ff; color: #fff; }
        QProgressBar { border: 2px solid #00f0ff; text-align: center; color: white; }
        QProgressBar::chunk { background-color: #ff007f; }
        QMenuBar { background-color: #0d0e15; color: #00f0ff; }
        QMenuBar::item:selected { background-color: #ff007f; color: #0d0e15; }
    )";
    qApp->setStyleSheet(qss);
}

void MainWindow::changeDifficulty() {
    DifficultyDialog dlg(difficulty, this);
    if (dlg.exec() == QDialog::Accepted) {
        difficulty = dlg.getSelectedLevel();
    }
}

void MainWindow::updateTranslationUI() {
    if (currentTask < translationTasks.size()) {
        transWordLabel->setText(translationTasks[currentTask].text);
        currentHint = translationTasks[currentTask].hint;
        transInput->clear();
    }
}

void MainWindow::startTranslation() {
    currentTask = 0;
    totalTasks = translationTasks.size();

    strikes = 0;
    timeLeft = 30 / difficulty; // Чем сложнее, тем меньше времени
    progressBar->setMaximum(totalTasks);
    progressBar->setValue(0);
    
    transWordLabel->setText("Переведи: Police"); // В реальном проекте здесь будет выборка из массива
    transInput->clear();
    currentHint = "Подсказка: Служители закона. Начинается на P.";
    
    stackedWidget->setCurrentWidget(translationPage);
    exerciseTimer->start(1000);

    updateTranslationUI();
    stackedWidget->setCurrentWidget(translationPage);
}

void MainWindow::startGrammar() {
    currentTask = 0;
    strikes = 0;
    timeLeft = 40 / difficulty;
    progressBar->setMaximum(totalTasks);
    progressBar->setValue(0);
    
    grammQuestionLabel->setText("I ___ the money yesterday.");
    
    // Очистка старых радиокнопок
    QLayoutItem *child;
    while ((child = radioLayout->takeAt(0)) != nullptr) {
        delete child->widget();
        delete child;
    }
    
    QStringList options = {"takes", "took", "taken"};
    for (int i = 0; i < options.size(); ++i) {
        QRadioButton *rb = new QRadioButton(options[i], this);
        radioGroup->addButton(rb, i);
        radioLayout->addWidget(rb);
    }
    
    currentHint = "Подсказка: Yesterday указывает на Past Simple.";
    stackedWidget->setCurrentWidget(grammarPage);
    exerciseTimer->start(1000);
}

// Расстояние Левенштейна: прощает опечатки (advanced feature)
int MainWindow::calculateLevenshteinDistance(const QString &s1, const QString &s2) {
    int len1 = s1.length();
    int len2 = s2.length();
    std::vector<std::vector<int>> d(len1 + 1, std::vector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1].toLower() == s2[j - 1].toLower()) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
        }
    }
    return d[len1][len2];
}

void MainWindow::checkTranslation() {
    QString answer = transInput->toPlainText().trimmed();
    QString correct = "полиция"; // В реальном проекте - из БД
    
    if (calculateLevenshteinDistance(answer, correct) <= 1) { // Допускаем 1 опечатку
        currentTask++;

        progressBar->setValue(currentTask);
        soundEffect->play();
        
        if (currentTask >= totalTasks) {
            endExercise(true, "Job's done. You earned respect.");
        } else {
            transInput->clear();
            transWordLabel->setText("Переведи слово №" + QString::number(currentTask + 1));
        }
    } else {
        strikes++;
        if (strikes >= MAX_STRIKES) {
            endExercise(false, "Busted! Too many strikes.");
        } else {
            QMessageBox::warning(this, "Strike!", "Wrong wire. Strikes: " + QString::number(strikes) + "/" + QString::number(MAX_STRIKES));
        }
    }
}

void MainWindow::checkGrammar() {
    if (radioGroup->checkedId() == 1) { // Индекс правильного ответа "took"
        currentTask++;
        progressBar->setValue(currentTask);
        soundEffect->play();
        
        if (currentTask >= totalTasks) {
            endExercise(true, "Smooth talker. Respect+");
        } else {
            grammQuestionLabel->setText("Вопрос №" + QString::number(currentTask + 1));
        }
    } else {
        strikes++;
        if (strikes >= MAX_STRIKES) {
            endExercise(false, "Busted! Too many strikes.");
        } else {
            QMessageBox::warning(this, "Strike!", "Wrong word, pal.");
        }
    }
}

void MainWindow::timeOut() {
    timeLeft--;
    timerLabel->setText("Time: " + QString::number(timeLeft));
    if (timeLeft <= 0) {
        endExercise(false, "Time is up! The cops are here.");
    }
}

void MainWindow::endExercise(bool success, const QString &reason) {
    exerciseTimer->stop();
    if (success) {
        streetCred += 10 * difficulty;
        scoreLabel->setText("Street Cred: " + QString::number(streetCred));
    }
    QMessageBox::information(this, success ? "Clear" : "Busted", reason);
    stackedWidget->setCurrentWidget(menuPage);
}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H && stackedWidget->currentWidget() != menuPage) {
        QMessageBox::information(this, "Intel (Help)", currentHint);
    }
    QMainWindow::keyPressEvent(event);
}