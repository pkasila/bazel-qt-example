//
// Created by Admin on 16.05.2025.
//

#include "MainWindow.h"

#include <QMenuBar>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QStackedWidget>
#include <QSqlDatabase>
#include <QSqlQuery>
#include  <QSqlError>
#include  <QRandomGenerator>
#include <QDialog>
#include <QSpinBox>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QMediaDevices>
#include <QAudioDevice>
#include <qdir.h>

#include "Exercise.h"
#include "ExerciseView.h"
#include "GrammarExercise.h"
#include "TranslationExercise.h"

MainWindow::MainWindow(QString dbPath, QWidget *parent) : QMainWindow(parent){

    setWindowTitle("Duolingo");
    resize(800, 600);

    QMediaPlayer* player = new QMediaPlayer(this);
    QAudioDevice audioDevice = QMediaDevices::defaultAudioOutput();
    QAudioOutput* audioOutput = new QAudioOutput(audioDevice, this);

    player->setAudioOutput(audioOutput);
    player->setSource(QUrl("qrc:/back.mp3"));
    audioOutput->setVolume(0.5);

    connect(player, &QMediaPlayer::mediaStatusChanged, this, [player] (QMediaPlayer::MediaStatus status) {
        if (status == QMediaPlayer::EndOfMedia) {
            player->play();
        }
    });

    player->play();


    _widget = new QStackedWidget(this);


    auto mb = new QMenuBar(this);
    auto mn = new QMenu("Settings", mb);
    mn->addAction("Change difficulty",  [this] () {
        QDialog* d = new QDialog(this);
        d->setWindowTitle("Settings");
        d->setModal(true);
        QVBoxLayout* l = new QVBoxLayout(d);
        auto sb = new QSpinBox(d);
        sb->setRange(0, 2);
        sb->setValue(_dif);
        connect(sb, &QSpinBox::valueChanged, [this] (int x) {_dif = x;});
        auto dn = new QPushButton(d);
        dn->setText("Done");
        connect(dn, &QPushButton::clicked, d, &QDialog::accept);
        l->addWidget(sb);
        l->addWidget(dn);
        d->setLayout(l);
        d->resize(300, 200);
        d->exec();
    });
    mb->addMenu(mn);
    setMenuBar(mb);


    QWidget *p1 = new QWidget;

    auto vl = new QVBoxLayout(p1);

    _score = new QLabel("Score: 0", p1);
    _score->setAlignment(Qt::AlignCenter);
    vl->addWidget(_score);

    QPushButton *btn1 = new QPushButton("Translation", p1);
    vl->addWidget(btn1);
    p1->setLayout(vl);

    QPushButton *btn2 = new QPushButton("Grammar", p1);
    vl->addWidget(btn2);
    p1->setLayout(vl);

    _widget->addWidget(p1);

    setCentralWidget(_widget);

    _db = QSqlDatabase::addDatabase("QSQLITE");
    _db.setDatabaseName(dbPath);
    if (!_db.open()) {
        qDebug() << "Db fail" << _db.lastError().text();
    }

    connect (btn1, &QPushButton::clicked, this, [this](){
        QSqlQuery*  query = new QSqlQuery(_db);
        query->prepare("SELECT russian, english FROM translationTasks WHERE difficulty <= :maxDifficulty ORDER BY RANDOM() LIMIT :N");
        query->bindValue(":N", 5);
        query->bindValue(":maxDifficulty", _dif);
        if (!query->exec()) {
            qDebug() << "Bazel не копирует файлы нормально, надо указать абсолютный путь к бд в аргс или собирать через симейк";
            return;
        }
        query->next();
        StartExercise([query](QWidget *parent) {
            QString ru = query->value(0).toString(), en = query->value(1).toString();
            if (!query->next()) {
                delete query;
            }
            return (Exercise*) new TranslationExercise(ru, en, nullptr);
        }, "The translations are obvious. You can ask ChatGPT if you are not sure.");
    });
    connect (btn2, &QPushButton::clicked, this, [this](){
        QSqlQuery* q1 = new QSqlQuery(_db);
        q1->prepare("SELECT question, answer FROM quiz WHERE difficulty <= :maxDifficulty ORDER BY RANDOM() LIMIT :N");
        q1->bindValue(":N", 5);
        q1->bindValue(":maxDifficulty", _dif);
        if (!q1->exec()) {
            qDebug() << "Bazel не копирует файлы нормально, надо указать абсолютный путь к бд в аргс или собирать через симейк";
            return;
        }
        q1->next();
        StartExercise([q1, this](QWidget *parent) {
            QSqlQuery q2(_db);
            QString correct = q1->value(1).toString();
            q2.prepare("SELECT answer FROM quiz WHERE answer != :correct ORDER BY RANDOM() LIMIT 3");
            q2.bindValue(":correct", correct);
            q2.exec();
            QString q = q1->value(0).toString();
            if (!q1->next()) {
                delete q1;
            }
            return (Exercise*) new GrammarExercise(parent, q, correct, (q2.next(), q2.value(0).toString()), (q2.next(), q2.value(0).toString()), (q2.next(), q2.value(0).toString()));
        }, "Grammar is very important. You can try observing rhymes in words...");
    });

}

void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        if (!_ev) return;
        QDialog* d = new QDialog(this);
        d->setWindowTitle("Hint");
        d->setModal(true);
        QVBoxLayout* lt = new QVBoxLayout(d);
        QLabel* l = new QLabel(d);
        l->setText(_ev->getHelp());
        l->setAlignment(Qt::AlignCenter);
        lt->addWidget(l);
        QPushButton* btn1 = new QPushButton(d);
        btn1->setText("Ok");
        lt->addWidget(btn1);
        connect(btn1, &QPushButton::clicked, d, &QDialog::accept);
        d->exec();
    } else {
        QMainWindow::keyPressEvent(event); // обработка других клавиш
    }
}

void MainWindow::StartExercise (std::function<Exercise*(QWidget*)> gen, QString help) {
    auto page = new QWidget();
    auto layout = new QVBoxLayout(page);
    _ev = new ExerciseView(gen, 5, 2, 300, help, page);
    layout->addWidget(_ev);
    auto bt = new QPushButton("Main menu", page);
    layout->addWidget(bt);
    page->setLayout(layout);
    _widget->addWidget(page);
    _widget->setCurrentIndex(1);
    connect(bt, &QPushButton::clicked, this, &MainWindow::MainPage);
}

void MainWindow::MainPage() {
    _iscore += _ev->getScore();
    _ev = nullptr;
    _score->setText(QString("Score: %1").arg(_iscore));
    auto page = _widget->widget(1);
    _widget->setCurrentIndex(0);
    _widget->removeWidget(page);
    delete page;
}

