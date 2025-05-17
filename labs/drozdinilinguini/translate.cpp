#include "translate.h"
#include <QMessageBox>

std::vector<TranslateMode::TranslateTask> TranslateMode::tasks;

void TranslateMode::loadTasksFromMemory() {
    tasks = {
      
    {1, "Hello", "Hallo", 1},
    {2, "Good morning", "Guten Morgen", 1},
    {3, "Good evening", "Guten Abend", 1},
    {4, "Good night", "Gute Nacht", 1},
    {5, "Thank you", "Danke", 1},
    {6, "Please", "Bitte", 1},
    {7, "Excuse me", "Entschuldigung", 1},
    {8, "Yes", "Ja", 1},
    {9, "No", "Nein", 1},
    {10, "I don't understand", "Ich verstehe nicht", 1},
    {11, "Please speak slowly", "Bitte sprechen Sie langsam", 1},
    {12, "My name is...", "Ich heiße...", 1},
    {13, "What is your name?", "Wie heißt du?", 1},
    {14, "How are you?", "Wie geht es dir?", 1},
    {15, "I'm fine", "Mir geht es gut", 1},
    {16, "Goodbye", "Auf Wiedersehen", 1},
    {17, "See you later", "Bis später", 1},
    {18, "Today", "Heute", 1},
    {19, "Tomorrow", "Morgen", 1},
    {20, "Yesterday", "Gestern", 1},

            
    {21, "I am learning German", "Ich lerne Deutsch", 2},
    {22, "The book is on the table", "Das Buch ist auf dem Tisch", 2},
    {23, "I would like a coffee", "Ich hätte gern einen Kaffee", 2},
    {24, "Where is the train station?", "Wo ist der Bahnhof?", 2},
    {25, "How much does it cost?", "Wie viel kostet das?", 2},
    {26, "Do you speak English?", "Sprechen Sie Englisch?", 2},
    {27, "I need help", "Ich brauche Hilfe", 2},
    {28, "What time is it?", "Wie spät ist es?", 2},
    {29, "I don't know", "Ich weiß nicht", 2},
    {30, "I'm sorry", "Es tut mir leid", 2},
    {31, "Where are you from?", "Woher kommst du?", 2},
    {32, "I'm from...", "Ich komme aus...", 2},
    {33, "Can you repeat that?", "Können Sie das wiederholen?", 2},
    {34, "I love this", "Ich liebe das", 2},
    {35, "I don't like that", "Das gefällt mir nicht", 2},
    {36, "How old are you?", "Wie alt bist du?", 2},
    {37, "I am hungry", "Ich habe Hunger", 2},
    {38, "I am thirsty", "Ich habe Durst", 2},
    {39, "Let's go", "Lass uns gehen", 2},
    {40, "Call the police", "Rufen Sie die Polizei", 2},
         
    {41, "If I had more time, I would travel more", 
        "Wenn ich mehr Zeit hätte, würde ich mehr reisen", 3},
    {42, "The project must be completed by Friday", 
        "Das Projekt muss bis Freitag abgeschlossen sein", 3},
    {43, "Despite the rain, we went for a walk", 
        "Trotz des Regens sind wir spazieren gegangen", 3},
    {44, "I wish I had studied harder", 
        "Ich wünschte, ich hätte härter gelernt", 3},
    {45, "She said that she would call later", 
        "Sie sagte, dass sie später anrufen würde", 3},
    {46, "The meeting was postponed due to technical issues", 
        "Die Sitzung wurde wegen technischer Probleme verschoben", 3},
    {47, "If you had told me earlier, I could have helped", 
        "Wenn du es mir früher gesagt hättest, hätte ich helfen können", 3},
    {48, "This is the best book I've ever read", 
        "Das ist das beste Buch, das ich je gelesen habe", 3},
    {49, "He works as if he never gets tired", 
        "Er arbeitet, als ob er nie müde wird", 3},
    {50, "It's not as easy as it seems", 
        "Es ist nicht so einfach, wie es scheint", 3},
    {51, "The decision depends on various factors", 
        "Die Entscheidung hängt von verschiedenen Faktoren ab", 3},
    {52, "I would have come if I had known", 
        "Ich wäre gekommen, wenn ich gewusst hätte", 3},
    {53, "The results were better than expected", 
        "Die Ergebnisse waren besser als erwartet", 3},
    {54, "She speaks as though she were a native", 
        "Sie spricht, als wäre sie Muttersprachlerin", 3},
    {55, "We should have taken the other route", 
        "Wir hätten die andere Route nehmen sollen", 3},
    {56, "It's a matter of principle", 
        "Es ist eine Prinzipienfrage", 3},
    {57, "The contract was signed under certain conditions", 
        "Der Vertrag wurde unter bestimmten Bedingungen unterschrieben", 3},
    {58, "He acted as if nothing had happened", 
        "Er tat so, als wäre nichts passiert", 3},
    {59, "This needs to be clarified as soon as possible", 
        "Das muss so schnell wie möglich geklärt werden", 3},
    {60, "The proposal was rejected without further discussion", 
        "Der Vorschlag wurde ohne weitere Diskussion abgelehnt", 3},
    };
}

TranslateMode::TranslateMode(QWidget* parent) : QWidget(parent) {
    setupUI();
}

void TranslateMode::setupUI() {
    layout = new QVBoxLayout(this);
    layout->setSpacing(15);

    instructionLabel = new QLabel("Translate the following sentence into German:");
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

    sentenceLabel = new QLabel;
    sentenceLabel->setWordWrap(true);
    sentenceLabel->setStyleSheet("font-size: 14px;");
    layout->addWidget(sentenceLabel);

    answerInput = new QTextEdit;
    answerInput->setMaximumHeight(100);
    answerInput->setStyleSheet("font-size: 14px;");
    answerInput->setEnabled(false);
    layout->addWidget(answerInput);

    submitButton = new QPushButton("Submit");
    submitButton->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; padding: 10px; font-size: 14px; }"
        "QPushButton:hover { background-color: #45a049; }");
    submitButton->setEnabled(false);
    layout->addWidget(submitButton);

    timerLabel = new QLabel(this);
    timerLabel->setStyleSheet("font-size: 16px; font-weight: bold;");
    timerLabel->setAlignment(Qt::AlignCenter);
    layout->insertWidget(1, timerLabel);

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

    connect(startButton, &QPushButton::clicked, this, &TranslateMode::startSession);
    connect(submitButton, &QPushButton::clicked, this, &TranslateMode::checkAnswer);
}

void TranslateMode::setMode(int mode) {
    currentMode = mode;
}

void TranslateMode::startSession() {
    currentSession = SessionState();
    currentSession.isActive = true;
    currentSession.startTime = QDateTime::currentDateTime();

    remainingTime = QTime(0, 2, 0);
    updateTimerDisplay();

    progressBar->setValue(0);
    answerInput->setEnabled(true);
    submitButton->setEnabled(true);
    startButton->setEnabled(false);

    timer->start(1000);
    loadTask();
}

void TranslateMode::endSession(bool success) {
    currentSession.isActive = false;
    timer->stop();

    answerInput->setEnabled(false);
    submitButton->setEnabled(false);
    startButton->setEnabled(true);

    QString message =
        success ? QString("Congratulations! You scored %1 points").arg(currentSession.score)
                : QString("Session ended. You scored %1 points").arg(currentSession.score);

    QMessageBox::information(this, "Session Ended", message);
    emit sessionCompleted(success);
}

void TranslateMode::loadTask() {
    if (tasks.empty()) loadTasksFromMemory();

    std::vector<TranslateTask> availableTasks;
    for (const auto& task : tasks) {
        if (task.difficulty == currentMode) {
            availableTasks.push_back(task);
        }
    }

    if (availableTasks.empty()) {
        sentenceLabel->setText("No tasks available for this difficulty level!");
        endSession(false);
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(static_cast<int>(availableTasks.size()));
    const auto& task = availableTasks[randomIndex];
    
    sentenceLabel->setText(task.english);
    correctAnswer = task.german;
    answerInput->clear();
}

void TranslateMode::checkAnswer() {
    if (!currentSession.isActive) return;

    QString userAnswer = answerInput->toPlainText().trimmed();
    if (userAnswer.isEmpty()) {
        QMessageBox::warning(this, "Warning", "Please enter a translation.");
        return;
    }

    bool isCorrect = (userAnswer.compare(correctAnswer, Qt::CaseInsensitive) == 0);

    if (isCorrect) {
        currentSession.correctAnswers++;
        currentSession.score += calculateScore();
        progressBar->setValue(currentSession.correctAnswers);

        showFeedback(true);
        emit answerSubmitted(true);
        emit scoreUpdated(currentSession.score);

        QTimer::singleShot(1000, this, [this]() {
            if (currentSession.correctAnswers >= totalTasks) {
                endSession(true);
            } else {
                loadTask();
            }
        });
    } else {
        currentSession.incorrectAttempts++;
        showFeedback(false);
        emit answerSubmitted(false);

        if (currentSession.incorrectAttempts >= maxAttempts) {
            endSession(false);
        }
    }
}

void TranslateMode::showFeedback(bool isCorrect) {
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

int TranslateMode::calculateScore() {
    qint64 elapsedSec = currentSession.startTime.secsTo(QDateTime::currentDateTime());
    int baseScore = 10;
    int timeBonus = qMax(0, 5 - elapsedSec / 10);
    return baseScore + timeBonus;
}

void TranslateMode::keyPressEvent(QKeyEvent* event) {
    if (event && event->key() == Qt::Key_H) {
        QMessageBox::information(
            this, "Help",
            "Translation tips:\n"
            "1. German nouns are always capitalized\n"
            "2. Verb position depends on sentence structure\n"
            "3. Pay attention to articles (der/die/das)\n"
            "4. Adjectives must match case and gender");
    }
    QWidget::keyPressEvent(event);
}

void TranslateMode::updateTimerDisplay() {
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