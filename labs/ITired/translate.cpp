#include "translate.h"
#include <QDialog>
#include <QUrl>
#include <QAudioOutput>
#include <QFile>
#include <QCoreApplication>

Translate::Translate(QWidget *parent)
    : QWidget{parent}
{


    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* upL = new QHBoxLayout();

    QVBoxLayout*  midL = new QVBoxLayout();

    QVBoxLayout* downL = new QVBoxLayout();

    correctAns = new QSoundEffect();
    correctAns->setSource(QUrl("qrc:/audio/correct_ans.wav"));

    notCorrectAns = new QSoundEffect();
    notCorrectAns->setSource(QUrl("qrc:/audio/bad_answer.wav"));

    levelComplete = new QSoundEffect();
    levelComplete->setSource(QUrl("qrc:/audio/level_complete.wav"));

    lost = new QSoundEffect();
    lost->setSource(QUrl("qrc:/audio/lives_lost.wav"));

    click = new QSoundEffect();
    click->setSource(QUrl("qrc:/audio/click.wav"));
    /*
    QString soundsPath = QCoreApplication::applicationDirPath() + "/audio/";

    correctAns = new QSoundEffect(this);
    correctAns->setSource(QUrl::fromLocalFile(soundsPath + "correct_ans.wav"));

    notCorrectAns = new QSoundEffect(this);
    notCorrectAns->setSource(QUrl::fromLocalFile(soundsPath + "bad_answer.wav"));

    levelComplete = new QSoundEffect(this);
    levelComplete->setSource(QUrl::fromLocalFile(soundsPath + "level_complete.wav"));

    lost = new QSoundEffect(this);
    lost->setSource(QUrl::fromLocalFile(soundsPath + "lives_lost.wav"));

    click = new QSoundEffect(this);
    click->setSource(QUrl::fromLocalFile(soundsPath + "click.wav"));*/

    attemptsL = new QLabel(QString("You have %1 attemts").arg(attempts));
    timeL = new QLabel(QString("You have %1 seconds").arg(seconds));
    scoreL = new QLabel(QString("Your score is %1").arg(localScore));
    helpButton = new QPushButton("help");
    question = new QLabel(currentQuestions[currentQuestionIndex].que);
    textEdit = new QTextEdit();
    progress = new QProgressBar();
    progress->setRange(0, numTasks);
    progress->setValue(0);
    submit = new QPushButton("submit");
    timer = new QTimer();

    upL->addWidget(attemptsL);
    upL->addWidget(timeL);
    upL->addWidget(scoreL);
    upL->addWidget(helpButton);

    midL->addWidget(question);
    midL->addWidget(textEdit);

    downL->addWidget(progress);
    downL->addWidget(submit);

    layout->addLayout(upL);
    layout->addLayout(midL);
    layout->addLayout(downL);

    connect(this, &Translate::start, timer, [&]() {
        timer->start(1000);
        //correctAns->play();
        qDebug() << "Timer starts";
    });
    connect(submit, &QPushButton::clicked, this, &Translate::checkans);

    connect(this, &Translate::end, timer, [&]() {
        timer->stop();
        reset();
    });

    connect(timer, &QTimer::timeout, this, [&]() {
        seconds--;
        if (seconds == 0) {
            if (tasksD != numTasks) {
                lost->play();
            } else {
                levelComplete->play();
            }
            showDialog(QString("The time is over! You have %1 correct answers. You have earned %2 points").arg(wellDone).arg(localScore));
            emit end();
            reset();
            update();
        }
        timeL->setText(QString("You have %1 seconds").arg(seconds));
    });

    connect(helpButton, &QPushButton::clicked, this, [&]() {
        click->play();
        showDialog(currentQuestions[currentQuestionIndex].hint);
        if (localScore >= 2)
            localScore-=2;
        update();
    });
}

void Translate::reset() {
    progress->setValue(0);
    attempts = 6;
    seconds = 60;
    //localScore = 0;
    emplCorrect = 0;
    tasksD = 0;
    wellDone = 0;
    update();
}

void Translate::setupDiffculty(int ind) {
    reset();
    if (ind == 0) {
        if (difficulty == 0) {
            return;
        } else if (difficulty == 1) {
            intInd = currentQuestionIndex;
        } else {
            advInd = currentQuestionIndex;
        }
        currentQuestionIndex = begInd;
        currentQuestions = questionsInfoBeginer;
    } else if (ind == 1) {
        if (difficulty == 0) {
            begInd = currentQuestionIndex;
        } else if (difficulty == 1) {
            return;
        } else {
            advInd = currentQuestionIndex;
        }
        currentQuestionIndex = intInd;
        currentQuestions = questionsInfoInter;
    } else {
        if (difficulty == 0) {
            begInd = currentQuestionIndex;
        } else if (difficulty == 1) {
            intInd = currentQuestionIndex;
        } else {
            return;
        }
        currentQuestionIndex = advInd;
        currentQuestions = questionsInfoAdvanced;
    }
    difficulty = ind;
    question->setText(currentQuestions[currentQuestionIndex].que);
    update();
}

void Translate::checkans() {
    if (textEdit->toPlainText() == currentQuestions[currentQuestionIndex].correctAns) {
        correctAns->play();
        if (difficulty == 0) {
            localScore += 10;
        } else if (difficulty == 1) {
            localScore += 15;
        } else {
            localScore += 20;
        }
        emplCorrect++;
        wellDone++;
        showDialog("Well done!");
    } else {
        notCorrectAns->play();
        attempts--;
        //sleep(1000);
        if (attempts == 0) {
            lost->play();
            showDialog(QString("The attempts are over! You have %1 correct answers. You have earned %2 points").arg(wellDone).arg(localScore));
            emit end();
            reset();
            update();
            return;
        }
        showDialog("NOT CORRECT!\nThe correct version is:\n" + currentQuestions[currentQuestionIndex].correctAns);
    }
    currentQuestionIndex++;
    tasksD++;
    if (tasksD == numTasks) {
        levelComplete->play();
        showDialog(QString("The questions are over! You have %1 correct answers. You have earned %2 points").arg(wellDone).arg(localScore));
        emit end();
        reset();
        update();
    }
    update();
}

void Translate::update() {
    question->setText(currentQuestions[currentQuestionIndex].que);
    timeL->setText(QString("You have %1 seconds").arg(seconds));
    scoreL->setText(QString("Your score is %1").arg(localScore));
    attemptsL->setText(QString("You have %1 attemts").arg(attempts));
    textEdit->clear();
    progress->setValue(emplCorrect);
}

void Translate::showDialog(QString str)
{
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Result");
    dialog->setFixedSize(400, 250);

    dialog->setStyleSheet(
        "QDialog {"
        "   background-color: #f8f9fa;"
        "   border-radius: 10px;"
        "   border: 1px solid #e0e0e0;"
        "}"
    );

    QVBoxLayout* layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(20, 20, 20, 15);
    layout->setSpacing(15);

    QLabel* messageLabel = new QLabel(str, dialog);
    messageLabel->setWordWrap(true);
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setStyleSheet(
        "QLabel {"
        "   font-size: 15px;"
        "   color: #333333;"
        "   padding: 5px;"
        "}"
    );

    QPushButton* okButton = new QPushButton("OK", dialog);
    okButton->setStyleSheet(
        "QPushButton {"
        "   background-color: #58cc02;"
        "   color: white;"
        "   border: none;"
        "   border-radius: 5px;"
        "   padding: 8px 16px;"
        "   font-size: 14px;"
        "   min-width: 80px;"
        "}"
        "QPushButton:hover {"
        "   background-color: #4db802;"
        "}"
    );

    layout->addWidget(messageLabel, 1);
    layout->addWidget(okButton, 0, Qt::AlignHCenter);

    connect(okButton, &QPushButton::clicked, dialog, &QDialog::accept);

    dialog->exec();
    dialog->deleteLater();
}