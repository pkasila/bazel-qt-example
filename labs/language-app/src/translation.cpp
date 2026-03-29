#include "translation.h"
#include "mainwindow.h"

#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMessageBox>
#include <QProgressBar>
#include <QTimer>

Translation::Translation(QWidget *parent)
    : QWidget(parent),
    itemIdx(0),
    maxAttempts(3),
    currentFails(0),
    score(0),
    pointsPerItem(1),
    difficultyLevel(tr("Лёгкий")),
    timeAllowedPerItem(60),
    timeRemaining(0) {

    initGui();
    applyStyle();
    loadDifficultySettings();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Translation::onItemTimerTimeout);

    connect(submitBtn, &QPushButton::clicked, this, &Translation::onSubmitBtnClicked);
    connect(startBtn, &QPushButton::clicked, this, &Translation::onStartBtnClicked);

    enableControls(false);
    sourceTextLabel->setText(tr("Нажмите 'Начать', чтобы загрузить предложения."));
    progressBar->setVisible(false);
    infoLabel->setVisible(false);
    timeLabel->setVisible(false);
}

Translation::~Translation() {
}

void Translation::initGui() {
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

    sourceTextLabel = new QLabel(this);
    sourceTextLabel->setWordWrap(true);
    sourceTextLabel->setMinimumHeight(60);
    sourceTextLabel->setAlignment(Qt::AlignCenter);

    inputEdit = new QTextEdit(this);
    inputEdit->setPlaceholderText(tr("Введите ваш перевод здесь..."));
    inputEdit->setAcceptRichText(false);
    inputEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    startBtn = new QPushButton(tr("Начать"), this);
    submitBtn = new QPushButton(tr("Отправить перевод"), this);

    buttonsLayout = new QHBoxLayout();
    buttonsLayout->addWidget(startBtn);
    buttonsLayout->addWidget(submitBtn);

    mainLayout->addLayout(topInfoLayout);
    mainLayout->addWidget(progressBar);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(sourceTextLabel, 0);
    mainLayout->addSpacing(10);
    mainLayout->addWidget(inputEdit, 1);
    mainLayout->addStretch(0);
    mainLayout->addLayout(buttonsLayout);

    setLayout(mainLayout);
}

void Translation::applyStyle() {
    infoLabel->setObjectName("sentenceNumberLabel");
    timeLabel->setObjectName("timerLabel");
    progressBar->setObjectName("progressBar");
    sourceTextLabel->setObjectName("originalSentenceLabel");
    inputEdit->setObjectName("translationInput");
    startBtn->setObjectName("startButton");
    submitBtn->setObjectName("submitButton");

    setStyleSheet(R"(
        QWidget {
            background: #2b2b2b;
            color: #fff;
            font: 16px 'Segoe UI', sans-serif;
        }
        QLabel#sentenceNumberLabel {
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
        QLabel#originalSentenceLabel {
            background: #3c3f41;
            font: 18px 'Segoe UI', sans-serif;
            color: #e0e0e0;
            border: 1px solid #555;
            border-radius: 8px;
            padding: 15px;
            text-align: left;
        }
        QTextEdit#translationInput {
            background: white;
            font-size: 16px;
            color: #2b2b2b;
            border: 1px solid #5A9BD5;
            border-radius: 8px;
            padding: 10px;
        }
        QPushButton#startButton {
            background: #e67e22;
            color: white;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 15px;
        }
        QPushButton#startButton, QPushButton#submitButton {
            background: #4682B4;
            color: white;
            border-radius: 8px;
            padding: 8px 15px;
            font-size: 15px;
        }
        QPushButton#submitButton:hover { background: #16a085; }
        QPushButton:disabled {
            background: #888;
            color: #eee;
        }
    )");
}

void Translation::enableControls(bool enable) {
    submitBtn->setEnabled(enable);
    inputEdit->setEnabled(enable);
    if (!enable) {
        inputEdit->clear();
        if (timer->isActive()) {
            timer->stop();
        }
        timeLabel->setText("");
    }
}

void Translation::startExercise(const QString &level, const QVector<Sentence> &newSentences, int timePerItemSec) {
    this->difficultyLevel = level;
    this->timeAllowedPerItem = timePerItemSec > 0 ? timePerItemSec : 60;
    loadDifficultySettings();

    allSentences = newSentences;
    itemIdx = 0;
    score = 0;
    currentFails = 0;

    if (allSentences.isEmpty()) {
        sourceTextLabel->setText(tr("Нет доступных предложений."));
        QMessageBox::warning(this,tr("Внимание"), tr("Нет доступных предложений для этого уровня.\nПожалуйста, добавьте их через меню 'Редактировать'."));
        enableControls(false);
        startBtn->setText(tr("Начать"));
        progressBar->setVisible(false);
        infoLabel->setVisible(false);
        timeLabel->setVisible(false);
        return;
    }

    progressBar->setRange(0, allSentences.size());
    progressBar->setVisible(true);
    infoLabel->setVisible(true);
    timeLabel->setVisible(true);

    displayItem();
    enableControls(true);
    inputEdit->setFocus();
    startBtn->setText(tr("Перезапустить"));
}

void Translation::updateLevel(const QString &level) {
    this->difficultyLevel = level;
    loadDifficultySettings();
}

void Translation::loadDifficultySettings() {
    if (difficultyLevel == tr("Лёгкий")) {
        maxAttempts = 5;
        pointsPerItem = 1;
        timeAllowedPerItem = 90;
    } else if (difficultyLevel == tr("Средний")) {
        maxAttempts = 3;
        pointsPerItem = 2;
        timeAllowedPerItem = 60;
    } else if (difficultyLevel == tr("Сложный")) {
        maxAttempts = 1;
        pointsPerItem = 3;
        timeAllowedPerItem = 30;
    }
}

void Translation::displayItem() {
    if (allSentences.isEmpty() || itemIdx >= allSentences.size()) {
        return;
    }

    updateUiProgress();
    inputEdit->clear();

    const Sentence &s = allSentences[itemIdx];
    sourceTextLabel->setText(s.original);

    currentFails = 0;

    timeRemaining = timeAllowedPerItem;
    timeLabel->setText(tr("Время: %1 сек").arg(timeRemaining));
    timer->start(1000);
}

void Translation::nextItem() {
    if (timer->isActive()) {
        timer->stop();
    }
    itemIdx++;
    if (itemIdx < allSentences.size()) {
        displayItem();
        inputEdit->setFocus();
        submitBtn->setEnabled(true);
    } else {
        enableControls(false);
        sourceTextLabel->setText(tr("Упражнение завершено!"));
        infoLabel->setText(tr("Итог: %1 очков").arg(score));
        timeLabel->setText("");
        progressBar->setValue(allSentences.size());
        startBtn->setText(tr("Начать заново"));
        emit exerciseComplete(score);
    }
}

void Translation::checkAnswer(bool timedOut) {
    if (timer->isActive()) {
        timer->stop();
    }

    if (itemIdx >= allSentences.size()) return;

    QString userAnswer = inputEdit->toPlainText().trimmed();
    const Sentence &s = allSentences[itemIdx];

    if (timedOut) {
        emit answerIncorrect();
        emit timeExpiredForItem();
        QMessageBox::warning(this, tr("Время вышло!"), tr("Время на перевод истекло."));
        nextItem();
        return;
    }

    if (userAnswer.isEmpty()) {
        QMessageBox::warning(this, tr("Внимание"), tr("Пожалуйста, введите ваш перевод."));
        if(timeRemaining > 0) timer->start(1000);
        return;
    }

    if (userAnswer.compare(s.translation, Qt::CaseInsensitive) == 0) {
        score += pointsPerItem;
        nextItem();
    } else {
        currentFails++;
        emit answerIncorrect();
        int attemptsLeft = maxAttempts - currentFails;

        if (attemptsLeft > 0) {
            QMessageBox::warning(this, tr("Результат"), tr("Неправильно. Осталось попыток: %1").arg(attemptsLeft));
            if(timeRemaining > 0) timer->start(1000);
        } else {
            nextItem();
        }
    }
}

void Translation::onSubmitBtnClicked() {
    if (allSentences.isEmpty() || itemIdx >= allSentences.size()) {
        return;
    }
    checkAnswer();
}

void Translation::onStartBtnClicked() {
    startExercise(difficultyLevel, allSentences, timeAllowedPerItem);
}

int Translation::getScore() const {
    return score;
}

void Translation::onItemTimerTimeout() {
    timeRemaining--;
    if (timeRemaining >= 0) {
        timeLabel->setText(tr("Время: %1 сек").arg(timeRemaining));
    } else {
        timer->stop();
        checkAnswer(true);
    }
}

void Translation::updateUiProgress() {
    if (!allSentences.isEmpty()) {
        infoLabel->setText(tr("Предложение %1 из %2").arg(itemIdx + 1).arg(allSentences.size()));
        progressBar->setValue(itemIdx);
    }
}

QTimer* Translation::getTimer() const {
    return timer;
}
