#include "translationexercise.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QTextEdit>
#include <QPushButton>
#include <QFont>
#include <algorithm>
#include <random>
#include <QDebug>
#include <QSizePolicy>


TranslationExercise::TranslationExercise(int difficultyLevel, QWidget *parent)
    : QWidget(parent),
    currentQuestion(0),
    attempts(0),
    correctAnswers(0),
    difficultyLevel(difficultyLevel),
    totalTimeRemaining(30) {


    questionLabel = new QLabel("Переведите:", this);
    questionLabel->setWordWrap(true);
    QFont questionFont = questionLabel->font();
    questionFont.setPointSize(20);
    questionLabel->setFont(questionFont);
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    timerLabel = new QLabel("Время: 30 сек", this);
    timerLabel->setAlignment(Qt::AlignCenter);
    QFont timerFont = timerLabel->font();
    timerFont.setPointSize(18);
    timerLabel->setFont(timerFont);
    timerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    answerEdit = new QTextEdit(this);
    QFont answerFont = answerEdit->font();
    answerFont.setPointSize(16);
    answerEdit->setFont(answerFont);
    answerEdit->setStyleSheet("border: 1px solid #ccc; border-radius: 8px; padding: 15px;");
    answerEdit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);


    submitButton = new QPushButton("Подтвердить", this);
    hintButton = new QPushButton("💡 Подсказка", this);
    QFont buttonFont;
    buttonFont.setPointSize(18);
    submitButton->setFont(buttonFont);
    hintButton->setFont(buttonFont);
    submitButton->setStyleSheet("background-color: #5cb85c; color: white; padding: 15px 30px; border-radius: 10px;");
    hintButton->setStyleSheet("background-color: #f0ad4e; color: white; padding: 12px 25px; border-radius: 10px;");
    submitButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    hintButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(timerLabel);
    mainLayout->addSpacing(40);
    mainLayout->addWidget(answerEdit, 1);
    mainLayout->addSpacing(30);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(hintButton);
    buttonLayout->addWidget(submitButton);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);


    connect(submitButton, &QPushButton::clicked, this, &TranslationExercise::checkAnswer);
    connect(hintButton, &QPushButton::clicked, this, &TranslationExercise::showHint);


    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, &TranslationExercise::updateTimer);


    loadQuestions();
    nextQuestion();

}


void TranslationExercise::setDifficulty(int level) {
    difficultyLevel = level;
    loadQuestions();
    nextQuestion();
    totalTimeRemaining = 30;
    updateTimerLabel();

}


QList<TranslationExercise::Question> loadTranslationQuestions(int level) {
    QList<TranslationExercise::Question> result;
    QFile file("labs/duolingo/exercises.json");
    if (!file.open(QIODevice::ReadOnly))
        return result;
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    QString key = QString("level_%1").arg(level);
    QJsonArray arr = obj["translation"][key].toArray();
    for (const QJsonValue& val : arr) {
        QJsonObject qObj = val.toObject();
        TranslationExercise::Question q;
        q.original = qObj["original"].toString();
        q.translation = qObj["translation"].toString();
        result.append(q);
    }
    return result;
}


void TranslationExercise::loadQuestions() {
    QList<TranslationExercise::Question> allQuestions = ::loadTranslationQuestions(difficultyLevel);
    questions.clear();
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(allQuestions.begin(), allQuestions.end(), g);
    if (allQuestions.size() <= 5) {
        questions = allQuestions;
    } else {
        for (int i = 0; i < 5; ++i) {
            questions.append(allQuestions[i]);
        }
    }
    currentQuestion = 0;
    totalTimeRemaining = 30;
    updateTimerLabel();
}


void TranslationExercise::nextQuestion() {
    if (currentQuestion >= questions.size()) {
        timer.stop();
        emit exerciseFinished();
        currentQuestion = 0;
        correctAnswers = 0;
        attempts = 0;
        loadQuestions();
        return;
    }


    questionLabel->setText(questions[currentQuestion].original);
    answerEdit->clear();
}


void TranslationExercise::checkAnswer() {
    timer.stop();
    QString userAnswer = answerEdit->toPlainText().trimmed();
    QString correctAnswer = questions[currentQuestion].translation;
    if (!userAnswer.isEmpty() && userAnswer.compare(correctAnswer, Qt::CaseInsensitive) == 0) {
        ++correctAnswers;
        ++currentQuestion;
        attempts = 0;
        if (currentQuestion < questions.size()) {
            nextQuestion();
            timer.start();
        } else {
            emit exerciseFinished();
        }
    } else {
        ++attempts;
        if (attempts >= 3) {
            emit gameOverSignal();
        } else {
            QMessageBox::warning(this, "Ошибка", "Неправильный перевод. Попробуйте еще раз.");
            timer.start();
        }
    }
}


void TranslationExercise::onTimeout() {
    timer.stop();
    emit gameOverSignal();
}


void TranslationExercise::showHint() {
    if (!questions.isEmpty()) {
        QString hint = QString("Переведите предложение:\n\n"
                               "%1\n\n"
                               "Правильный ответ: %2")
                           .arg(questions[currentQuestion].original)
                           .arg(questions[currentQuestion].translation);
        QMessageBox::information(this, "Подсказка", hint);
    }
}


void TranslationExercise::updateTimer() {
    totalTimeRemaining--;
    updateTimerLabel();
    if (totalTimeRemaining <= 0) {
        timer.stop();
        emit gameOverSignal();
    }
}


void TranslationExercise::updateTimerLabel() {
    timerLabel->setText(QString("Время: %1 сек").arg(totalTimeRemaining));
}


void TranslationExercise::startTimer() {
    timer.start(1000);
}


void TranslationExercise::stopTimer() {
    timer.stop();
}
