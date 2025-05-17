#include "grammar.h"
#include <QMessageBox>

std::vector<GrammarMode::GrammarTask> GrammarMode::tasks;

void GrammarMode::loadTasksFromMemory() {
tasks = {
        // Beginner (1)
    {1, "What is the correct article for \"Hund\"?", 0, "der", "die", "das", "", 1},
    {2, "What is the plural of \"Kind\"?", 1, "Kind", "Kinder", "Kinds", "Kindes", 1},
    {3, "What is the correct article for \"Frau\"?", 1, "die", "der", "das", "", 1},
    {4, "What is the plural of \"Mann\"?", 0, "Männer", "Manns", "Männen", "Mann", 1},
    {5, "Which pronoun is correct for \"Mädchen\" (neuter)?", 1, "er", "es", "sie", "", 1},
    {6, "What is the correct article for \"Buch\"?", 2, "der", "die", "das", "", 1},
    {7, "What is the plural of \"Haus\"?", 1, "Haus", "Häuser", "Hauses", "Häusen", 1},
    {8, "Which pronoun is correct for \"Tisch\" (masculine)?", 0, "er", "es", "sie", "", 1},
    {9, "What is the correct article for \"Katze\"?", 1, "die", "der", "das", "", 1},
    {10, "What is the plural of \"Mutter\"?", 1, "Mutter", "Mütter", "Mutters", "Müttern", 1},
    {11, "Which pronoun is correct for \"Sonne\" (feminine)?", 2, "er", "es", "sie", "", 1},
    {12, "What is the correct article for \"Schule\"?", 1, "die", "der", "das", "", 1},
    {13, "What is the plural of \"Vater\"?", 0, "Väter", "Vaters", "Vättern", "Vater", 1},
    {14, "Which pronoun is correct for \"Kind\" (neuter)?", 1, "er", "es", "sie", "", 1},
    {15, "What is the correct article for \"Wasser\"?", 2, "der", "die", "das", "", 1},
    {16, "What is the plural of \"Auto\"?", 0, "Autos", "Auto", "Autos", "Autos", 1},
    {17, "Which pronoun is correct for \"Blume\" (feminine)?", 2, "er", "es", "sie", "", 1},
    {18, "What is the correct article for \"Fenster\"?", 2, "der", "die", "das", "", 1},
    {19, "What is the plural of \"Bruder\"?", 0, "Brüder", "Bruders", "Brüdern", "Bruder", 1},
    {20, "Which pronoun is correct for \"Hund\" (masculine)?", 0, "er", "es", "sie", "", 1},

    // Intermediate (2)
    {21, "What is the correct dative form of \"der Mann\"?", 1, "der Mann", "dem Mann", "den Mann", "des Mannes", 2},
    {22, "Which auxiliary verb is used with \"fahren\" in perfect tense?", 0, "sein", "haben", "", "", 2},
    {23, "What is the correct comparative of \"gut\"?", 2, "guter", "gute", "besser", "mehr gut", 2},
    {24, "What is the correct accusative form of \"die Frau\"?", 0, "die Frau", "der Frau", "den Frau", "die Frauen", 2},
    {25, "Which preposition correctly completes: \"Ich gehe ___ Schule\"?", 1, "bei", "zur", "von", "mit", 2},
    {26, "What is the correct past participle of \"sehen\"?", 2, "sehte", "geseht", "gesehen", "sah", 2},
    {27, "What is the correct dative form of \"das Kind\"?", 1, "das Kind", "dem Kind", "den Kind", "des Kindes", 2},
    {28, "Which preposition completes: \"Er arbeitet ___ einer Firma\"?", 1, "in", "bei", "an", "zu", 2},
    {29, "What is the correct comparative of \"schnell\"?", 0, "schneller", "schnellere", "mehr schnell", "am schnellsten", 2},
    {30, "Which auxiliary verb is used with \"lesen\" in perfect tense?", 1, "sein", "haben", "", "", 2},
    {31, "What is the correct accusative form of \"der Hund\"?", 2, "der Hund", "dem Hund", "den Hund", "des Hundes", 2},
    {32, "Which preposition completes: \"Sie wohnt ___ Berlin\"?", 0, "in", "bei", "an", "nach", 2},
    {33, "What is the correct past participle of \"trinken\"?", 1, "trinkte", "getrunken", "trunken", "getrinkt", 2},
    {34, "What is the correct dative form of \"die Mutter\"?", 1, "die Mutter", "der Mutter", "den Mutter", "die Mütter", 2},
    {35, "Which preposition completes: \"Ich interessiere mich ___ Musik\"?", 2, "an", "bei", "für", "mit", 2},
    {36, "What is the correct comparative of \"hoch\"?", 0, "höher", "hoch", "hocher", "am höchsten", 2},
    {37, "Which auxiliary verb is used with \"schwimmen\" in perfect tense?", 0, "sein", "haben", "", "", 2},
    {38, "What is the correct accusative form of \"das Buch\"?", 2, "der Buch", "dem Buch", "das Buch", "die Bücher", 2},
    {39, "Which preposition completes: \"Wir treffen uns ___ dem Park\"?", 1, "in", "im", "an", "auf", 2},
    {40, "What is the correct past participle of \"schreiben\"?", 1, "schrieb", "geschrieben", "geschriebt", "schreibte", 2},
    // Advanced (3)
    {41, "Which is the correct subjunctive II form of \"haben\" for \"wir\"?", 2, "hätte", "hättest", "hätten", "hättet", 3},
    {42, "What is the correct passive construction for \"Man liest das Buch\"?", 0, "Das Buch wird gelesen", "Das Buch wird lesen", "Das Buch ist gelesen", "Das Buch kann gelesen werden", 3},
    {43, "Which is the correct genitive form of \"das Auto des Mannes\"?", 1, "des Auto", "des Autos", "dem Auto", "die Autos", 3},
    {44, "What is the correct subjunctive II form of \"gehen\" for \"er\"?", 0, "ginge", "gehte", "gehen würde", "geht", 3},
    {45, "Which conjunction correctly completes: \"___ ich mehr Zeit hätte, würde ich mehr lesen\"?", 2, "Denn", "Weil", "Wenn", "Als", 3},
    {46, "What is the correct passive construction for \"Sie öffnet das Fenster\"?", 1, "Das Fenster wird öffnen", "Das Fenster wird geöffnet", "Das Fenster ist geöffnet", "Das Fenster kann geöffnet werden", 3},
    {47, "Which is the correct subjunctive II form of \"können\" for \"ich\"?", 0, "könnte", "könntest", "können", "könnt", 3},
    {48, "What is the correct genitive form of \"die Tasche der Frau\"?", 1, "der Tasche", "der Frau", "die Tasche der Frau", "der Frau Tasche", 3},
    {49, "Which conjunction completes: \"Er bleibt zu Hause, ___ er krank ist\"?", 1, "denn", "weil", "obwohl", "wenn", 3},
    {50, "What is the correct passive construction for \"Man hat das Haus gebaut\"?", 0, "Das Haus wurde gebaut", "Das Haus ist gebaut worden", "Das Haus war gebaut", "Das Haus wird gebaut", 3},
    {51, "Which is the correct subjunctive II form of \"müssen\" for \"du\"?", 1, "müsste", "müsstest", "müssten", "müsstet", 3},
    {52, "What is the correct genitive form of \"der Hund des Kindes\"?", 0, "des Hundes", "des Hunds", "dem Hund", "die Hunde", 3},
    {53, "Which conjunction completes: \"___ es regnet, gehen wir spazieren\"?", 2, "Denn", "Weil", "Obwohl", "Wenn", 3},
    {54, "What is the correct passive construction for \"Sie wird den Brief schreiben\"?", 1, "Der Brief wird schreiben", "Der Brief wird geschrieben werden", "Der Brief ist geschrieben", "Der Brief kann geschrieben werden", 3},
    {55, "Which is the correct subjunctive II form of \"sein\" for \"ihr\"?", 3, "wäre", "wärst", "wären", "wärt", 3},
    {56, "What is the correct genitive form of \"das Buch des Lehrers\"?", 1, "des Buches", "des Lehrers", "dem Buch", "die Bücher", 3},
    {57, "Which conjunction completes: \"___ du kommst, rufe ich dich an\"?", 3, "Denn", "Weil", "Obwohl", "Wenn", 3},
    {58, "What is the correct passive construction for \"Man kann das Problem lösen\"?", 3, "Das Problem wird lösen", "Das Problem wird gelöst", "Das Problem ist gelöst", "Das Problem kann gelöst werden", 3},
    {59, "Which is the correct subjunctive II form of \"wissen\" for \"sie\" (singular)?", 0, "wüsste", "wüsstest", "wüssten", "wüsstet", 3},
    {60, "What is the correct genitive form of \"die Idee des Mannes\"?", 1, "der Idee", "des Mannes", "die Idee des Mannes", "dem Mann seine Idee", 3}
};
}

GrammarMode::GrammarMode(QWidget* parent) : QWidget(parent), correctAnswerId(-1) {
    setupUI();
}

void GrammarMode::setupUI() {
    layout = new QVBoxLayout(this);
    layout->setSpacing(15);

    instructionLabel = new QLabel("Choose the correct option:");
    instructionLabel->setStyleSheet("font-weight: bold; font-size: 16px;");
    layout->addWidget(instructionLabel);

    startButton = new QPushButton("Start Session");
    startButton->setStyleSheet(
        "QPushButton { background-color: #2196F3; color: white; padding: 10px; font-size: 14px; }"
        "QPushButton:hover { background-color: #0b7dda; }");
    layout->addWidget(startButton);

    progressBar = new QProgressBar;
    progressBar->setRange(0, totalTasks);
    progressBar->setValue(0);
    progressBar->setTextVisible(true);
    progressBar->setFormat("%v/%m tasks");
    layout->addWidget(progressBar);

    questionLabel = new QLabel;
    questionLabel->setWordWrap(true);
    questionLabel->setStyleSheet("font-size: 14px;");
    layout->addWidget(questionLabel);

    optionsLayout = new QVBoxLayout;
    optionsGroup = new QButtonGroup(this);
    layout->addLayout(optionsLayout);

    submitButton = new QPushButton("Submit");
    submitButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px; font-size: 14px; }"
        "QPushButton:hover { background-color: #45a049; }");
    submitButton->setEnabled(false);
    layout->addWidget(submitButton);

    layout->addStretch();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        remainingTime = remainingTime.addSecs(-1);
        updateTimerDisplay();

        if (remainingTime == QTime(0, 0, 0)) {
            endSession(false);
            QMessageBox::warning(this, "Time's up", "You ran out of time!");
        }
    });

    timerLabel = new QLabel(this);
    timerLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    timerLabel->setAlignment(Qt::AlignCenter);
    layout->insertWidget(1, timerLabel);

    connect(startButton, &QPushButton::clicked, this, &GrammarMode::startSession);
    connect(submitButton, &QPushButton::clicked, this, &GrammarMode::checkAnswer);
}

void GrammarMode::setMode(int mode) {
    currentMode = mode;
}

void GrammarMode::startSession() {
    currentSession = SessionState();
    currentSession.isActive = true;
    currentSession.startTime = QDateTime::currentDateTime();

    remainingTime = QTime(0, 2, 0);
    updateTimerDisplay();

    progressBar->setValue(0);
    submitButton->setEnabled(true);
    startButton->setEnabled(false);

    timer->start(1000);
    loadTask(currentMode);
}

void GrammarMode::endSession(bool success) {
    currentSession.isActive = false;
    timer->stop();

    submitButton->setEnabled(false);
    startButton->setEnabled(true);

    QString message =
        success ? QString("Congratulations! You scored %1 points").arg(currentSession.score)
                : QString("Session ended. You scored %1 points").arg(currentSession.score);

    QMessageBox::information(this, "Session Ended", message);
    emit sessionCompleted(success);
}

void GrammarMode::loadTask(int mode) {
    clearOptions();
    if (tasks.empty()) loadTasksFromMemory();

    std::vector<GrammarTask> availableTasks;
    for (const auto& task : tasks) {
        if (task.difficulty == mode) {
            availableTasks.push_back(task);
        }
    }

    if (availableTasks.empty()) {
        questionLabel->setText("No tasks available for this difficulty level!");
        endSession(false);
        return;
    }

    // Явное приведение типа для bounded()
    int randomIndex = QRandomGenerator::global()->bounded(static_cast<int>(availableTasks.size()));
    const auto& task = availableTasks[randomIndex];

    questionLabel->setText(task.question);
    correctAnswerId = task.correct_id;

    if (!task.option1.isEmpty()) {
        QRadioButton* radio = new QRadioButton(task.option1);
        optionsGroup->addButton(radio, 0);
        optionsLayout->addWidget(radio);
    }
    if (!task.option2.isEmpty()) {
        QRadioButton* radio = new QRadioButton(task.option2);
        optionsGroup->addButton(radio, 1);
        optionsLayout->addWidget(radio);
    }
    if (!task.option3.isEmpty()) {
        QRadioButton* radio = new QRadioButton(task.option3);
        optionsGroup->addButton(radio, 2);
        optionsLayout->addWidget(radio);
    }
    if (!task.option4.isEmpty()) {
        QRadioButton* radio = new QRadioButton(task.option4);
        optionsGroup->addButton(radio, 3);
        optionsLayout->addWidget(radio);
    }
}

void GrammarMode::checkAnswer() {
    if (!currentSession.isActive) return;

    if (!optionsGroup->checkedButton()) {
        QMessageBox::warning(this, "Warning", "Please select an option.");
        return;
    }

    int selectedId = optionsGroup->checkedId();
    bool isCorrect = (selectedId == correctAnswerId);

    if (isCorrect) {
        currentSession.correctAnswers++;
        currentSession.score += calculateScore();
        progressBar->setValue(currentSession.correctAnswers);

        showFeedback(true);
        emit answerSubmitted(selectedId, true);
        emit scoreUpdated(currentSession.score);

        QTimer::singleShot(1000, this, [this]() {
            if (currentSession.correctAnswers >= totalTasks) {
                endSession(true);
            } else {
                loadTask(currentMode);
            }
        });
    } else {
        currentSession.incorrectAttempts++;
        showFeedback(false);
        emit answerSubmitted(selectedId, false);

        if (currentSession.incorrectAttempts >= maxAttempts) {
            endSession(false);
        }
    }
}

void GrammarMode::showFeedback(bool isCorrect) {
    QLabel* feedback = new QLabel(isCorrect ? "✓ Correct!" : "✗ Try again", this);
    feedback->setStyleSheet(
        isCorrect ? "color: green; font-size: 16px;" : "color: red; font-size: 16px;");
    feedback->setAlignment(Qt::AlignCenter);

    feedback->setGeometry(width() / 2 - 50, height() / 2 - 15, 100, 30);
    feedback->show();

    QPropertyAnimation* anim = new QPropertyAnimation(feedback, "geometry", this);
    anim->setDuration(1000);
    anim->setStartValue(feedback->geometry());
    anim->setEndValue(QRect(feedback->x(), feedback->y() - 15, feedback->width(), feedback->height()));
    anim->start(QAbstractAnimation::DeleteWhenStopped);

    QTimer::singleShot(1000, feedback, &QLabel::deleteLater);
}

int GrammarMode::calculateScore() {
    qint64 elapsedSec = currentSession.startTime.secsTo(QDateTime::currentDateTime());
    int baseScore = 10;
    int timeBonus = qMax(0, 5 - elapsedSec / 10);
    return baseScore + timeBonus;
}

void GrammarMode::keyPressEvent(QKeyEvent* event) {
    if (event && event->key() == Qt::Key_H) {
        QMessageBox::information(
            this, "Help",
            "Grammar tips:\n"
            "1. German nouns are always capitalized\n"
            "2. Articles change based on case\n"
            "3. Verb position varies in clauses\n"
            "4. Adjectives must agree in gender");
    }
    QWidget::keyPressEvent(event);
}

void GrammarMode::clearOptions() {
    qDeleteAll(optionsGroup->buttons());
    optionsGroup->buttons().clear();
    QLayoutItem* item;
    while ((item = optionsLayout->takeAt(0))) {
        delete item->widget();
        delete item;
    }
    correctAnswerId = -1;
}

void GrammarMode::updateTimerDisplay() {
    QString textColor = "color: ";
    if (remainingTime <= QTime(0, 0, 30)) {
        textColor += "red;";
    } else if (remainingTime <= QTime(0, 1, 0)) {
        textColor += "orange;";
    } else {
        textColor += "green;";
    }

    timerLabel->setStyleSheet("font-size: 16px; font-weight: bold; " + textColor);
    timerLabel->setText(
        QString("⏱ Time: %1:%2")
            .arg(remainingTime.minute(), 2, 10, QChar('0'))
            .arg(remainingTime.second(), 2, 10, QChar('0'))
    );
}