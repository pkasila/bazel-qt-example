#include "grammarexercise.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QFont>
#include <algorithm>
#include <random>
#include <QDebug>
#include <QSizePolicy>
#include <QResizeEvent>
#include <QApplication>
#include <QFontDatabase>


GrammarExercise::GrammarExercise(int difficultyLevel, QWidget *parent)
    : QWidget(parent),
    currentQuestion(0),
    attempts(0),
    correctAnswers(0),
    difficultyLevel(difficultyLevel),
    totalTimeRemaining(30),
    baseFontSize(16) // Базовый размер шрифта
{
    questionLabel = new QLabel("Вопрос", this);
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignCenter);
    questionLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    timerLabel = new QLabel("Время: 30 сек", this);
    timerLabel->setAlignment(Qt::AlignCenter);
    timerLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    optionA = new QRadioButton("Вариант A", this);
    optionB = new QRadioButton("Вариант B", this);
    optionC = new QRadioButton("Вариант C", this);
    optionA->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    optionB->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    optionC->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);


    submitButton = new QPushButton("Подтвердить", this);
    hintButton = new QPushButton("💡 Подсказка", this);
    submitButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);
    hintButton->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Preferred);


    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(40, 40, 40, 40);
    mainLayout->addWidget(questionLabel);
    mainLayout->addWidget(timerLabel);
    mainLayout->addSpacing(40);
    mainLayout->addWidget(optionA);
    mainLayout->addWidget(optionB);
    mainLayout->addWidget(optionC);
    mainLayout->addSpacing(30);
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addWidget(hintButton);
    buttonLayout->addWidget(submitButton);
    mainLayout->addLayout(buttonLayout);
    setLayout(mainLayout);


    connect(submitButton, &QPushButton::clicked, this, &GrammarExercise::checkAnswer);
    connect(hintButton, &QPushButton::clicked, this, &GrammarExercise::showHint);


    timer.setInterval(1000);
    connect(&timer, &QTimer::timeout, this, &GrammarExercise::updateTimer);


    loadQuestions();
    nextQuestion();


    adjustFontsAndSizes(size()); // Initial adjustment
}


void GrammarExercise::setDifficulty(int level) {
    difficultyLevel = level;
    loadQuestions();
    nextQuestion();
    totalTimeRemaining = 30;
    updateTimerLabel();
}


QList<GrammarExercise::Question> loadGrammarQuestions(int level) {
    QList<GrammarExercise::Question> result;
    QFile file("labs/duolingo/exercises.json");
    if (!file.open(QIODevice::ReadOnly)) {
        return result;
    }
    QJsonDocument doc = QJsonDocument::fromJson(file.readAll());
    QJsonObject obj = doc.object();
    QString key = QString("level_%1").arg(level);
    QJsonArray arr = obj["grammar"][key].toArray();
    for (const QJsonValue& val : arr) {
        QJsonObject qObj = val.toObject();
        GrammarExercise::Question q;
        q.sentence = qObj["sentence"].toString();
        q.correctOption = qObj["correct"].toString();
        QJsonArray optsArr = qObj["options"].toArray();
        for (const QJsonValue& v : optsArr)
            q.options.append(v.toString());
        result.append(q);
    }
    return result;
}


void GrammarExercise::loadQuestions() {
    QList<GrammarExercise::Question> allQuestions = ::loadGrammarQuestions(difficultyLevel);
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


void GrammarExercise::nextQuestion() {
    optionA->setChecked(false);
    optionB->setChecked(false);
    optionC->setChecked(false);


    if (currentQuestion >= questions.size()) {
        timer.stop();
        emit exerciseFinished();
        return;
    }


    Question q = questions[currentQuestion];
    questionLabel->setText(q.sentence);


    QStringList randomizedOptions = q.options;
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(randomizedOptions.begin(), randomizedOptions.end(), g);


    optionA->setText(randomizedOptions[0]);
    optionB->setText(randomizedOptions[1]);
    optionC->setText(randomizedOptions[2]);
    optionA->setChecked(true);
}


void GrammarExercise::checkAnswer() {
    timer.stop();
    QString selectedOption;
    if (optionA->isChecked())
        selectedOption = optionA->text();
    else if (optionB->isChecked())
        selectedOption = optionB->text();
    else if (optionC->isChecked())
        selectedOption = optionC->text();
    else {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, выберите вариант.");
        timer.start();
        return;
    }
    if (currentQuestion >= questions.size()) {
        emit exerciseFinished();
        return;
    }
    QString correctOption = questions[currentQuestion].correctOption;
    if (selectedOption.compare(correctOption, Qt::CaseInsensitive) == 0) {
        ++correctAnswers;
        ++currentQuestion;
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
            QMessageBox::warning(this, "Ошибка", "Неправильный вариант. Попробуйте еще раз.");
            timer.start();
        }
    }
}


void GrammarExercise::onTimeout() {
    timer.stop();
    emit gameOverSignal();
}


void GrammarExercise::showHint() {
    if (!questions.isEmpty()) {
        const auto& q = questions[currentQuestion];
        QString hint = QString("Выберите правильный вариант:\n\n"
                               "%1\n\n"
                               "Правильный ответ: %2")
                           .arg(q.sentence)
                           .arg(q.correctOption);
        QMessageBox::information(this, "Подсказка", hint);
    }
}


void GrammarExercise::updateTimer() {
    totalTimeRemaining--;
    updateTimerLabel();
    if (totalTimeRemaining <= 0) {
        timer.stop();
        emit gameOverSignal();
    }
}


void GrammarExercise::updateTimerLabel() {
    timerLabel->setText(QString("Время: %1 сек").arg(totalTimeRemaining));
}


void GrammarExercise::startTimer() {
    timer.start(1000);
}


void GrammarExercise::stopTimer() {
    timer.stop();
}


void GrammarExercise::resizeEvent(QResizeEvent *event) {
    QWidget::resizeEvent(event);
    adjustFontsAndSizes(event->size());
}


void GrammarExercise::adjustFontsAndSizes(const QSize& newSize) {
    if (newSize.isEmpty()) return;


    // Determine a base dimension to scale from (e.g., height)
    int baseDimension = std::min(newSize.width(), newSize.height());


    // Scale factor based on the base dimension
    qreal scaleFactor = static_cast<qreal>(baseDimension) / 600.0; // 600 is an arbitrary base size
    if (scaleFactor < 0.5) scaleFactor = 0.5; // Ensure minimum size
    if (scaleFactor > 2.0) scaleFactor = 2.0; // Ensure maximum size


    QFont questionFont = questionLabel->font();
    questionFont.setPointSize(static_cast<int>(baseFontSize * 1.5 * scaleFactor));
    questionLabel->setFont(questionFont);


    QFont timerFont = timerLabel->font();
    timerFont.setPointSize(static_cast<int>(baseFontSize * 1.2 * scaleFactor));
    timerLabel->setFont(timerFont);


    QFont optionFont = optionA->font();
    optionFont.setPointSize(static_cast<int>(baseFontSize * scaleFactor));
    optionA->setFont(optionFont);
    optionB->setFont(optionFont);
    optionC->setFont(optionFont);


    QFont buttonFont = submitButton->font();
    buttonFont.setPointSize(static_cast<int>(baseFontSize * 1.1 * scaleFactor));
    submitButton->setFont(buttonFont);
    hintButton->setFont(buttonFont);


    // Adjust button padding dynamically
    submitButton->setStyleSheet(QString("background-color: #5cb85c; color: white; padding: %1px %2px; border-radius: %3px;")
                                    .arg(static_cast<int>(10 * scaleFactor)).arg(static_cast<int>(20 * scaleFactor)).arg(static_cast<int>(8 * scaleFactor)));
    hintButton->setStyleSheet(QString("background-color: #f0ad4e; color: white; padding: %1px %2px; border-radius: %3px;")
                                  .arg(static_cast<int>(8 * scaleFactor)).arg(static_cast<int>(15 * scaleFactor)).arg(static_cast<int>(8 * scaleFactor)));


    // You might also want to adjust layout spacing based on the scale factor
    QVBoxLayout* layout = qobject_cast<QVBoxLayout*>(this->layout());
    if (layout) {
        layout->setContentsMargins(static_cast<int>(20 * scaleFactor), static_cast<int>(20 * scaleFactor), static_cast<int>(20 * scaleFactor), static_cast<int>(20 * scaleFactor));
        layout->setSpacing(static_cast<int>(15 * scaleFactor));
    }


    QHBoxLayout* buttonLayout = qobject_cast<QHBoxLayout*>(layout->itemAt(layout->count() - 1)->layout());
    if (buttonLayout) {
        buttonLayout->setSpacing(static_cast<int>(10 * scaleFactor));
    }
}
