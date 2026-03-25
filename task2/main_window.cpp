#include "main_window.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>
#include <QMessageBox>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent), remainingSeconds_(0), totalSeconds_(0), isRunning_(false) {
    setWindowTitle("🎓 CS Study Focus Station");
    resize(800, 600);

    timer_ = new QTimer(this);
    connect(timer_, &QTimer::timeout, this, &MainWindow::onTimerTick);

    setupUi();
    updateTheme(0);  // Start with default theme
}

void MainWindow::setupUi() {
    QVBoxLayout* rootLayout = new QVBoxLayout(this);

    mainTabs_ = new QTabWidget(this);

    // --- Tab 1: Focus Timer ---
    QWidget* focusTab = new QWidget();
    QVBoxLayout* focusLayout = new QVBoxLayout(focusTab);

    // Session Title
    QHBoxLayout* headerLayout = new QHBoxLayout();
    headerLayout->addWidget(new QLabel("Session Name:"));
    sessionLabel_ = new QLineEdit("Deep Work #1");
    headerLayout->addWidget(sessionLabel_);
    focusLayout->addLayout(headerLayout);

    // Core Timer area
    QHBoxLayout* timerArea = new QHBoxLayout();

    // Dial for setting time
    QVBoxLayout* dialContainer = new QVBoxLayout();
    durationDial_ = new QDial();
    durationDial_->setRange(1, 99);
    durationDial_->setValue(25);
    durationDial_->setNotchesVisible(true);
    dialContainer->addWidget(new QLabel("Minutes", this), 0, Qt::AlignCenter);
    dialContainer->addWidget(durationDial_);
    timerArea->addLayout(dialContainer);

    // LCD Display
    timeDisplay_ = new QLCDNumber();
    timeDisplay_->setDigitCount(5);
    timeDisplay_->display("25:00");
    timeDisplay_->setMinimumSize(200, 100);
    timerArea->addWidget(timeDisplay_, 1);

    focusLayout->addLayout(timerArea);

    // Progress Bar
    progressBar_ = new QProgressBar();
    progressBar_->setRange(0, 100);
    progressBar_->setValue(0);
    focusLayout->addWidget(progressBar_);

    // Notes Area
    focusLayout->addWidget(new QLabel("Session Notes:"));
    notesArea_ = new QTextEdit();
    notesArea_->setPlaceholderText("What are you focusing on right now?");
    focusLayout->addWidget(notesArea_);

    // Controls
    QHBoxLayout* ctrlLayout = new QHBoxLayout();
    startStopBtn_ = new QPushButton("🚀 Start Session");
    resetBtn_ = new QPushButton("🔄 Reset");
    resetBtn_->setEnabled(false);
    ctrlLayout->addWidget(startStopBtn_);
    ctrlLayout->addWidget(resetBtn_);
    focusLayout->addLayout(ctrlLayout);

    mainTabs_->addTab(focusTab, "🔥 Focus");

    // --- Tab 2: Dashboard/History ---
    QWidget* dashTab = new QWidget();
    QGridLayout* dashLayout = new QGridLayout(dashTab);

    calendar_ = new QCalendarWidget();
    calendar_->setGridVisible(true);
    dashLayout->addWidget(new QLabel("<b>Study Activity</b>"), 0, 0);
    dashLayout->addWidget(calendar_, 1, 0, 1, 2);

    modeSelector_ = new QComboBox();
    modeSelector_->addItems({"Standard Dark", "Cyber Neon", "Paper Light"});
    dashLayout->addWidget(new QLabel("Vibe Mode:"), 2, 0);
    dashLayout->addWidget(modeSelector_, 2, 1);

    volumeSlider_ = new QSlider(Qt::Horizontal);
    volumeSlider_->setRange(0, 100);
    volumeSlider_->setValue(50);
    dashLayout->addWidget(new QLabel("Lo-fi Volume:"), 3, 0);
    dashLayout->addWidget(volumeSlider_, 3, 1);

    notificationsCheck_ = new QCheckBox("Mute distractors");
    notificationsCheck_->setChecked(true);
    dashLayout->addWidget(notificationsCheck_, 4, 0, 1, 2);

    mainTabs_->addTab(dashTab, "📊 Stats & Settings");

    rootLayout->addWidget(mainTabs_);

    // Connections
    connect(durationDial_, &QDial::valueChanged, this, &MainWindow::onDialChanged);
    connect(startStopBtn_, &QPushButton::clicked, this, &MainWindow::onStartStopClicked);
    connect(resetBtn_, &QPushButton::clicked, this, &MainWindow::onResetClicked);
    connect(calendar_, &QCalendarWidget::clicked, this, &MainWindow::onCalendarClicked);
    connect(
        modeSelector_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::updateTheme);
}

void MainWindow::onDialChanged(int value) {
    if (!isRunning_) {
        remainingSeconds_ = value * 60;
        totalSeconds_ = remainingSeconds_;
        int mins = remainingSeconds_ / 60;
        int secs = remainingSeconds_ % 60;
        timeDisplay_->display(
            QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0')));
        progressBar_->setValue(0);
    }
}

void MainWindow::onStartStopClicked() {
    if (!isRunning_) {
        if (remainingSeconds_ <= 0) {
            onDialChanged(durationDial_->value());
        }
        timer_->start(1000);
        isRunning_ = true;
        startStopBtn_->setText("⏸ Pause Session");
        startStopBtn_->setStyleSheet("background-color: #e74c3c; color: white;");
        durationDial_->setEnabled(false);
        resetBtn_->setEnabled(true);
    } else {
        timer_->stop();
        isRunning_ = false;
        startStopBtn_->setText("▶ Resume Session");
        startStopBtn_->setStyleSheet("");
    }
}

void MainWindow::onResetClicked() {
    timer_->stop();
    isRunning_ = false;
    onDialChanged(durationDial_->value());
    startStopBtn_->setText("🚀 Start Session");
    startStopBtn_->setStyleSheet("");
    durationDial_->setEnabled(true);
    resetBtn_->setEnabled(false);
}

void MainWindow::onTimerTick() {
    if (remainingSeconds_ > 0) {
        remainingSeconds_--;
        int mins = remainingSeconds_ / 60;
        int secs = remainingSeconds_ % 60;
        timeDisplay_->display(
            QString("%1:%2").arg(mins, 2, 10, QChar('0')).arg(secs, 2, 10, QChar('0')));

        int progress = 100 - (remainingSeconds_ * 100 / totalSeconds_);
        progressBar_->setValue(progress);
    } else {
        timer_->stop();
        isRunning_ = false;
        QMessageBox::information(this, "Focus Done!", "Great job! Time for a short break.");
        onResetClicked();
    }
}

void MainWindow::onCalendarClicked(const QDate& date) {
    QString msg = QString("Activity for %1:\n- 3 hours of Algorithms\n- 45 mins of Qt Lab")
                      .arg(QLocale::system().toString(date, QLocale::LongFormat));
    QMessageBox::information(this, "Day Log", msg);
}

void MainWindow::updateTheme(int index) {
    if (index == 0) {
        applyTheme("dark");
    } else if (index == 1) {
        applyTheme("cyber");
    } else {
        applyTheme("light");
    }
}

void MainWindow::applyTheme(const QString& theme) {
    if (theme == "dark") {
        setStyleSheet(R"(
            QWidget { background-color: #2c3e50; color: #ecf0f1; font-family: 'Segoe UI', sans-serif; }
            QLineEdit, QTextEdit { background-color: #34495e; border: 1px solid #7f8c8d; border-radius: 4px; padding: 5px; }
            QPushButton { background-color: #3498db; border-radius: 5px; padding: 8px; font-weight: bold; }
            QPushButton:hover { background-color: #2980b9; }
            QProgressBar { border: 1px solid #7f8c8d; border-radius: 5px; text-align: center; }
            QProgressBar::chunk { background-color: #2ecc71; }
            QTabWidget::pane { border: 1px solid #7f8c8d; }
            QTabBar::tab { background: #34495e; padding: 10px; border-top-left-radius: 4px; border-top-right-radius: 4px; }
            QTabBar::tab:selected { background: #3498db; }
        )");
    } else if (theme == "cyber") {
        setStyleSheet(R"(
            QWidget { background-color: #0d0221; color: #00ff41; font-family: 'Courier New'; }
            QLineEdit, QTextEdit { background-color: #1a0b2e; border: 1px solid #00f0ff; color: #00f0ff; }
            QPushButton { background-color: #ff0055; color: white; border: 2px solid #00f0ff; }
            QLCDNumber { color: #f0f; border: 1px solid #00f0ff; }
            QProgressBar::chunk { background-color: #00f0ff; }
        )");
    } else {
        setStyleSheet("");  // Default / Light
    }
}
