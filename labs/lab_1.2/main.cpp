#include "header.h"
#include <QApplication>
#include <QSoundEffect>
#include <QTemporaryFile>
#include <QWidget>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLineEdit>
#include <QCheckBox>
#include <QCalendarWidget>
#include <QDial>
#include <QLabel>
#include <QListWidget>
#include <QPalette>
#include <QTimer>
#include <cmath>
#include <algorithm>

// Функцыя для генерацыі WAV-структуры ў памяці
QByteArray generateBeepData() {
    QByteArray data;
    const int sampleRate = 44100;
    const int durationMs = 1500; // Зробім гук даўжэйшым (1.5 секунды)
    const int numSamples = sampleRate * durationMs / 1000;

    // WAV Header (застаецца стандартным)
    data.append("RIFF", 4);
    int fileSize = 36 + numSamples * 2;
    data.append(reinterpret_cast<const char*>(&fileSize), 4);
    data.append("WAVEfmt ", 8);
    int fmtSize = 16;
    data.append(reinterpret_cast<const char*>(&fmtSize), 4);
    short format = 1; 
    data.append(reinterpret_cast<const char*>(&format), 2);
    short channels = 1; 
    data.append(reinterpret_cast<const char*>(&channels), 2);
    data.append(reinterpret_cast<const char*>(&sampleRate), 4);
    int byteRate = sampleRate * 2;
    data.append(reinterpret_cast<const char*>(&byteRate), 4);
    short blockAlign = 2;
    data.append(reinterpret_cast<const char*>(&blockAlign), 2);
    short bitsPerSample = 16;
    data.append(reinterpret_cast<const char*>(&bitsPerSample), 2);
    data.append("data", 4);
    int dataSize = numSamples * 2;
    data.append(reinterpret_cast<const char*>(&dataSize), 4);

    double phase = 0;
    for (int i = 0; i < numSamples; ++i) {
        double time = static_cast<double>(i) / sampleRate;

        double frequency = 600.0 + 300.0 * std::sin(2.0 * M_PI * 5.0 * time);
        
        phase += 2.0 * M_PI * frequency / sampleRate;

        short sample = (std::sin(phase) > 0) ? 10000 : -10000;

        if (std::fmod(time, 0.1) > 0.08) sample = 0;

        data.append(reinterpret_cast<const char*>(&sample), 2);
    }
    return data;
}

CreativeApp::CreativeApp(QWidget *parent) : QMainWindow(parent) {
    setupUI();
}

void CreativeApp::adjustVolume(int value) {
    int diff = value - lastDialValue;
    if (diff < -180) diff += 360;
    else if (diff > 180) diff -= 360;
    accumulatedAngle += diff;
    if (accumulatedAngle < 0) accumulatedAngle = 0;
    lastDialValue = value;

    double turns = accumulatedAngle / 360.0;
    double multiplier = 1.0 - std::pow(0.5, turns); 
    
    if (alarmSound) {
        alarmSound->setVolume(static_cast<float>(multiplier));
    }

    int percent = static_cast<int>(multiplier * 100.0);
    volumeLabel->setText(QString("Гучнасць напаміну: %1%").arg(percent));
}

void CreativeApp::playReminder() {
    if (alarmSound && alarmSound->volume() > 0.001) {
        alarmSound->play();
    }
}

void CreativeApp::setupUI() {
    auto central = new QWidget();
    auto mainLayout = new QGridLayout(central);

    addBtn = new QPushButton("Дадаць задачу");
    delBtn = new QPushButton("Выдаліць выбраную");
    noteEdit = new QLineEdit();
    noteEdit->setPlaceholderText("Увядзіце тэкст задачы...");
    
    urgentBox = new QCheckBox("Тэрміновы дэдлайн (сігнал з гучнасцю)");
    calendar = new QCalendarWidget();
    dial = new QDial();
    dial->setRange(0, 359); 
    dial->setWrapping(true); 
    dial->setEnabled(false); 

    taskList = new QListWidget();
    volumeLabel = new QLabel("Гучнасць напаміну: 0%");
    infoLabel = new QLabel("Спакойны працоўны рэжым.");
    dateInfo = new QLabel("Выберыце дату дэдлайну");

    reminderTimer = new QTimer(this);
    connect(reminderTimer, &QTimer::timeout, this, &CreativeApp::playReminder);

    if (tempSoundFile.open()) {
        tempSoundFile.write(generateBeepData());
        tempSoundFile.flush();
        tempSoundFile.close();
    }

    alarmSound = new QSoundEffect(this);
    alarmSound->setSource(QUrl::fromLocalFile(tempSoundFile.fileName()));
    alarmSound->setVolume(0.0f);

    mainLayout->addWidget(new QLabel("## ПАНЭЛЬ КІРАВАННЯ ДЭДЛАЙНАМІ ##"), 0, 0, 1, 2, Qt::AlignCenter);
    mainLayout->addWidget(calendar, 1, 0);
    
    auto rightLayout = new QVBoxLayout();
    rightLayout->addWidget(dateInfo);
    rightLayout->addWidget(noteEdit);
    auto btnLayout = new QHBoxLayout();
    btnLayout->addWidget(addBtn);
    btnLayout->addWidget(delBtn);
    rightLayout->addLayout(btnLayout);
    rightLayout->addWidget(taskList);
    mainLayout->addLayout(rightLayout, 1, 1);

    auto bottomLayout = new QHBoxLayout();
    bottomLayout->addWidget(dial);
    auto statusLayout = new QVBoxLayout();
    statusLayout->addWidget(volumeLabel);
    statusLayout->addWidget(urgentBox);
    statusLayout->addWidget(infoLabel);
    bottomLayout->addLayout(statusLayout);
    mainLayout->addLayout(bottomLayout, 2, 0, 1, 2);

    connect(dial, &QDial::valueChanged, this, &CreativeApp::adjustVolume);
    connect(urgentBox, &QCheckBox::toggled, this, &CreativeApp::toggleUrgency);
    connect(addBtn, &QPushButton::clicked, this, &CreativeApp::addTask);
    connect(delBtn, &QPushButton::clicked, this, &CreativeApp::deleteTask);
    connect(noteEdit, &QLineEdit::returnPressed, this, &CreativeApp::addTask);
    connect(calendar, &QCalendarWidget::selectionChanged, this, &CreativeApp::updateDateLabel);
    connect(taskList, &QListWidget::itemDoubleClicked, this, &CreativeApp::toggleTaskStatus);

    setCentralWidget(central);
    setWindowTitle("Творчая прастора: Дэдлайны");
    resize(900, 600);
}

void CreativeApp::toggleUrgency(bool checked) {
    dial->setEnabled(checked);
    if (checked) {
        QPalette pal = palette();
        pal.setColor(QPalette::Window, Qt::darkRed);
        setPalette(pal);
        infoLabel->setText("УВАГА: Дэдлайн! Гук уключаны.");
        reminderTimer->start(5000); 
    } else {
        setPalette(QPalette());
        infoLabel->setText("Спакойны працоўны рэжым.");
        reminderTimer->stop();
    }
}

void CreativeApp::addTask() {
    QString text = noteEdit->text();
    if (!text.isEmpty()) {
        Task newTask;
        newTask.date = calendar->selectedDate();
        newTask.text = text;
        newTask.completed = false;
        tasks.push_back(newTask);
        std::sort(tasks.begin(), tasks.end(), [](const Task& a, const Task& b) { return a.date < b.date; });
        renderTasks();
        noteEdit->clear();
    }
}

void CreativeApp::updateDateLabel() {
    dateInfo->setText("Дэдлайн задачы: " + calendar->selectedDate().toString("dd MMMM yyyy"));
}

void CreativeApp::toggleTaskStatus(QListWidgetItem* item) {
    int index = taskList->row(item);
    if (index >= 0 && index < (int)tasks.size()) {
        tasks[index].completed = !tasks[index].completed;
        renderTasks(); 
    }
}

void CreativeApp::deleteTask() {
    int index = taskList->currentRow();
    if (index >= 0 && index < (int)tasks.size()) {
        tasks.erase(tasks.begin() + index);
        renderTasks();
    }
}

void CreativeApp::renderTasks() {
    taskList->clear();
    for (const auto& task : tasks) {
        QString itemText = QString("[%1] %2").arg(task.date.toString("dd.MM.yyyy")).arg(task.text);
        auto item = new QListWidgetItem(itemText, taskList);
        if (task.completed) item->setBackground(Qt::green);
        else item->setBackground(Qt::lightGray);
    }
}

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);
    CreativeApp w;
    w.show();
    return a.exec();
}