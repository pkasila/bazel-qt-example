#include "mainwindow.h"

#include <QDebug>
#include <QFont>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QPixmap>
#include <QSqlError>
#include <QSqlQuery>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* p)
    : QMainWindow(p)
    , curExType_(ExType::None)
    , curTaskIdx_(-1)
    , completedTasks_(0)
    , curScore_(0)
    , remAttempts_(MAX_ATTEMPTS_PER_SERIES)
    , seriesTimeRemSec_(SERIES_TIME_SECONDS)
    , curDifficulty_(1) {
    initDb();

    stackedWidget_ = new QStackedWidget(this);
    progressBar_ = new QProgressBar(this);
    scoreLbl_ = new QLabel("Счёт: 0", this);
    attemptsLbl_ = new QLabel(QString("Попытки: %1").arg(MAX_ATTEMPTS_PER_SERIES), this);
    timerLbl_ = new QLabel(
        QString("Время: %1:%02d").arg(SERIES_TIME_SECONDS / 60).arg(SERIES_TIME_SECONDS % 60),
        this);
    seriesTimer_ = new QTimer(this);

    setupTopPanel();
    setupStartPage();
    setupPlaceholderPage();
    setupTransPage();
    setupGrammarPage();

    setupMainLayout();
    setupMenu();
    setupConnections();
    createDiffDialog();

    setWindowTitle("Duolingua");
    resize(800, 600);

    stackedWidget_->setCurrentWidget(startPage_);
    topPanel_->hide();
    progressBar_->hide();
    progressBar_->setRange(0, TASKS_PER_SERIES);
    progressBar_->setValue(0);
    progressBar_->setStyleSheet(
        "QProgressBar {"
        "   border: 2px solid grey;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "}"
        "QProgressBar::chunk { background-color: #4CAF50; width: 1px; }");
}

MainWindow::~MainWindow() {
    if (db_.isOpen()) {
        db_.close();
    }
}

void MainWindow::initDb() {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("exercises.db");
    if (!db_.open()) {
        QMessageBox::critical(this, "DB Error", "Failed to open DB: " + db_.lastError().text());
        return;
    }

    QSqlQuery q(db_);

    if (!q.exec("CREATE TABLE IF NOT EXISTS grammar_exercises ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "question TEXT NOT NULL, "
                "option1 TEXT NOT NULL, "
                "option2 TEXT NOT NULL, "
                "option3 TEXT NOT NULL, "
                "option4 TEXT NOT NULL, "
                "correct_option INTEGER NOT NULL, "
                "hint TEXT, "
                "difficulty INTEGER NOT NULL)")) {
        qWarning() << "Failed to create grammar_exercises table:" << q.lastError().text();
        return;
    }

    if (!q.exec("CREATE TABLE IF NOT EXISTS translation_exercises ("
                "id INTEGER PRIMARY KEY AUTOINCREMENT, "
                "native_text TEXT NOT NULL, "
                "target_text TEXT NOT NULL, "
                "hint TEXT, "
                "difficulty INTEGER NOT NULL)")) {
        qWarning() << "Failed to create translation_exercises table:" << q.lastError().text();
        return;
    }

    QSqlQuery chk_q(db_);

    chk_q.exec("SELECT COUNT(*) FROM grammar_exercises");
    if (chk_q.next() && chk_q.value(0).toInt() == 0) {
        qDebug() << "Populating grammar_exercises table...";
        q.prepare(
            "INSERT INTO grammar_exercises (question, option1, option2, option3, option4, "
            "correct_option, hint, difficulty) "
            "VALUES (?, ?, ?, ?, ?, ?, ?, ?)");
        QStringList data = {
          "I ___ a student.",
          "is",
          "are",
          "am",
          "be",
          "3",
          "Verb 'to be' for 'I'.",
          "1",
          "She ___ a doctor.",
          "is",
          "are",
          "am",
          "be",
          "1",
          "Verb 'to be' for singular third person.",
          "1",
          "They ___ friends.",
          "is",
          "are",
          "am",
          "be",
          "2",
          "Verb 'to be' for plural.",
          "1",
          "He ___ a cat.",
          "has",
          "have",
          "is",
          "are",
          "1",
          "'has' for he/she/it.",
          "1",
          "We ___ English.",
          "speak",
          "speaks",
          "speaking",
          "spoke",
          "1",
          "Simple Present for 'We'.",
          "1",
          "This is ___ apple.",
          "a",
          "an",
          "the",
          "some",
          "2",
          "Use 'an' before a vowel sound.",
          "1",
          "There ___ two books on the table.",
          "is",
          "are",
          "was",
          "am",
          "2",
          "'There are' for plural nouns.",
          "1",
          "My brother ___ TV every day.",
          "watch",
          "watches",
          "watching",
          "watched",
          "2",
          "Simple Present, 3rd person singular (-es).",
          "1",
          "Can you help ___?",
          "I",
          "my",
          "me",
          "mine",
          "3",
          "Object pronoun needed here.",
          "1",
          "The book is ___ the table.",
          "in",
          "on",
          "at",
          "under",
          "2",
          "Preposition of place.",
          "1",
          "They ___ football now.",
          "play",
          "plays",
          "are playing",
          "played",
          "3",
          "Present Continuous for an action happening now.",
          "2",
          "She ___ to Paris last year.",
          "go",
          "goes",
          "went",
          "has gone",
          "3",
          "Past Simple for a completed action in the past.",
          "2",
          "I ___ this movie before.",
          "see",
          "saw",
          "have seen",
          "am seeing",
          "3",
          "Present Perfect for experiences.",
          "2",
          "This car is ___ than that one.",
          "big",
          "bigger",
          "biggest",
          "more big",
          "2",
          "Comparative form of adjectives.",
          "2",
          "You ___ study for the exam.",
          "can",
          "may",
          "should",
          "would",
          "3",
          "Modal verb for advice.",
          "2",
          "There isn't ___ milk in the fridge.",
          "some",
          "any",
          "no",
          "a lot",
          "2",
          "'any' is used in negative sentences.",
          "2",
          "He ___ (visit) his grandparents next week.",
          "visit",
          "visits",
          "will visit",
          "visited",
          "3",
          "Future Simple for future plans.",
          "2",
          "While I ___ (read), the phone ___ (ring).",
          "read, ringed",
          "was reading, rang",
          "read, was ringing",
          "reading, rings",
          "2",
          "Past Continuous and Past Simple for interrupted actions.",
          "2",
          "She is interested ___ learning new languages.",
          "on",
          "at",
          "in",
          "for",
          "3",
          "Preposition 'in' after 'interested'.",
          "2",
          "How ___ students are in your class?",
          "much",
          "many",
          "a lot of",
          "some",
          "2",
          "'many' for countable nouns, 'much' for uncountable.",
          "2",
          "If I ___ you, I would apologize.",
          "am",
          "was",
          "were",
          "be",
          "3",
          "Second conditional, subjunctive mood 'were' for 'I'.",
          "3",
          "The work ___ by tomorrow morning.",
          "will finish",
          "will be finished",
          "finishes",
          "is finishing",
          "2",
          "Future Passive voice.",
          "3",
          "He said he ___ tired.",
          "is",
          "was",
          "has been",
          "will be",
          "2",
          "Reported speech: tense backshift.",
          "3",
          "She has been living here ___ 2010.",
          "since",
          "for",
          "ago",
          "from",
          "1",
          "Use 'since' with a specific point in time.",
          "3",
          "I'd rather you ___ make so much noise.",
          "don't",
          "didn't",
          "won't",
          "wouldn't",
          "2",
          "Structure 'would rather someone did something' (past tense for present preference).",
          "3",
          "By the time she arrived, we ___ dinner.",
          "had finished",
          "finished",
          "were finishing",
          "finish",
          "1",
          "Past Perfect for an action completed before another past action.",
          "3",
          "It's no use ___ about the spilt milk.",
          "to cry",
          "cry",
          "crying",
          "cried",
          "3",
          "Fixed phrase 'it's no use' + gerund (-ing form).",
          "3",
          "___ he studies hard, he will pass the exam.",
          "Unless",
          "If",
          "Although",
          "Despite",
          "2",
          "Conditional conjunction.",
          "3",
          "This is the man ___ dog bit me.",
          "who",
          "which",
          "whose",
          "whom",
          "3",
          "Possessive relative pronoun.",
          "3",
          "Not only ___ famous, but she is also very talented.",
          "she is",
          "is she",
          "she was",
          "was she",
          "2",
          "Inversion after 'Not only' at the beginning of a sentence.",
          "3"};
        for (int i = 0; i < data.size(); i += 8) {
            q.addBindValue(data[i]);
            q.addBindValue(data[i + 1]);
            q.addBindValue(data[i + 2]);
            q.addBindValue(data[i + 3]);
            q.addBindValue(data[i + 4]);
            q.addBindValue(data[i + 5].toInt());
            q.addBindValue(data[i + 6]);
            q.addBindValue(data[i + 7].toInt());
            q.exec();
        }
    }

    chk_q.exec("SELECT COUNT(*) FROM translation_exercises");
    if (chk_q.next() && chk_q.value(0).toInt() == 0) {
        qDebug() << "Populating translation_exercises table...";
        q.prepare(
            "INSERT INTO translation_exercises (native_text, target_text, hint, difficulty) VALUES "
            "(?, ?, ?, ?)");
        QStringList data = {
          "Привет",
          "Hello",
          "A common greeting.",
          "1",
          "Спасибо",
          "Thank you",
          "Expressing gratitude.",
          "1",
          "Да",
          "Yes",
          "Affirmative response.",
          "1",
          "Нет",
          "No",
          "Negative response.",
          "1",
          "Мой дом",
          "My house",
          "Possessive pronoun + noun.",
          "1",
          "Красный шар",
          "Red ball",
          "Adjective + noun.",
          "1",
          "Я вижу собаку",
          "I see a dog",
          "Simple sentence structure.",
          "1",
          "Это книга",
          "This is a book",
          "Demonstrative pronoun.",
          "1",
          "Доброе утро",
          "Good morning",
          "Morning greeting.",
          "1",
          "Как тебя зовут?",
          "What is your name?",
          "Asking for a name.",
          "1",
          "Я люблю читать книги",
          "I like to read books",
          "Verb 'like' + infinitive.",
          "2",
          "Она говорит по-английски очень хорошо",
          "She speaks English very well",
          "Adverb placement.",
          "2",
          "Мы ходили в кино вчера",
          "We went to the cinema yesterday",
          "Past Simple tense.",
          "2",
          "Где находится ближайший магазин?",
          "Where is the nearest shop?",
          "Asking for directions, superlative.",
          "2",
          "У меня есть брат и сестра",
          "I have a brother and a sister",
          "Family members.",
          "2",
          "Завтра будет солнечно",
          "It will be sunny tomorrow",
          "Future tense, weather.",
          "2",
          "Эта задача сложнее, чем предыдущая",
          "This task is more difficult than the previous one",
          "Comparative adjective.",
          "2",
          "Он всегда помогает своим друзьям",
          "He always helps his friends",
          "Adverb of frequency, possessive pronoun.",
          "2",
          "Не могли бы вы мне помочь?",
          "Could you help me, please?",
          "Polite request.",
          "2",
          "Я учусь в университете уже два года",
          "I have been studying at the university for two years",
          "Present Perfect Continuous.",
          "2",
          "Если бы я знал ответ, я бы тебе сказал",
          "If I knew the answer, I would tell you",
          "Second conditional.",
          "3",
          "Несмотря на плохую погоду, мы пошли на прогулку",
          "Despite the bad weather, we went for a walk",
          "Conjunction 'despite'.",
          "3",
          "Книга, которую я читаю, очень интересная",
          "The book that I am reading is very interesting",
          "Relative clause.",
          "3",
          "Ему пришлось ждать более часа",
          "He had to wait for more than an hour",
          "Modal 'had to' for necessity in the past.",
          "3",
          "Я бы предпочел остаться дома сегодня вечером",
          "I would prefer to stay home tonight",
          "'Would prefer' + infinitive.",
          "3",
          "Чем больше ты учишься, тем больше ты знаешь",
          "The more you study, the more you know",
          "Comparative construction 'The more... the more...'.",
          "3",
          "Говорят, что этот фильм стоит посмотреть",
          "They say this movie is worth watching",
          "Passive-like construction with 'They say', 'worth doing sth'.",
          "3",
          "Я с нетерпением жду нашей встречи",
          "I am looking forward to our meeting",
          "Phrasal verb 'look forward to' + noun/-ing.",
          "3",
          "Он не только умный, но и очень добрый",
          "He is not only smart but also very kind",
          "Correlative conjunction 'not only... but also...'.",
          "3",
          "К тому времени, как мы приехали, вечеринка уже началась",
          "By the time we arrived, the party had already started",
          "Past Perfect tense with 'by the time'.",
          "3"};
        for (int i = 0; i < data.size(); i += 4) {
            q.addBindValue(data[i]);
            q.addBindValue(data[i + 1]);
            q.addBindValue(data[i + 2]);
            q.addBindValue(data[i + 3].toInt());
            q.exec();
        }
    }
    qDebug() << "Database initialized/checked.";
}

void MainWindow::setupMainLayout() {
    mainContainer_ = new QWidget(this);
    QVBoxLayout* lyt = new QVBoxLayout(mainContainer_);

    lyt->addWidget(topPanel_);
    lyt->addWidget(progressBar_);
    lyt->addWidget(stackedWidget_, 1);

    stackedWidget_->addWidget(startPage_);
    stackedWidget_->addWidget(placeholderPage_);
    stackedWidget_->addWidget(transPage_);
    stackedWidget_->addWidget(grammarPage_);

    setCentralWidget(mainContainer_);
}

void MainWindow::setupTopPanel() {
    topPanel_ = new QWidget(this);
    QHBoxLayout* lyt = new QHBoxLayout(topPanel_);

    QString stl =
        "QPushButton {"
        "   background-color: #5cb85c;"
        "   border: none;"
        "   color: white;"
        "   padding: 10px 20px;"
        "   text-align: center;"
        "   text-decoration: none;"
        "   font-size: 14px;"
        "   border-radius: 8px;"
        "   min-width: 120px;"
        "}"
        "QPushButton:hover { background-color: #4cae4c; }"
        "QPushButton:pressed { background-color: #449d44; }";

    transBtn_ = new QPushButton("Перевод", this);
    transBtn_->setStyleSheet(stl);
    grammarBtn_ = new QPushButton("Грамматика", this);
    grammarBtn_->setStyleSheet(stl);

    lyt->addWidget(transBtn_);
    lyt->addWidget(grammarBtn_);
    lyt->addStretch();
    lyt->addWidget(scoreLbl_);
    lyt->addWidget(attemptsLbl_);
    lyt->addWidget(timerLbl_);

    topPanel_->setLayout(lyt);
}

void MainWindow::setupStartPage() {
    startPage_ = new QWidget(this);
    QVBoxLayout* lyt = new QVBoxLayout(startPage_);

    logoLbl_ = new QLabel(this);
    QPixmap pxm("labs/duolingo/logo.png");
    if (!pxm.isNull()) {
        logoLbl_->setPixmap(pxm.scaled(200, 200, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        logoLbl_->setText("Логотип (не найден)");
        qWarning() << "Logo not found at ../../logo.png";
    }
    logoLbl_->setAlignment(Qt::AlignCenter);

    welcomeLbl_ = new QLabel("Добро пожаловать в Language Learner!", this);
    welcomeLbl_->setAlignment(Qt::AlignCenter);
    QFont fnt = welcomeLbl_->font();
    fnt.setPointSize(20);
    fnt.setBold(true);
    welcomeLbl_->setFont(fnt);

    startBtn_ = new QPushButton("Начать", this);
    startBtn_->setStyleSheet(
        "QPushButton {"
        "   background-color: #4CAF50;"
        "   color: white;"
        "   font-size: 18px;"
        "   padding: 15px 30px;"
        "   border-radius: 10px;"
        "   min-width: 150px;"
        "}"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:pressed { background-color: #3e8e41; }");
    startBtn_->setMinimumHeight(50);

    lyt->addStretch();
    lyt->addWidget(logoLbl_, 0, Qt::AlignCenter);
    lyt->addWidget(welcomeLbl_, 0, Qt::AlignCenter);
    lyt->addWidget(startBtn_, 0, Qt::AlignCenter);
    lyt->addStretch();
    startPage_->setLayout(lyt);
}

void MainWindow::setupPlaceholderPage() {
    placeholderPage_ = new QWidget(this);
    QVBoxLayout* lyt = new QVBoxLayout(placeholderPage_);
    placeholderLbl_ =
        new QLabel("Выберите тип упражнения (Перевод или Грамматика), чтобы начать.", this);
    placeholderLbl_->setAlignment(Qt::AlignCenter);
    QFont fnt = placeholderLbl_->font();
    fnt.setPointSize(16);
    placeholderLbl_->setFont(fnt);
    lyt->addStretch();
    lyt->addWidget(placeholderLbl_, 0, Qt::AlignCenter);
    lyt->addStretch();
    placeholderPage_->setLayout(lyt);
}

void MainWindow::setupMenu() {
    QMenu* m_set = menuBar()->addMenu("Настройки");
    QAction* a_diff = new QAction("Изменить сложность", this);
    connect(a_diff, &QAction::triggered, this, &MainWindow::openDifficultyDialog);
    m_set->addAction(a_diff);

    QMenu* m_help = menuBar()->addMenu("Помощь");
    QAction* a_help_ex = new QAction("Подсказка к упражнению (H)", this);
    connect(a_help_ex, &QAction::triggered, this, &MainWindow::showHelp);
    m_help->addAction(a_help_ex);
}

void MainWindow::createDiffDialog() {
    diffDialog_ = new QDialog(this);
    diffDialog_->setWindowTitle("Выбор уровня сложности");
    QVBoxLayout* lyt = new QVBoxLayout(diffDialog_);

    QLabel* lbl = new QLabel("Выберите уровень сложности:", diffDialog_);
    diffComboBox_ = new QComboBox(diffDialog_);
    diffComboBox_->addItem("Легкий", QVariant(1));
    diffComboBox_->addItem("Средний", QVariant(2));
    diffComboBox_->addItem("Сложный", QVariant(3));

    int idx = diffComboBox_->findData(curDifficulty_);
    if (idx != -1) {
        diffComboBox_->setCurrentIndex(idx);
    }

    QDialogButtonBox* btns =
        new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, diffDialog_);
    connect(btns, &QDialogButtonBox::accepted, diffDialog_, &QDialog::accept);
    connect(btns, &QDialogButtonBox::rejected, diffDialog_, &QDialog::reject);

    lyt->addWidget(lbl);
    lyt->addWidget(diffComboBox_);
    lyt->addWidget(btns);
    diffDialog_->setLayout(lyt);
}

void MainWindow::openDifficultyDialog() {
    int idx = diffComboBox_->findData(curDifficulty_);
    if (idx != -1) {
        diffComboBox_->setCurrentIndex(idx);
    }

    if (diffDialog_->exec() == QDialog::Accepted) {
        curDifficulty_ = diffComboBox_->currentData().toInt();
        QMessageBox::information(
            this, "Сложность изменена",
            QString("Новый уровень сложности: %1").arg(diffComboBox_->currentText()));
    }
}

void MainWindow::setupTransPage() {
    transPage_ = new QWidget(this);
    QVBoxLayout* lyt = new QVBoxLayout(transPage_);

    transTaskLbl_ = new QLabel("Текст для перевода появится здесь.", this);
    transTaskLbl_->setWordWrap(true);
    transTaskLbl_->setStyleSheet(
        "font-size: 16px; border: 1px solid #ccc; padding: 10px; background-color: #f9f9f9; "
        "border-radius: 5px;");
    transTaskLbl_->setMinimumHeight(100);

    transInput_ = new QTextEdit(this);
    transInput_->setPlaceholderText("Введите ваш перевод здесь...");
    transInput_->setStyleSheet(
        "font-size: 16px; border: 1px solid #ccc; padding: 10px; border-radius: 5px;");
    transInput_->setMaximumHeight(150);

    transSubmitBtn_ = new QPushButton("Отправить", this);
    transSubmitBtn_->setStyleSheet(
        "QPushButton { background-color: #007bff; color: white; font-size: 16px; padding: 10px "
        "20px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #0069d9; }"
        "QPushButton:pressed { background-color: #005cbf; }");
    transSubmitBtn_->setMinimumHeight(40);

    lyt->addWidget(new QLabel("Переведите следующий текст:", this));
    lyt->addWidget(transTaskLbl_, 1);
    lyt->addWidget(new QLabel("Ваш перевод:", this));
    lyt->addWidget(transInput_, 1);
    lyt->addWidget(transSubmitBtn_, 0, Qt::AlignCenter);

    transPage_->setLayout(lyt);
}

void MainWindow::setupGrammarPage() {
    grammarPage_ = new QWidget(this);
    QVBoxLayout* lyt = new QVBoxLayout(grammarPage_);

    grammarQuestLbl_ = new QLabel("Вопрос по грамматике появится здесь.", this);
    grammarQuestLbl_->setWordWrap(true);
    grammarQuestLbl_->setStyleSheet("font-size: 16px; margin-bottom: 10px;");

    grammarOptsGrp_ = new QGroupBox("Выберите один вариант:", this);
    grammarOptsLayout_ = new QVBoxLayout();

    for (int i = 0; i < 4; ++i) {
        QRadioButton* rb = new QRadioButton(QString("Вариант %1").arg(i + 1), this);
        rb->setStyleSheet("font-size: 14px;");
        grammarOptsRb_.append(rb);
        grammarOptsLayout_->addWidget(rb);
    }
    grammarOptsGrp_->setLayout(grammarOptsLayout_);

    grammarSubmitBtn_ = new QPushButton("Отправить", this);
    grammarSubmitBtn_->setStyleSheet(
        "QPushButton { background-color: #007bff; color: white; font-size: 16px; padding: 10px "
        "20px; border-radius: 5px; }"
        "QPushButton:hover { background-color: #0069d9; }"
        "QPushButton:pressed { background-color: #005cbf; }");
    grammarSubmitBtn_->setMinimumHeight(40);

    lyt->addWidget(grammarQuestLbl_);
    lyt->addWidget(grammarOptsGrp_);
    lyt->addStretch();
    lyt->addWidget(grammarSubmitBtn_, 0, Qt::AlignCenter);

    grammarPage_->setLayout(lyt);
}

void MainWindow::setupConnections() {
    connect(startBtn_, &QPushButton::clicked, this, &MainWindow::onStartBtnClicked);
    connect(transBtn_, &QPushButton::clicked, this, &MainWindow::switchToTransEx);
    connect(grammarBtn_, &QPushButton::clicked, this, &MainWindow::switchToGrammarEx);
    connect(transSubmitBtn_, &QPushButton::clicked, this, &MainWindow::checkTransAnswer);
    connect(grammarSubmitBtn_, &QPushButton::clicked, this, &MainWindow::checkGrammarAnswer);
    connect(seriesTimer_, &QTimer::timeout, this, &MainWindow::updateSeriesTimer);
}

void MainWindow::onStartBtnClicked() {
    topPanel_->show();
    progressBar_->show();
    stackedWidget_->setCurrentWidget(placeholderPage_);
    resetGameStats();
    updateStatsUI();
}

void MainWindow::resetGameStats() {
    curScore_ = 0;
}

void MainWindow::switchToTransEx() {
    startNewSeries(ExType::Translation);
}

void MainWindow::switchToGrammarEx() {
    startNewSeries(ExType::Grammar);
}

void MainWindow::startNewSeries(ExType type) {
    curExType_ = type;
    completedTasks_ = 0;
    curTaskIdx_ = -1;
    remAttempts_ = MAX_ATTEMPTS_PER_SERIES;
    seriesTimeRemSec_ = SERIES_TIME_SECONDS;

    curTransSeries_.clear();
    curGrammarSeries_.clear();

    loadTasksForSeries();

    if ((type == ExType::Translation && curTransSeries_.isEmpty()) ||
        (type == ExType::Grammar && curGrammarSeries_.isEmpty())) {
        QMessageBox::warning(
            this, "Нет заданий",
            QString("Не удалось загрузить задания для сложности (%1).").arg(curDifficulty_));
        stackedWidget_->setCurrentWidget(placeholderPage_);
        curExType_ = ExType::None;
        seriesTimer_->stop();
        updateStatsUI();
        return;
    }

    if (type == ExType::Translation) {
        stackedWidget_->setCurrentWidget(transPage_);
    } else if (type == ExType::Grammar) {
        stackedWidget_->setCurrentWidget(grammarPage_);
    }

    seriesTimer_->start(1000);
    loadNextTask();
    updateStatsUI();
}

void MainWindow::loadTasksForSeries() {
    QSqlQuery q(db_);
    QString sql;

    if (curExType_ == ExType::Translation) {
        sql =
            "SELECT * FROM translation_exercises WHERE difficulty = :difficulty ORDER BY RANDOM() "
            "LIMIT :limit";
    } else if (curExType_ == ExType::Grammar) {
        sql =
            "SELECT * FROM grammar_exercises WHERE difficulty = :difficulty ORDER BY RANDOM() "
            "LIMIT :limit";
    } else {
        return;
    }

    q.prepare(sql);
    q.bindValue(":difficulty", curDifficulty_);
    q.bindValue(":limit", TASKS_PER_SERIES);

    if (!q.exec()) {
        qWarning() << "Failed to load tasks:" << q.lastError().text();
        return;
    }

    while (q.next()) {
        if (curExType_ == ExType::Translation) {
            curTransSeries_.append(TranslationTask::fromSql(q));
        } else {
            curGrammarSeries_.append(GrammarTask::fromSql(q));
        }
    }
    qDebug() << "Loaded"
             << (curExType_ == ExType::Translation ? curTransSeries_.size()
                                                   : curGrammarSeries_.size())
             << "tasks.";
}

void MainWindow::loadNextTask() {
    curTaskIdx_++;

    if (curExType_ == ExType::Translation) {
        if (curTaskIdx_ < curTransSeries_.size()) {
            updateTransTaskUI();
        } else {
            endSeries(true, "Все задания на перевод выполнены!");
        }
    } else if (curExType_ == ExType::Grammar) {
        if (curTaskIdx_ < curGrammarSeries_.size()) {
            updateGrammarTaskUI();
        } else {
            endSeries(true, "Все задания по грамматике выполнены!");
        }
    }
}

void MainWindow::updateTransTaskUI() {
    if (curTaskIdx_ < curTransSeries_.size()) {
        const auto& t = curTransSeries_[curTaskIdx_];
        transTaskLbl_->setText(t.nativeText());
        transInput_->clear();
        transInput_->setFocus();
    }
}

void MainWindow::updateGrammarTaskUI() {
    if (curTaskIdx_ < curGrammarSeries_.size()) {
        const auto& t = curGrammarSeries_[curTaskIdx_];
        grammarQuestLbl_->setText(t.question());
        QStringList opts = t.options();
        for (int i = 0; i < grammarOptsRb_.size(); ++i) {
            if (i < opts.size()) {
                grammarOptsRb_[i]->setText(opts[i]);
                grammarOptsRb_[i]->setVisible(true);
                grammarOptsRb_[i]->setAutoExclusive(false);
                grammarOptsRb_[i]->setChecked(false);
                grammarOptsRb_[i]->setAutoExclusive(true);
            } else {
                grammarOptsRb_[i]->setVisible(false);
            }
        }
    }
}

void MainWindow::checkTransAnswer() {
    if (curExType_ != ExType::Translation || curTaskIdx_ >= curTransSeries_.size()) {
        return;
    }

    QString ans = transInput_->toPlainText().trimmed();
    const auto& task = curTransSeries_[curTaskIdx_];
    int total = curTransSeries_.size();

    if (task.checkAnswer(ans)) {
        completedTasks_++;
        if (completedTasks_ == total) {
            int pts = 10 * completedTasks_ * curDifficulty_;
            curScore_ += pts;
            updateStatsUI();
            endSeries(
                true, QString("Поздравляем! Серия выполнена! +%1 очков. Счёт: %2")
                          .arg(pts)
                          .arg(curScore_));
        } else if (curTaskIdx_ < total - 1) {
            updateStatsUI();
            loadNextTask();
        } else {
            updateStatsUI();
            qWarning() << "checkTransAnswer: Unexpected state.";
        }
    } else {
        remAttempts_--;
        updateStatsUI();
        QMessageBox::warning(
            this, "Неправильно",
            QString("Попробуйте еще. Ответ: %1\nПопыток: %2")
                .arg(task.targetText())
                .arg(remAttempts_));
        if (remAttempts_ <= 0) {
            endSeries(false, "Попытки закончились. Серия не пройдена.");
        }
    }
}

void MainWindow::checkGrammarAnswer() {
    if (curExType_ != ExType::Grammar || curTaskIdx_ >= curGrammarSeries_.size()) {
        return;
    }

    int sel = -1;
    for (int i = 0; i < grammarOptsRb_.size(); ++i) {
        if (grammarOptsRb_[i]->isChecked()) {
            sel = i;
            break;
        }
    }

    if (sel == -1) {
        QMessageBox::warning(this, "Внимание", "Выберите вариант.");
        return;
    }

    const auto& task = curGrammarSeries_[curTaskIdx_];
    int total = curGrammarSeries_.size();

    if ((sel + 1) == task.correctOption()) {
        completedTasks_++;
        if (completedTasks_ == total) {
            int pts = 15 * completedTasks_ * curDifficulty_;
            curScore_ += pts;
            updateStatsUI();
            endSeries(
                true, QString("Поздравляем! Серия выполнена! +%1 очков. Счёт: %2")
                          .arg(pts)
                          .arg(curScore_));
        } else if (curTaskIdx_ < total - 1) {
            updateStatsUI();
            loadNextTask();
        } else {
            updateStatsUI();
            qWarning() << "checkGrammarAnswer: Unexpected state.";
        }
    } else {
        remAttempts_--;
        updateStatsUI();
        QString cr_ans = "неизвестен";
        if (task.correctOption() > 0 && task.correctOption() <= task.options().size()) {
            cr_ans = task.options().at(task.correctOption() - 1);
        }
        QMessageBox::warning(
            this, "Неправильно",
            QString("Неверно. Ответ: %1\nПопыток: %2").arg(cr_ans).arg(remAttempts_));
        if (remAttempts_ <= 0) {
            endSeries(false, "Попытки закончились. Серия не пройдена.");
        }
    }
}

void MainWindow::updateSeriesTimer() {
    if (seriesTimeRemSec_ > 0) {
        seriesTimeRemSec_--;
        updateStatsUI();
    } else {
        seriesTimer_->stop();
        endSeries(false, "Время вышло! Серия не пройдена.");
    }
}

void MainWindow::endSeries(bool success, const QString& msg) {
    seriesTimer_->stop();
    curExType_ = ExType::None;

    if (success) {
        QMessageBox::information(this, "Серия завершена", msg);
    } else {
        QMessageBox::critical(this, "Серия прервана", msg);
    }

    updateStatsUI();
    stackedWidget_->setCurrentWidget(placeholderPage_);
}

void MainWindow::updateStatsUI() {
    scoreLbl_->setText(QString("Счёт: %1").arg(curScore_));

    if (curExType_ != ExType::None) {
        attemptsLbl_->setText(QString("Попытки: %1").arg(remAttempts_));
        timerLbl_->setText(
            QString("Время: %1:%02").arg(seriesTimeRemSec_ / 60).arg(seriesTimeRemSec_ % 60));
        progressBar_->setValue(completedTasks_);
        progressBar_->setFormat(QString("%1 / %2").arg(completedTasks_).arg(TASKS_PER_SERIES));
    } else {
        attemptsLbl_->setText(QString("Попытки: %1").arg(MAX_ATTEMPTS_PER_SERIES));
        timerLbl_->setText(QString("Время: --:--"));
        progressBar_->setValue(0);
        progressBar_->setFormat("0 / " + QString::number(TASKS_PER_SERIES));
    }
}

void MainWindow::keyPressEvent(QKeyEvent* ev) {
    if (ev->key() == Qt::Key_H) {
        if (curExType_ != ExType::None && curTaskIdx_ >= 0) {
            showHelp();
        } else {
            QMessageBox::information(this, "Подсказка", "Начните упражнение для подсказки.");
        }
    } else {
        QMainWindow::keyPressEvent(ev);
    }
}

void MainWindow::showHelp() {
    QString hnt = "Подсказка отсутствует.";
    if (curExType_ == ExType::Translation) {
        if (curTaskIdx_ < curTransSeries_.size()) {
            QString th = curTransSeries_[curTaskIdx_].hint();
            if (!th.isEmpty()) {
                hnt = th;
            }
        }
    } else if (curExType_ == ExType::Grammar) {
        if (curTaskIdx_ < curGrammarSeries_.size()) {
            QString th = curGrammarSeries_[curTaskIdx_].hint();
            if (!th.isEmpty()) {
                hnt = th;
            }
        }
    } else {
        QMessageBox::information(this, "Подсказка", "Начните упражнение для подсказки.");
        return;
    }
    QMessageBox::information(this, "Подсказка к упражнению", hnt);
}
