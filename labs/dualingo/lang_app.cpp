#include "lang_app.h"

#include <QtWidgets/QWidget>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QMenu>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QApplication>

#include <QtGui/QAction>
#include <QtGui/QShortcut>
#include <QtGui/QKeySequence>

#include <QtCore/QRegularExpression>
#include <QtCore/QProcess>
#include <QtCore/QTimer>

#include <algorithm>
#include <random>

LangApp::LangApp(QWidget* parent) : QMainWindow(parent) {
    resize(850, 500);

    loadMockData();
    setupMenu();
    setupUI();

    m_timer = new QTimer(this);
    connect(m_timer, &QTimer::timeout, this, &LangApp::onTimerTick);

    QShortcut* helpShortcut = new QShortcut(QKeySequence(Qt::Key_H), this);
    helpShortcut->setContext(Qt::ApplicationShortcut);
    connect(helpShortcut, &QShortcut::activated, this, &LangApp::showHelp);
}

void LangApp::setupMenu() {
    menuBar()->setNativeMenuBar(false); 
    QMenu* settingsMenu = menuBar()->addMenu("Настройки");
    QAction* diffAction = settingsMenu->addAction("Изменить уровень сложности");
    connect(diffAction, &QAction::triggered, this, &LangApp::openDifficultyDialog);
}

void LangApp::openDifficultyDialog() {
    QDialog dialog(this);
    dialog.setWindowTitle("Уровень сложности");
    QVBoxLayout* layout = new QVBoxLayout(&dialog);

    QRadioButton* easyBtn = new QRadioButton("Легко (5 ошибок, 90 сек)");
    QRadioButton* medBtn = new QRadioButton("Средне (3 ошибки, 60 сек)");
    QRadioButton* hardBtn = new QRadioButton("Сложно (1 ошибка, 30 сек)");

    layout->addWidget(easyBtn);
    layout->addWidget(medBtn);
    layout->addWidget(hardBtn);

    if (m_maxMistakes == 5) easyBtn->setChecked(true);
    else if (m_maxMistakes == 1) hardBtn->setChecked(true);
    else medBtn->setChecked(true);

    QDialogButtonBox* buttons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        if (easyBtn->isChecked()) { m_maxMistakes = 5; m_timeLimit = 90; }
        else if (hardBtn->isChecked()) { m_maxMistakes = 1; m_timeLimit = 30; }
        else { m_maxMistakes = 3; m_timeLimit = 60; }
        QMessageBox::information(this, "Готово", "Уровень сложности обновлен! Мяу!");
    }
}

void LangApp::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout* mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout* leftPanel = new QVBoxLayout();
    
    m_scoreLabel = new QLabel("🐟 Вкусные рыбки: 0");
    m_livesLabel = new QLabel("🐾 Кото-жизни: 3");
    m_timerLabel = new QLabel("⏳ Время: --:--");
    m_scoreLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #0984e3;");
    m_livesLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #d63031;");
    m_timerLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #636e72;");

    m_progressBar = new QProgressBar();
    m_progressBar->setValue(0);
    m_progressBar->setTextVisible(true);

    m_btnTranslation = new QPushButton("📝 Перевод (Мяу!)");
    m_btnGrammar = new QPushButton("🧠 Грамматика (Мурр!)");

    leftPanel->addWidget(m_scoreLabel);
    leftPanel->addWidget(m_livesLabel);
    leftPanel->addWidget(m_timerLabel);
    leftPanel->addWidget(new QLabel("Миска знаний заполняется: 🥣"));
    leftPanel->addWidget(m_progressBar);
    leftPanel->addSpacing(30);
    leftPanel->addWidget(m_btnTranslation);
    leftPanel->addWidget(m_btnGrammar);
    leftPanel->addStretch();

    m_rightPanel = new QStackedWidget();

    QWidget* welcomePage = new QWidget();
    QVBoxLayout* welcomeLayout = new QVBoxLayout(welcomePage);
    QLabel* welcomeLabel = new QLabel("Привет, человек! 🐈\nВыберите режим слева, чтобы покормить котика знаниями.");
    welcomeLabel->setAlignment(Qt::AlignCenter);
    welcomeLabel->setStyleSheet("font-size: 18px; font-weight: bold; color: #d35400;");
    welcomeLayout->addWidget(welcomeLabel);
    m_rightPanel->addWidget(welcomePage);

    QWidget* transPage = new QWidget();
    QVBoxLayout* transLayout = new QVBoxLayout(transPage);
    m_transQuestionLabel = new QLabel("");
    m_transQuestionLabel->setWordWrap(true);
    m_transQuestionLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    
    m_transTextEdit = new QTextEdit();
    m_transTextEdit->setPlaceholderText("Введите перевод здесь...");
    m_transTextEdit->setMaximumHeight(100);
    
    m_transAudioBtn = new QPushButton("🔊 Слушать кошачий английский");
    m_transAudioBtn->setStyleSheet("background-color: #0abde3;");
    m_transSubmitBtn = new QPushButton("🐾 Отправить перевод");
    
    transLayout->addWidget(m_transQuestionLabel);
    transLayout->addWidget(m_transAudioBtn);
    transLayout->addWidget(m_transTextEdit);
    transLayout->addWidget(m_transSubmitBtn);
    transLayout->addStretch();
    m_rightPanel->addWidget(transPage);

    QWidget* gramPage = new QWidget();
    QVBoxLayout* gramLayout = new QVBoxLayout(gramPage);
    m_gramQuestionLabel = new QLabel("");
    m_gramQuestionLabel->setStyleSheet("font-size: 20px; font-weight: bold;");
    
    m_gramRadioLayout = new QVBoxLayout();
    m_gramRadioGroup = new QButtonGroup(this);
    
    m_gramSubmitBtn = new QPushButton("🐾 Выбрать этот вариант");

    gramLayout->addWidget(m_gramQuestionLabel);
    gramLayout->addLayout(m_gramRadioLayout);
    gramLayout->addStretch();
    gramLayout->addWidget(m_gramSubmitBtn);
    m_rightPanel->addWidget(gramPage);

    QWidget* resultPage = new QWidget();
    QVBoxLayout* resultLayout = new QVBoxLayout(resultPage);
    m_resultLabel = new QLabel("");
    m_resultLabel->setAlignment(Qt::AlignCenter);
    m_resultLabel->setStyleSheet("font-size: 22px; font-weight: bold;");
    resultLayout->addStretch();
    resultLayout->addWidget(m_resultLabel);
    resultLayout->addStretch();
    m_rightPanel->addWidget(resultPage);

    mainLayout->addLayout(leftPanel, 1);
    mainLayout->addWidget(m_rightPanel, 2);

    connect(m_btnTranslation, &QPushButton::clicked, this, &LangApp::startTranslationTask);
    connect(m_btnGrammar, &QPushButton::clicked, this, &LangApp::startGrammarTask);
    connect(m_transSubmitBtn, &QPushButton::clicked, this, &LangApp::submitTranslation);
    connect(m_gramSubmitBtn, &QPushButton::clicked, this, &LangApp::submitGrammar);
    connect(m_transAudioBtn, &QPushButton::clicked, this, &LangApp::playAudioMock);
    
    m_rightPanel->setCurrentIndex(0);
}

void LangApp::loadMockData() {
    m_allTranslationTasks = {
        {TaskType::Translation, "Мой кот любит спать на клавиатуре", "My cat loves to sleep on the keyboard", {}, "Глагол 'любить' - love. Клавиатура - keyboard."},
        {TaskType::Translation, "Кошка пьет молоко из миски", "The cat drinks milk from the bowl", {}, "Не забудьте 's' у глагола в 3-м лице (drinks). Миска - bowl."},
        {TaskType::Translation, "Где мой пушистый котенок?", "Where is my fluffy kitten", {}, "Пушистый - fluffy, котенок - kitten."},
        {TaskType::Translation, "Этот рыжий кот очень толстый", "This ginger cat is very fat", {}, "Рыжий кот по-английски часто называется 'ginger cat'."},
        {TaskType::Translation, "Я кормлю кота каждый день", "I feed the cat every day", {}, "Кормить - feed. Каждый день - every day."},
        {TaskType::Translation, "Коты боятся огурцов", "Cats are afraid of cucumbers", {}, "Бояться чего-то - 'are afraid of'."},
        {TaskType::Translation, "Она гладит кошку прямо сейчас", "She is petting the cat right now", {}, "Действие происходит сейчас, используйте Present Continuous (is petting)."}
    };

    m_allGrammarTasks = {
        {TaskType::Grammar, "The cat ___ on the sofa right now.", "is sleeping", {"sleeps", "is sleeping", "slept", "sleeping"}, "Слово 'right now' указывает на Present Continuous."},
        {TaskType::Grammar, "I have ___ fed the street cats.", "already", {"yet", "already", "since", "for"}, "С Present Perfect в утвердительных предложениях часто используется 'already'."},
        {TaskType::Grammar, "If the cat ___, I will give it a treat.", "meows", {"meowed", "will meow", "meows", "meow"}, "Условное предложение 1 типа: If + Present, Future Simple. (treat - вкусняшка)"},
        {TaskType::Grammar, "He is interested ___ reading about lion behavior.", "in", {"on", "in", "at", "about"}, "Устойчивое выражение: interested in."},
        {TaskType::Grammar, "Look at that cat! It ___ catch the mouse.", "is going to", {"will", "is going to", "goes to", "would"}, "Планы или очевидные намерения в будущем выражаются через 'to be going to'."},
        {TaskType::Grammar, "You ___ not pull a cat's tail!", "must", {"can", "might", "must", "could"}, "Выражение строгой обязанности или правила (must). Не дергай кота за хвост!"},
        {TaskType::Grammar, "This is the laser pointer ___ the cat loves.", "which", {"who", "where", "which", "whose"}, "Для неодушевленных предметов (лазерная указка) используется 'which' или 'that'."}
    };
}

void LangApp::startTranslationTask() { startSession(TaskType::Translation); }
void LangApp::startGrammarTask() { startSession(TaskType::Grammar); }

void LangApp::startSession(TaskType type) {
    if (type == TaskType::Translation) m_sessionTasks = m_allTranslationTasks;
    else m_sessionTasks = m_allGrammarTasks;

    std::random_device rd;
    std::default_random_engine rng(rd());
    std::shuffle(m_sessionTasks.begin(), m_sessionTasks.end(), rng);

    if (m_sessionTasks.size() > 5) {
        m_sessionTasks.resize(5);
    }
    
    m_currentIndex = 0;
    m_sessionPoints = 0;
    m_currentMistakes = 0;
    
    m_livesLabel->setText(QString("🐾 Кото-жизни: %1").arg(m_maxMistakes));
    m_progressBar->setMaximum(m_sessionTasks.size());
    m_progressBar->setValue(0);
    m_timeLeft = m_timeLimit;
    m_timerLabel->setText(QString("⏳ Время: %1").arg(m_timeLeft));
    m_timer->start(1000);

    displayCurrentTask();
}

void LangApp::displayCurrentTask() {
    if (m_currentIndex >= m_sessionTasks.size()) {
        finishExercise(true, "Упражнение успешно завершено! 😻");
        return;
    }

    const Task& current = m_sessionTasks[m_currentIndex];
    
    if (current.type == TaskType::Translation) {
        m_transQuestionLabel->setText("Переведите на английский:\n\n" + current.question);
        m_transTextEdit->clear();
        m_rightPanel->setCurrentIndex(1);
        m_transTextEdit->setFocus();
    } 
    else if (current.type == TaskType::Grammar) {
        m_gramQuestionLabel->setText("Выберите правильный вариант:\n\n" + current.question);
        
        QLayoutItem* item;
        while ((item = m_gramRadioLayout->takeAt(0)) != nullptr) {
            delete item->widget();
            delete item;
        }
        
        for (const QString& opt : current.options) {
            QRadioButton* rb = new QRadioButton(opt);
            m_gramRadioGroup->addButton(rb);
            m_gramRadioLayout->addWidget(rb);
        }
        m_rightPanel->setCurrentIndex(2);
    }
}

void LangApp::playAudioMock() {
    if (m_currentIndex >= m_sessionTasks.size()) return;
    
    QString textToSpeak = m_sessionTasks[m_currentIndex].correctAnswer;

#ifdef Q_OS_MAC
    QProcess::startDetached("say", QStringList() << textToSpeak);
#elif defined(Q_OS_WIN)
    QString psCommand = QString("Add-Type -AssemblyName System.Speech; (New-Object System.Speech.Synthesis.SpeechSynthesizer).Speak('%1')").arg(textToSpeak);
    QProcess::startDetached("powershell", QStringList() << "-Command" << psCommand);
#else
    QApplication::beep();
#endif

    m_transAudioBtn->setText("🔊 Читаю... (Мяу)");
    QTimer::singleShot(1500, [this](){ m_transAudioBtn->setText("🔊 Слушать кошачий английский"); });
}

bool LangApp::isTranslationAcceptable(const QString& input, const QString& expected) {
    QString s1 = input.trimmed().toLower();
    QString s2 = expected.trimmed().toLower();
    s1.remove(QRegularExpression("[\\.,!?]"));
    s2.remove(QRegularExpression("[\\.,!?]"));

    int len1 = s1.length();
    int len2 = s2.length();
    QVector<QVector<int>> d(len1 + 1, QVector<int>(len2 + 1));

    for (int i = 0; i <= len1; ++i) d[i][0] = i;
    for (int j = 0; j <= len2; ++j) d[0][j] = j;

    for (int i = 1; i <= len1; ++i) {
        for (int j = 1; j <= len2; ++j) {
            int cost = (s1[i - 1] == s2[j - 1]) ? 0 : 1;
            d[i][j] = std::min({ d[i - 1][j] + 1, d[i][j - 1] + 1, d[i - 1][j - 1] + cost });
        }
    }
    int allowedMistakes = std::max(1, len2 / 5);
    return d[len1][len2] <= allowedMistakes;
}

void LangApp::submitTranslation() {
    QString answer = m_transTextEdit->toPlainText();
    if (answer.isEmpty()) return;

    const Task& current = m_sessionTasks[m_currentIndex];
    bool correct = isTranslationAcceptable(answer, current.correctAnswer);
    processAnswer(correct);
}

void LangApp::submitGrammar() {
    QAbstractButton* checked = m_gramRadioGroup->checkedButton();
    if (!checked) {
        QMessageBox::warning(this, "Внимание", "Пожалуйста, выберите ответ!");
        return;
    }

    const Task& current = m_sessionTasks[m_currentIndex];
    bool correct = (checked->text() == current.correctAnswer);
    processAnswer(correct);
}

void LangApp::processAnswer(bool isCorrect) {
    if (isCorrect) {
        m_sessionPoints += 10;
        m_currentIndex++;
        m_progressBar->setValue(m_currentIndex);
        displayCurrentTask();
    } else {
        m_currentMistakes++;
        m_livesLabel->setText(QString("🐾 Кото-жизни: %1").arg(m_maxMistakes - m_currentMistakes));

        if (m_currentMistakes >= m_maxMistakes) {
            finishExercise(false, "Шшшш! 😾 Слишком много ошибок.\nКотик убежал под диван.");
        } else {
            QMessageBox::warning(this, "Упс!", "Неверно! Котик недоволен. Попробуй еще раз! 😿");
        }
    }
}

void LangApp::onTimerTick() {
    if (m_timeLeft > 0) {
        m_timeLeft--;
        
        if (m_timeLeft <= 10) m_timerLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #e84118;");
        else m_timerLabel->setStyleSheet("font-size: 16px; font-weight: bold; color: #636e72;");
        
        int m = m_timeLeft / 60;
        int s = m_timeLeft % 60;
        m_timerLabel->setText(QString("⏳ Время: %1:%2").arg(m, 2, 10, QChar('0')).arg(s, 2, 10, QChar('0')));
    } else {
        finishExercise(false, "Время вышло! Мышка убежала 🐁💨");
    }
}

void LangApp::showHelp() {
    int page = m_rightPanel->currentIndex();
    if (page == 1 || page == 2) {
        if (m_currentIndex < m_sessionTasks.size()) {
            QMessageBox::information(this, "Подсказка от кота-ученого 🧐", m_sessionTasks[m_currentIndex].hint);
        }
    }
}

void LangApp::finishExercise(bool success, const QString& message) {
    m_timer->stop();
    m_rightPanel->setCurrentIndex(3);

    if (success) {
        m_totalPoints += m_sessionPoints;
        m_scoreLabel->setText(QString("🐟 Вкусные рыбки: %1").arg(m_totalPoints));
        m_resultLabel->setStyleSheet("color: #4cd137;");
        m_resultLabel->setText(message + QString("\n\n😻 Заработано: +%1 рыбок!").arg(m_sessionPoints));
    } else {
        m_resultLabel->setStyleSheet("color: #e84118;");
        m_resultLabel->setText(message + "\n\n🙀 Очки за сессию сгорели.");
    }
}