#include "grammar.h"


Grammar::Grammar(QWidget *parent)
    : QWidget{parent}
{

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


    QVBoxLayout* layout = new QVBoxLayout(this);

    QHBoxLayout* upL = new QHBoxLayout();

    QVBoxLayout*  midL = new QVBoxLayout();

    QVBoxLayout* downL = new QVBoxLayout();

    attemptsL = new QLabel(QString("You have %1 attemts").arg(attempts));
    timeL = new QLabel(QString("You have %1 seconds").arg(seconds));
    scoreL = new QLabel(QString("Your score is %1").arg(localScore));
    helpButton = new QPushButton("help");
    question = new QLabel(currentQuestions[currentQuestionIndex].que);

    //textEdit = new QTextEdit();
    buttonGroup = new QButtonGroup();

    QVBoxLayout* buttonsL = new QVBoxLayout();
    radio1 = new QRadioButton(currentQuestions[currentQuestionIndex].ans[0]);
    radio2 = new QRadioButton(currentQuestions[currentQuestionIndex].ans[1]);
    radio3 = new QRadioButton(currentQuestions[currentQuestionIndex].ans[2]);
    radio4 = new QRadioButton(currentQuestions[currentQuestionIndex].ans[3]);
    buttonsL->addWidget(radio1);
    buttonsL->addWidget(radio2);
    buttonsL->addWidget(radio3);
    buttonsL->addWidget(radio4);

    buttonGroup->addButton(radio1);
    buttonGroup->addButton(radio2);
    buttonGroup->addButton(radio3);
    buttonGroup->addButton(radio4);

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
    midL->addLayout(buttonsL);

    downL->addWidget(progress);
    downL->addWidget(submit);

    layout->addLayout(upL);
    layout->addLayout(midL);
    layout->addLayout(downL);

    connect(this, &Grammar::start, timer, [&]() {
        timer->start(1000);
        // qDebug() << "Timer starts";
    });

    connect(submit, &QPushButton::clicked, this, &Grammar::checkans);

    connect(this, &Grammar::end, timer, [&]() {
        //showDialog(QString("The time is over! You have %1 correct answers. You have earned %2 points").arg(wellDone).arg(localScore));
        timer->stop();
        reset();
    });

    connect(timer, &QTimer::timeout, this, [&]() {
        seconds--;
        if (seconds == 0) {
            lost->play();
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


void Grammar::reset() {
    progress->setValue(0);
    attempts = 6;
    seconds = 60;
    //localScore = 0;
    emplCorrect = 0;
    tasksD = 0;
    wellDone = 0;
    update();
}

void Grammar::setupDiffculty(int ind) {
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

void Grammar::checkans() {
    if (!buttonGroup->checkedButton()) {
        showDialog("You should to choose a variant");
        return;
    }
    if (buttonGroup->checkedButton()->text() == currentQuestions[currentQuestionIndex].correctAns) {
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
        if (attempts == 0) {
            lost->play();
            showDialog(QString("The time is over! You have %1 correct answers. You have earned %2 points").arg(wellDone).arg(localScore));
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

void Grammar::update() {
    question->setText(currentQuestions[currentQuestionIndex].que);
    timeL->setText(QString("You have %1 seconds").arg(seconds));
    scoreL->setText(QString("Your score is %1").arg(localScore));
    attemptsL->setText(QString("You have %1 attemts").arg(attempts));
    radio1->setText(currentQuestions[currentQuestionIndex].ans[0]);
    radio2->setText(currentQuestions[currentQuestionIndex].ans[1]);
    radio3->setText(currentQuestions[currentQuestionIndex].ans[2]);
    radio4->setText(currentQuestions[currentQuestionIndex].ans[3]);
    if (buttonGroup->checkedButton()) {
        buttonGroup->setExclusive(false);
        buttonGroup->checkedButton()->setChecked(false);
        buttonGroup->setExclusive(true);
    }
    progress->setValue(emplCorrect);
}

void Grammar::showDialog(QString str)
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