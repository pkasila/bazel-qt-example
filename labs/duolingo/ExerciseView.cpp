//
// Created by Admin on 16.05.2025.
//

#include "ExerciseView.h"

#include <QAudioOutput>
#include <QLabel>
#include <QMediaPlayer>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QPushButton>
#include <QTimer>

ExerciseView::ExerciseView(std::function<Exercise*(QWidget*)> gen, int N, int M, int time, QString help, QWidget *parent) : QWidget(parent), _timeRemaining(time), _exRemaining(N), _wrongs(M), _help(help) {
    auto layout = new QVBoxLayout(this);

    auto timeText = new QLabel(this);
    timeText->setAlignment(Qt::AlignCenter);
    timeText->setText(QString("%1: %2").arg(_timeRemaining/60, 2, 10, QChar('0')).arg(_timeRemaining%60, 2, 10, QChar('0')));
    layout->addWidget(timeText);

    auto timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [timeText, timer, this]() {
        _timeRemaining--;
        timeText->setText(QString("%1: %2").arg(_timeRemaining/60, 2, 10, QChar('0')).arg(_timeRemaining%60, 2, 10, QChar('0')));
        if (_timeRemaining <= 0) {
            timer->stop();
            timeText->setText("TIME!!!");
            _addScore = false;
            this->setDisabled(true);
        }
    });
    timer->start(1000);
    QWidget* w1 = new QWidget(this);
    auto hb = new QHBoxLayout(w1);
    auto exCont = new QWidget(w1);
    auto el = new QVBoxLayout(exCont);
    _exercise = gen(exCont);
    el->addWidget(_exercise);
    exCont->setLayout(el);
    hb->addWidget(exCont);

    QLabel* result = new QLabel(w1);
    hb->addWidget(result);
    w1->setLayout(hb);
    layout->addWidget(w1);

    auto pb = new QProgressBar(this);
    pb->setMinimum(0);
    pb->setMaximum(_exRemaining);
    pb->setValue(0);

    auto bt = new QPushButton("Submit", this);
    connect(bt, &QPushButton::clicked, this, [gen, this, exCont, timer, timeText, pb, el, result]() {
        _exercise->Submit();
        result->setText(_exercise->Result());
        QMediaPlayer* player = new QMediaPlayer(this);
        QAudioOutput* audioOutput = new QAudioOutput(this);
        player->setAudioOutput(audioOutput);
        audioOutput->setVolume(0.5);
        if (_exercise->GetScore() == 0) {
            player->setSource(QUrl("qrc:/fail.mp3"));
            _addScore = 0;
            _wrongs--;
        }
        else {
            player->setSource(QUrl("qrc:/pass.mp3"));
        }
        player->play();
        if (_wrongs <= 0) {
            timer->stop();
            timeText->setText("Failed");
            this->setDisabled(true);
            return;
        }
        _exRemaining--;
        pb->setValue(pb->value()+1);
        if (_exRemaining <= 0) {
            timer->stop();
            timeText->setText(QString("Finished, score %1").arg(_addScore ? "added" : "not added"));
            this->setDisabled(true);
            return;
        }
        el->removeWidget(_exercise);
        delete _exercise;
        _exercise = gen(exCont);
        el->addWidget(_exercise);
        exCont->update();
    });
    layout->addWidget(bt);


    layout->addWidget(pb);

    setLayout(layout);
}

ExerciseView::~ExerciseView() {
    delete _exercise;
}

int ExerciseView::getScore()  {
    return _addScore & !_exRemaining;
}

QString ExerciseView::getHelp() {
    return this->_help;
}


