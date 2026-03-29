#include "grammar.h"
#include "mainwindow.h"

#include <QButtonGroup>
#include <QMessageBox>

Grammar::Grammar(QWidget *parent)
    : QWidget(parent),
    qIdx(0),
    maxAttempts(5),
    currentFails(0),
    score(0),
    pointsPerQ(1),
    difficultyLevel(tr("Лёгкий")),
    timeAllowedPerQ(30),
    timeRemaining(0) {

    setupUi();
    applyStyle();
    loadDifficultySettings();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Grammar::onQuestionTimerTimeout);

    connect(submitBtn, &QPushButton::clicked, this, &Grammar::onSubmitButtonClicked);
    connect(startBtn, &QPushButton::clicked, this, &Grammar::onStartButtonClicked);

    enableControls(false);
    questionTextLabel->setText(tr("Нажмите 'Начать', чтобы загрузить вопросы."));
    progressBar->setVisible(false);
    infoLabel->setVisible(false);
    timeLabel->setVisible(false);
}

Grammar::~Grammar() {
}

void Grammar::setupUi() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    topInfoLayout = new QHBoxLayout();

    infoLabel = new QLabel(this);
    timeLabel = new QLabel(this);
    timeLabel->setAlignment(Qt::AlignRight);
    progressBar = new QProgressBar(this);
    progressBar->setTextVisible(true);

    topInfoLayout->addWidget(infoLabel, 1);
    topInfoLayout->addWidget(timeLabel, 0);

    questionTextLabel = new QLabel(this);
    questionTextLabel->setWordWrap(true);
    questionTextLabel->setMinimumHeight(30);
    questionTextLabel->setMaximumHeight(60);
    questionTextLabel->setAlignment(Qt::AlignCenter);

    optionsLayout = new QVBoxLayout();
    optionsLayout->setSpacing(10);
    optionGroup = new QButtonGroup(this);
    optionGroup->setExclusive(true);

    for (int i = 0; i < 4; ++i) {
        QRadioButton *rb = new QRadioButton(this);
        rb->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        optionButtons.append(rb);
        optionGroup->addButton(rb, i);
        optionsLayout->addWidget(rb);
    }

    startBtn = new QPushButton(tr("Начать"), this);
    submitBtn = new QPushButton(tr("Ответить"), this);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(startBtn);
    buttonsLayout->addWidget(submitBtn);

    mainLayout->addLayout(topInfoLayout);
    mainLayout->addWidget(progressBar);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(questionTextLabel, 1);
    mainLayout->addSpacing(15);
    mainLayout->addLayout(optionsLayout);
    mainLayout->addStretch(1);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
}

void Grammar::applyStyle() {
    infoLabel->setObjectName("questionNumberLabel");
    timeLabel->setObjectName("timerLabel");
    progressBar->setObjectName("progressBar");
    questionTextLabel->setObjectName("questionLabel");
    startBtn->setObjectName("startButton");
    submitBtn->setObjectName("submitButton");
    for (auto rb : optionButtons) {
        rb->setObjectName("optionRadio");
    }

    setStyleSheet(R"(
        QWidget {
            background: #2b2b2b;
            color: #fff;
            font: 16px 'Segoe UI', sans-serif;
        }
        QLabel#questionNumberLabel {
            font-size: 14px;
            color: #cccccc;
        }
        QLabel#timerLabel {
            font-size: 14px;
            font-weight: bold;
            color: #e8a865;
        }
        QProgressBar#progressBar {
            min-height: 12px;
            max-height: 12px;
            border-radius: 6px;
            text-align: center;
            color: #ffffff;
            font-size: 15px;
        }
        QProgressBar#progressBar::chunk {
            background-color: #1abc9c;
            border-radius: 6px;
        }
        QLabel#questionLabel {
            background: white;
            font: bold;
            font-size: 22px;
            color: #3c3f41;
            border: 1px solid #5A9BD5;
            border-radius: 8px;
            padding: 15px;
            text-align: center;
        }
        QRadioButton#optionRadio {
            background: #3c3f41;
            border: 1px solid #5A9BD5;
            border-radius: 8px;
            padding: 10px;
            font-size: 15px;
        }
        QRadioButton#optionRadio::indicator {
            width: 16px;
            height: 16px;
        }
        QRadioButton#optionRadio:hover {
            background: #505355;
        }
        QPushButton#submitButton:hover { background: #16a085; }
        QPushButton#startButton, QPushButton#submitButton {
            background: #4682B4;
            color: white;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 15px;
        }
        QPushButton:hover {
            background: #5A9BD5;
        }
        QPushButton:disabled {
            background: #888;
            color: #eee;
        }
    )");
}

void Grammar::enableControls(bool enable) {
    submitBtn->setEnabled(enable);
    for(QRadioButton* rb : optionButtons) {
        rb->setEnabled(enable);
    }
    if (!enable) {
        clearOptions();
        if (timer->isActive()) {
            timer->stop();
        }
        timeLabel->setText("");
    }
}

void Grammar::clearOptions() {
    optionGroup->setExclusive(false);
    for (QRadioButton *button : optionButtons) {
        button->setChecked(false);
        button->setText("");
        button->setVisible(false);
    }
    optionGroup->setExclusive(true);
}

void Grammar::startExercise(const QString &level, const QVector<Question> &newQuestions, int timePerQSec) {
    this->difficultyLevel = level;
    this->timeAllowedPerQ = timePerQSec > 0 ? timePerQSec : 30;
    loadDifficultySettings();

    allQuestions = newQuestions;
    qIdx = 0;
    score = 0;

    if (allQuestions.isEmpty()) {
        questionTextLabel->setText(tr("Нет доступных вопросов."));
        QMessageBox::warning(this,tr("Внимание"), tr("Нет доступных вопросов для этого уровня.\nПожалуйста, добавьте вопросы через меню 'Редактировать'."));
        enableControls(false);
        startBtn->setText(tr("Начать"));
        progressBar->setVisible(false);
        infoLabel->setVisible(false);
        timeLabel->setVisible(false);
        return;
    }

    progressBar->setRange(0, allQuestions.size());
    progressBar->setVisible(true);
    infoLabel->setVisible(true);
    timeLabel->setVisible(true);

    displayQuestion();
    enableControls(true);
    startBtn->setText(tr("Перезапустить"));
}

void Grammar::updateLevel(const QString &level) {
    this->difficultyLevel = level;
    loadDifficultySettings();
}

void Grammar::loadDifficultySettings() {
    if (difficultyLevel == tr("Лёгкий")) {
        maxAttempts = 3;
        pointsPerQ = 1;
        timeAllowedPerQ = 60;
    } else if (difficultyLevel == tr("Средний")) {
        maxAttempts = 2;
        pointsPerQ = 2;
        timeAllowedPerQ = 30;
    } else if (difficultyLevel == tr("Сложный")) {
        maxAttempts = 1;
        pointsPerQ = 3;
        timeAllowedPerQ = 10;
    }
}

void Grammar::displayQuestion() {
    if (allQuestions.isEmpty() || qIdx >= allQuestions.size()) {
        return;
    }

    clearOptions();
    updateUiProgress();

    const Question &q = allQuestions[qIdx];
    questionTextLabel->setText(q.question);

    for (int i = 0; i < q.options.size(); ++i) {
        if (i < optionButtons.size()) {
            optionButtons[i]->setText(q.options[i]);
            optionButtons[i]->setVisible(true);
        }
    }
    for (int i = q.options.size(); i < optionButtons.size(); ++i) {
        optionButtons[i]->setVisible(false);
    }

    currentFails = 0;
    timeRemaining = timeAllowedPerQ;
    timeLabel->setText(tr("Время: %1 сек").arg(timeRemaining));
    timer->start(1000);
}

void Grammar::nextQuestion() {
    if(timer->isActive()) {
        timer->stop();
    }
    qIdx++;
    if (qIdx < allQuestions.size()) {
        displayQuestion();
        submitBtn->setEnabled(true);
    } else {
        enableControls(false);
        questionTextLabel->setText(tr("Упражнение завершено!"));
        infoLabel->setText(tr("Итог: %1 очков").arg(score));
        timeLabel->setText("");
        progressBar->setValue(allQuestions.size());
        startBtn->setText(tr("Начать заново"));
        emit exerciseComplete(score);
    }
}

void Grammar::checkAnswer(bool timedOut) {
    timer->stop();

    if (qIdx >= allQuestions.size()) return;

    const Question currentQ = allQuestions[qIdx];

    if (timedOut) {
        emit answerIncorrect();
        emit timeExpiredForQuestion();
        QMessageBox::warning(this, tr("Время вышло!"), tr("Время на ответ истекло.\nПравильный ответ: %1").arg(currentQ.options.at(currentQ.correctOption)));
        nextQuestion();
        return;
    }

    const int checked_id = optionGroup->checkedId();
    if (checked_id == -1) {
        QMessageBox::warning(this, tr("Внимание"), tr("Пожалуйста, выберите вариант ответа."));
        if (timeRemaining > 0) {
            timer->start(1000);
        }
        return;
    }

    if (checked_id == currentQ.correctOption) {
        score += pointsPerQ;
        nextQuestion();
    } else {
        currentFails++;
        emit answerIncorrect();
        int const attempts_left = maxAttempts - currentFails;
        if (attempts_left > 0) {
            QMessageBox::warning(this, tr("Результат"), tr("Неправильно. Осталось попыток: %1").arg(attempts_left));
            if (timeRemaining > 0) {
                timer->start(1000);
            }
        } else {
            nextQuestion();
        }
    }
}

void Grammar::onSubmitButtonClicked() {
    if (allQuestions.isEmpty() || qIdx >= allQuestions.size()) {
        return;
    }
    checkAnswer();
}

void Grammar::onStartButtonClicked() {
    startExercise(difficultyLevel, allQuestions, timeAllowedPerQ);
}

int Grammar::getScore() const {
    return score;
}

void Grammar::onQuestionTimerTimeout() {
    timeRemaining--;
    if (timeRemaining >= 0) {
        timeLabel->setText(tr("Время: %1 сек").arg(timeRemaining));
    } else {
        checkAnswer(true);
    }
}

void Grammar::updateUiProgress() {
    if (!allQuestions.isEmpty()) {
        infoLabel->setText(tr("Вопрос %1 из %2").arg(qIdx + 1).arg(allQuestions.size()));
        progressBar->setValue(qIdx);
    }
}

QTimer* Grammar::getTimer() {
    return timer;
}
