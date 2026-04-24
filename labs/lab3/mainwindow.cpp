#include <QCoreApplication>
#include <QUrl>
#include "mainwindow.h"
#include "level_dialog.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    initData();
    setupUI();

    successPlayer = new QMediaPlayer(this);
    successOutput = new QAudioOutput(this);
    successPlayer->setAudioOutput(successOutput);

    failurePlayer = new QMediaPlayer(this);
    failureOutput = new QAudioOutput(this);
    failurePlayer->setAudioOutput(failureOutput);

    successOutput->setVolume(1.0); 
    failureOutput->setVolume(1.0);

    successPlayer->setSource(QUrl::fromLocalFile("labs/lab3/success.wav"));
    failurePlayer->setSource(QUrl::fromLocalFile("labs/lab3/error.wav"));
}

MainWindow::~MainWindow() {}

void MainWindow::initData() {
    translationTasks = {
        {"Translate 'Heritage'", "Спадчына", "Title of a famous poem by Janka Kupala"},
        {"Translate 'Dreams/Fantasies'", "Мроі", "Poetic word for dreams or illusions"},
        {"Translate 'Storm cloud'", "Хмара", "A dark, heavy cloud before the rain"},
        {"Translate 'Journey/Trip'", "Вандроўка", "Synonym for traveling or wandering"},
        {"Translate 'Window pane'", "Шыба", "Distinct Belarusian word for window glass"},
        {"Translate 'Romantic love'", "Каханне", "In Belarusian, 'lubov' is for things, this is for people"},
        {"Translate 'Paradise (for birds)'", "Вырай", "The mythical place where birds fly for winter"},
        {"Translate 'Hat'", "Капялюш", "Starts with Капя..."},
        {"Translate 'To glitter/shimmer'", "Блішчаць", "Like stars or water in the sun"},
        {"Translate 'Butterfly'", "Матылёк", "A beautiful winged insect"},
        {"Translate 'Socks'", "Шкарпэткі", "Only in plural!!"},
        {"Translate 'Heel'", "Абцас", "Just one!!"}

    };

    grammarTasks = {
        {"Прабачце (me)", {"мяня", "мне", "мяне"}, 1, "U-skladovaje (ў) is used after vowels"},
        {"Я (to eat) смачны дранік.", {"ясні", "ем", "яду"}, 1, "Irregular verb 'есці' (1st person)"},
        {"Мы жадаем вам (Happiness).", {"шчасця", "шчасце", "шчасцю"}, 0, "Genitive case after 'жадаць'"},
        {"Кніга ляжыць на (Table).", { "стол", "стале", "сталу"}, 1, "Locative case (месны склон)"},
        {"Дзякуй (You) за дапамогу.", {"цябе", "ты", "табе"}, 2, "Dative case after 'Дзякуй'"},
        {"Яна (to go) у краму.", {"ідзе", "іду", "ідзем"}, 0, "3rd person singular"},
        {"Матуля (to cook) вячэру.", {"гатую", "гатуюць", "гатуе"}, 2, "Verb 'гатаваць'"},
        {"Сёння (Warm) надвор'е.", { "цёплы","цёплае", "цепла"}, 1, "Neuter adjective for 'надвор'е'"},
        {"У (Garden) растуць кветкі.", {"садзе", "саду", "садам"}, 0, "Locative case for 'сад'"},
        {"Хлопчык (to look) у вакно.", {"глядзіць", "глядзяць", "гляжу"}, 0, "3rd person singular"},
        {"Дзядзька (to talk) з братамі ўчора.", {"гутарка", "гутарыць", "гутарыў"}, 2, "3rd person singular past simple"},
        {"Вучні здзекаваліся .. дзяўчат.", {"над", "з", "пра"}, 1, "3rd person singular"}
    };
}

void MainWindow::setupUI() {
    stackedWidget = new QStackedWidget(this);
    setCentralWidget(stackedWidget);

    menuPage = new QWidget();
    QVBoxLayout *menuLayout = new QVBoxLayout(menuPage);

    QHBoxLayout *topRowLayout = new QHBoxLayout();
    QPushButton *btnSettings = new QPushButton("Settings", menuPage);
    btnSettings->setMinimumSize(100, 40);
    topRowLayout->addStretch();
    topRowLayout->addWidget(btnSettings);
    menuLayout->addLayout(topRowLayout);
    btnSettings->setStyleSheet("font-weight: 20px;");

    QLabel *title = new QLabel("Learn Belarusian", menuPage);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 35px; font-weight: bold; color: #d32f2f;");

    QPushButton *btnTrans = new QPushButton("Translation Practice", menuPage);
    QPushButton *btnGrammar = new QPushButton("Grammar Practice", menuPage);
    btnTrans->setMinimumHeight(70);
    btnGrammar->setMinimumHeight(70);
    btnTrans->setStyleSheet("font-size: 20px; font-weight: 20px;");
    btnGrammar->setStyleSheet("font-size: 20px; font-weight: 20px;");

    menuLayout->addStretch();
    menuLayout->addWidget(title);
    menuLayout->addSpacing(30);
    menuLayout->addWidget(btnTrans);
    menuLayout->addWidget(btnGrammar);
    menuLayout->addStretch();

    exercisePage = new QWidget();
    QVBoxLayout *exLayout = new QVBoxLayout(exercisePage);
    QHBoxLayout *statLayout = new QHBoxLayout();
    scoreLabel = new QLabel("Score: 0", this);
    heartsLabel = new QLabel("Hearts: 3", this);
    timerLabel = new QLabel("Time: 0s", this);
    statLayout->addWidget(scoreLabel);
    statLayout->addWidget(heartsLabel);
    statLayout->addWidget(timerLabel);

    progressBar = new QProgressBar(this);
    taskLabel = new QLabel("", this);
    taskLabel->setWordWrap(true);
    taskLabel->setStyleSheet("font-size: 18px; margin: 20px;");

    translationEdit = new QTextEdit(this);
    translationEdit->installEventFilter(this);

    grammarContainer = new QWidget(this);
    new QVBoxLayout(grammarContainer);
    grammarGroup = new QButtonGroup(this);

    QPushButton *submitBtn = new QPushButton("Submit", this);
    QPushButton *exitBtn = new QPushButton("Exit to Menu", this);

    exLayout->addLayout(statLayout);
    exLayout->addWidget(progressBar);
    exLayout->addWidget(taskLabel);
    exLayout->addWidget(translationEdit);
    exLayout->addWidget(grammarContainer);
    exLayout->addStretch();
    exLayout->addWidget(submitBtn);
    exLayout->addWidget(exitBtn);

    stackedWidget->addWidget(menuPage);
    stackedWidget->addWidget(exercisePage);

    sessionTimer = new QTimer(this);

    connect(btnTrans, &QPushButton::clicked, this, [this](){ startTranslationMode(); });
    connect(btnGrammar, &QPushButton::clicked, this, [this](){ startGrammarMode(); });
    connect(btnSettings, &QPushButton::clicked, this, [this](){ showDifficultyDialog(); });
    connect(submitBtn, &QPushButton::clicked, this, [this](){ submitAnswer(); });
    connect(exitBtn, &QPushButton::clicked, this, [this](){ backToMenu(); });
    connect(sessionTimer, &QTimer::timeout, this, [this](){ updateTimer(); });
    this->installEventFilter(this);
    
    resize(600, 700);
}

bool MainWindow::eventFilter(QObject *obj, QEvent *event) {
    if (event->type() == QEvent::KeyPress) {
        QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
        if (keyEvent->key() == Qt::Key_H) {
            if (stackedWidget->currentWidget() == exercisePage) {
                showHint();
                return true;
            }
        }
    }
    return QMainWindow::eventFilter(obj, event);
}

void MainWindow::showHint() {
    if (stackedWidget->currentWidget() == exercisePage) {
        QString hint = isTranslationMode ? translationTasks[currentTaskIdx].hint : grammarTasks[currentTaskIdx].hint;
        QMessageBox::information(this, "Hint", hint);
    }
}

void MainWindow::showDifficultyDialog() {
    LevelDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        currentDifficulty = dialog.getSelectedDifficulty();
        timerEnabled = dialog.isTimerEnabled();
    }
}

void MainWindow::resetSession() {
    hearts = 3;
    currentTaskIdx = 0;
    
    if (currentDifficulty == Difficulty::Easy) {
        timeLeft = 60;
        totalTasks = 5;
    }
    else if (currentDifficulty == Difficulty::Medium) {
        timeLeft = 45;
        totalTasks = 8;
    }
    else {
        timeLeft = 30;
        totalTasks = 12;
    }
    
    scoreLabel->setText(QString("Score: %1").arg(score));
    heartsLabel->setText(QString("Hearts: %1").arg(hearts));
    progressBar->setRange(0, totalTasks);
    progressBar->setValue(0);

    if (timerEnabled) {
        timerLabel->show();
        timerLabel->setText(QString("Time: %1s").arg(timeLeft));
        sessionTimer->start(1000); 
    }
    else {
        timerLabel->hide();
        sessionTimer->stop();
    }
}

void MainWindow::startTranslationMode() {
    isTranslationMode = true;
    resetSession();
    translationEdit->show();
    grammarContainer->hide();
    stackedWidget->setCurrentWidget(exercisePage);
    nextTask();
}

void MainWindow::startGrammarMode() {
    isTranslationMode = false;
    resetSession();
    translationEdit->hide();
    grammarContainer->show();
    stackedWidget->setCurrentWidget(exercisePage);
    nextTask();
}

void MainWindow::nextTask() {
    
    progressBar->setValue(currentTaskIdx);
    
    if (currentTaskIdx >= totalTasks) {
        score += 10;
        finishSession(true, "Great job!");
        return;
    }
    if (isTranslationMode) {
        taskLabel->setText(translationTasks[currentTaskIdx].question);
        translationEdit->clear();
        translationEdit->setFocus();
    } else {
        taskLabel->setText(grammarTasks[currentTaskIdx].question);
        qDeleteAll(grammarContainer->findChildren<QRadioButton*>());
        for (int i = 0; i < grammarTasks[currentTaskIdx].options.size(); ++i) {
            QRadioButton *rb = new QRadioButton(grammarTasks[currentTaskIdx].options[i], grammarContainer);
            grammarContainer->layout()->addWidget(rb);
            grammarGroup->addButton(rb, i);
        }
    }
}

void MainWindow::submitAnswer() {
    bool correct = false;
    bool typo = false;

    if (isTranslationMode) {
        QString userAns = translationEdit->toPlainText().trimmed().toLower();
        QString correctAns = translationTasks[currentTaskIdx].answer.toLower();
        
        int dist = calculateLevenshteinDistance(userAns, correctAns);

        if (dist == 0) {
            correct = true;
        } else if (dist == 1 && correctAns.length() > 3) {
            correct = true;
            typo = true;
        }
    } else {
        if (grammarGroup->checkedId() == grammarTasks[currentTaskIdx].correctIndex) correct = true;
    }

    if (correct) {
        successPlayer->stop(); 
        successPlayer->play(); 

        if (typo) {
            QMessageBox::information(this, "Typo", "Almost correct! Mind the spelling next time.");
        }
        currentTaskIdx++;
        nextTask();
    } else {
        failurePlayer->stop();
        failurePlayer->play();

        hearts--;
        heartsLabel->setText(QString("Hearts: %1").arg(hearts));
        if (hearts <= 0) finishSession(false, "Out of hearts!");
        else QMessageBox::warning(this, "Wrong", "Try again!");
    }
}

void MainWindow::updateTimer() {
    timeLeft--;
    timerLabel->setText(QString("Time: %1s").arg(timeLeft));
    if (timeLeft <= 0) finishSession(false, "Time's up!");
}

void MainWindow::finishSession(bool success, QString message) {
    sessionTimer->stop();
    if (success) QMessageBox::information(this, "Status", message);
    else QMessageBox::critical(this, "Status", message);
    backToMenu();
}

void MainWindow::backToMenu() {
    sessionTimer->stop();
    stackedWidget->setCurrentWidget(menuPage);
    scoreLabel->setText(QString("Score: %1").arg(score));
}

int MainWindow::calculateLevenshteinDistance(const QString &s1, const QString &s2) {
    int m = s1.length();
    int n = s2.length();
    QVector<QVector<int>> d(m + 1, QVector<int>(n + 1));
    for (int i = 0; i <= m; i++) d[i][0] = i;
    for (int j = 0; j <= n; j++) d[0][j] = j;
    for (int i = 1; i <= m; i++) {
        for (int j = 1; j <= n; j++) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
        }
    }
    return d[m][n];
}