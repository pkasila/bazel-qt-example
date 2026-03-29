#include "mainwindow.h"
#include <QPainter>
#include <QFileInfo>
#include <QTimer>
#include <QDateTime>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUI();
    resize(900,600);
    QString imagePath = QCoreApplication::applicationDirPath() + ":/images/background.png";
    if (QFileInfo::exists(imagePath)) {
        backgroundPixmap.load(imagePath);
    }

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &MainWindow::updateTimer);
    timer->start(1000);
}

MainWindow::~MainWindow() {}

void MainWindow::paintEvent(QPaintEvent *event) {
    QPainter painter(this);

    if (!backgroundPixmap.isNull()) {
        painter.drawPixmap(0, 0, width(), height(), backgroundPixmap);
    }

    QMainWindow::paintEvent(event);
}

void MainWindow::setupUI() {

    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);

    QHBoxLayout *topLayout = new QHBoxLayout();
    timerLabel = new QLabel("Прошло времени:", this);
    timerLabel->setStyleSheet("font-weight: bold; color: white;");
    topLayout->addWidget(timerLabel);

    timerDisplay = new QLCDNumber(this);
    timerDisplay->setDigitCount(8);
    timerDisplay->setSegmentStyle(QLCDNumber::Flat);
    timerDisplay->display("00:00:00");
    timerDisplay->setFrameStyle(QFrame::NoFrame);
    topLayout->addWidget(timerDisplay);

    QLabel *dateLabel = new QLabel("Сегодняшняя дата:", this);
    dateLabel->setStyleSheet("font-weight: bold; color: white;");
    topLayout->addWidget(dateLabel);

    miniCalendar = new QCalendarWidget(this);
    miniCalendar->setMinimumDate(QDate::currentDate());
    miniCalendar->setMaximumDate(QDate::currentDate());
    miniCalendar->setGridVisible(false);
    miniCalendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);
    miniCalendar->setHorizontalHeaderFormat(QCalendarWidget::ShortDayNames);
    miniCalendar->setNavigationBarVisible(false);
    miniCalendar->setFixedSize(200, 150);
    topLayout->addWidget(miniCalendar);

    mainLayout->addLayout(topLayout);

    mathAnalysisComboBox = new QComboBox(this);
    mathAnalysisComboBox->addItems({"","Васьковский М.М.", "Грекова А.В.", "Ковнацкая О.А.","Радыно Н.Я","Чернов С.Ю.","Леваков А.А."});
    QLabel *mathAnalysisLabel = new QLabel("Математический анализ:");
    mathAnalysisLabel->setStyleSheet("font-weight: bold; color: white;");
    mainLayout->addWidget(mathAnalysisLabel);
    mainLayout->addWidget(mathAnalysisComboBox);

    algebraComboBox = new QComboBox(this);
    algebraComboBox->addItems({"","Размыслович Г.П.","Филипцов А.В.", "Задорожнюк А.О.", "Грекова А.В."});
    QLabel *algebraLabel = new QLabel("Высшая алгебра:");
    algebraLabel->setStyleSheet("font-weight: bold; color: white;");
    mainLayout->addWidget(algebraLabel);
    mainLayout->addWidget(algebraComboBox);

    programmingComboBox = new QComboBox(this);
    programmingComboBox->addItems({"","Дубков В.П.", "Казанцева О.Г.", "Рябый В.В.","Яндексоид","Гусейнова А.С."});
    QLabel *programmingLabel = new QLabel("Программирование:");
    programmingLabel->setStyleSheet("font-weight: bold; color: white;");
    mainLayout->addWidget(programmingLabel);
    mainLayout->addWidget(programmingComboBox);

    historyComboBox = new QComboBox(this);
    historyComboBox->addItems({"","Лебедева И.В.", "Пыж Д.С."});
    QLabel *histLabel = new QLabel("История:");
    histLabel->setStyleSheet("font-weight: bold; color: white;");
    mainLayout->addWidget(histLabel);
    mainLayout->addWidget(historyComboBox);

    englishComboBox = new QComboBox(this);
    englishComboBox->addItems({"","Ситникова Т.В.", "Видишева С.Л"});
    QLabel *enLabel = new QLabel("Английский:");
    enLabel->setStyleSheet("font-weight: bold; color: white;");
    mainLayout->addWidget(enLabel);
    mainLayout->addWidget(englishComboBox);

    discreteMathComboBox = new QComboBox(this);
    discreteMathComboBox->addItems({"","Орлович Ю.Л.", "Ловеров Я.А.", "Яндексоид"});
    QLabel *DMLabel = new QLabel("Дискретная математика:");
    DMLabel->setStyleSheet("font-weight: bold; color: white;");
    mainLayout->addWidget(DMLabel);
    mainLayout->addWidget(discreteMathComboBox);

    olympiadMathCheckBox = new QCheckBox("Олимпиадник по математике", this);
    mainLayout->addWidget(olympiadMathCheckBox);

    olympiadInformaticsCheckBox = new QCheckBox("Олимпиадник по информатике", this);
    mainLayout->addWidget(olympiadInformaticsCheckBox);

    olympiadAstronomyCheckBox = new QCheckBox("Олимпиадник по астрономии", this);
    mainLayout->addWidget(olympiadAstronomyCheckBox);

    diligenceSpinBox = new QSpinBox(this);
    diligenceSpinBox->setRange(0, 3);
    diligenceSpinBox->setValue(0);
    mainLayout->addWidget(new QLabel("Трудолюбие (1-3):"));
    mainLayout->addWidget(diligenceSpinBox);

    memorySpinBox = new QSpinBox(this);
    memorySpinBox->setRange(0, 3);
    memorySpinBox->setValue(0);
    mainLayout->addWidget(new QLabel("Способность запоминать текст (1-3):"));
    mainLayout->addWidget(memorySpinBox);

    thinkingSpeedSpinBox = new QSpinBox(this);
    thinkingSpeedSpinBox->setSpecialValueText("");
    thinkingSpeedSpinBox->setRange(0, 3);
    thinkingSpeedSpinBox->setValue(0);
    mainLayout->addWidget(new QLabel("Скорость думания (1-3):"));
    mainLayout->addWidget(thinkingSpeedSpinBox);

    neatnessSpinBox = new QSpinBox(this);
    neatnessSpinBox->setRange(0, 3);
    neatnessSpinBox->setValue(0);
    mainLayout->addWidget(new QLabel("Аккуратность (1-3):"));
    mainLayout->addWidget(neatnessSpinBox);

    calculateButton = new QPushButton("Рассчитать", this);
    connect(calculateButton, &QPushButton::clicked, this, &MainWindow::calculateResult);
    mainLayout->addWidget(calculateButton);

    resultLabel = new QLabel("Результат", this);
    mainLayout->addWidget(resultLabel);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    mainLayout->addWidget(progressBar);

    setCentralWidget(centralWidget);

    connect(mathAnalysisComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateProgress);
    connect(algebraComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateProgress);
    connect(programmingComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateProgress);
    connect(historyComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateProgress);
    connect(englishComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateProgress);
    connect(discreteMathComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateProgress);
    connect(diligenceSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateProgress);
    connect(memorySpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateProgress);
    connect(thinkingSpeedSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateProgress);
    connect(neatnessSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::updateProgress);
}

void MainWindow::updateTimer() {
    elapsedTime++;
    int hours = elapsedTime / 3600;
    int minutes = (elapsedTime % 3600) / 60;
    int seconds = elapsedTime % 60;

    QString timeString = QString("%1:%2:%3")
                             .arg(hours, 2, 10, QLatin1Char('0'))
                             .arg(minutes, 2, 10, QLatin1Char('0'))
                             .arg(seconds, 2, 10, QLatin1Char('0'));

    timerDisplay->display(timeString);
}

bool MainWindow::checkFieldsFilled() {
    if (mathAnalysisComboBox->currentIndex() == 0) return false;
    if (algebraComboBox->currentIndex() == 0) return false;
    if (programmingComboBox->currentIndex() == 0) return false;
    if (historyComboBox->currentIndex() == 0) return false;
    if (englishComboBox->currentIndex() == 0) return false;
    if (discreteMathComboBox->currentIndex() == 0) return false;

    if (diligenceSpinBox->value() == 0) return false;
    if (memorySpinBox->value() == 0) return false;
    if (thinkingSpeedSpinBox->value() == 0) return false;
    if (neatnessSpinBox->value() == 0) return false;

    return true;
}

void MainWindow::calculateResult() {
    int Ateacher = 0;
    int MAteacher=0;
    int Progteacher=0;
    int histteacher = 0;
    int enteacher = 0;
    int Dmteacher = 0;
    int score = 0;
    int Math = 0;
    int Astr = 0;
    int Inf = 0;
    int dil = 0;
    int mem = 0;
    int speed = 0;
    int neat = 0;

    if (!checkFieldsFilled()) {
        QMessageBox::warning(this, "Ошибка", "Пожалуйста, заполните все поля!");
        return;
    }

    int maIndex = mathAnalysisComboBox->currentIndex();
    if (maIndex == 1) MAteacher += 10;
    else if (maIndex == 2) MAteacher += 5;
    else if (maIndex == 3) MAteacher += 4;
    else if (maIndex == 4) MAteacher+= 2;
    else if (maIndex == 5) MAteacher += 7;
    else if (maIndex == 6) MAteacher += 8;

    int algebraIndex = algebraComboBox->currentIndex();
    if (algebraIndex == 1) Ateacher += 8;
    else if (algebraIndex == 2) Ateacher += 5;
    else if (algebraIndex == 3) Ateacher += 3;
    else if (algebraIndex == 4) Ateacher += 2;

    int progIndex = programmingComboBox->currentIndex();
    if (progIndex == 1) Progteacher += 2;
    else if (progIndex == 2) Progteacher += 9;
    else if (progIndex == 3) Progteacher += 5;
    else if (progIndex == 4) Progteacher += 7;
    else if (progIndex == 5) Progteacher += 4;

    int historyIndex = historyComboBox->currentIndex();
    if (historyIndex == 1) histteacher += 8;
    else if (historyIndex == 2) histteacher += 7;

    int englishIndex = englishComboBox->currentIndex();
    if (englishIndex == 1) enteacher += 10;
    else if (englishIndex == 2) enteacher += 8;

    int dmIndex = discreteMathComboBox->currentIndex();
    if (dmIndex == 1) Dmteacher += 5;
    else if (dmIndex == 2) Dmteacher += 8;
    else if (dmIndex == 3) Dmteacher += 3;


    if (olympiadMathCheckBox->isChecked()) Math = 20;
    if (olympiadInformaticsCheckBox->isChecked()) Inf = 10;
    if (olympiadAstronomyCheckBox->isChecked()) Astr = 5;

    dil = diligenceSpinBox->value();
    mem = memorySpinBox->value();
    speed = thinkingSpeedSpinBox->value();
    neat = neatnessSpinBox->value();

    score = Ateacher*(dil*0.3+mem*0.6+speed*0.9+neat*0.8)+MAteacher*(dil*1.2+mem*0.2+speed*0.5+neat*0.8)+Progteacher*(dil*0.9+speed*0.2+neat*1)+
            histteacher*(dil*0.6+mem*1.5+neat*0.1)+enteacher*(dil*0.3+mem*0.6+neat*0.3)+Dmteacher*(dil*0.1+mem*0.3+speed*0.3+neat*0.4)+Astr+Inf*(Dmteacher*0.5+MAteacher*0.3+Progteacher)+
            Math*(Ateacher*0.5+MAteacher*1.5);

    if (MAteacher==10 && Math==20 && dil>=2) {
        resultLabel->setText("Будете жить как шоколадный математик!");
    } else if (MAteacher==7 && Math==20) {
        resultLabel->setText("Зачиллитесь, Чернов тоже чиллит");
    } else if (MAteacher==10 && Inf==10 && Math==0 && dil==1) {
        resultLabel->setText("На экзамене по МА вам придется не сладко");
    } else if (MAteacher==10 && Math==20 && dil==1) {
        resultLabel->setText("'Будете первым математиком, котрого отчислили с ФПМА (А.В. Филипцов)'");
    } else if (Math==20 && enteacher==8) {
        resultLabel->setText("Прошли в первую группу? Ну ладно, но учтите, что все, отчисленные из нее, были из группы Видишевой");
    }
    else if (score >= 350) {
        resultLabel->setText("Вы будете успешным на первом курсе!");
    } else if (score >= 180) {
        resultLabel->setText("Вы будете старательным середнячком.");
    } else if (score >= 100) {
        resultLabel->setText("Вы будете рядовым студентом.");
    } else {
        resultLabel->setText("Вы будете отчислены.");
    }
}


void MainWindow::updateProgress() {
    int totalQuestions = 10;
    int answeredQuestions = 0;

    if (mathAnalysisComboBox->currentIndex() > 0) answeredQuestions++;
    if (algebraComboBox->currentIndex() > 0) answeredQuestions++;
    if (programmingComboBox->currentIndex() > 0) answeredQuestions++;
    if (historyComboBox->currentIndex() > 0) answeredQuestions++;
    if (englishComboBox->currentIndex() > 0) answeredQuestions++;
    if (discreteMathComboBox->currentIndex() > 0) answeredQuestions++;
    if (diligenceSpinBox->value() > 0) answeredQuestions++;
    if (memorySpinBox->value() > 0) answeredQuestions++;
    if (thinkingSpeedSpinBox->value() > 0) answeredQuestions++;
    if (neatnessSpinBox->value() > 0) answeredQuestions++;

    int progress = (answeredQuestions * 100) / totalQuestions;
    progressBar->setValue(progress);
}

