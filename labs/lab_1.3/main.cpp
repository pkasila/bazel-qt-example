#include "Header.h"
#include <QtWidgets/QApplication>
#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtCore/QRect>

const QRect WaifuManager::HEAD_AREA(150, 50, 100, 100);
const QStringList WaifuManager::OUTFITS = {
    "school_uniform",
    "casual",
    "maid",
    "yukata",
    "idol",
    "nurse",
    "teacher",
    "cat_girl"
};

WaifuManager::WaifuManager(QWidget *parent) : QMainWindow(parent), 
    clickCount(0), happiness(50), isShy(false), ecchiMode(false), currentOutfit("school_uniform") {
    setupUI();
    applyEcchiMode(false);
    updateCharacterImage("normal");
    
    happinessTimer = new QTimer(this);
    connect(happinessTimer, &QTimer::timeout, [this]() { onHappinessDecay(); });
    happinessTimer->start(2000);
    
    shynessTimer = new QTimer(this);
    connect(shynessTimer, &QTimer::timeout, [this]() { resetShyness(); });
}

WaifuManager::~WaifuManager() {}

void WaifuManager::setupUI() {
    centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    mainLayout = new QHBoxLayout(centralWidget);
    
    leftLayout = new QVBoxLayout();
    characterLabel = new QLabel();
    characterLabel->setAlignment(Qt::AlignCenter);
    characterLabel->setMinimumSize(400, 400);
    characterLabel->setStyleSheet("QLabel { background-color: #ffe0f0; border: 2px solid #ff69b4; }");
    characterLabel->setText("👧\n\n[Персонаж]\n\nКликни на голову!");
    leftLayout->addWidget(characterLabel);
    
    rightLayout = new QVBoxLayout();
    
    QGroupBox *controlGroup = new QGroupBox("Управление");
    QVBoxLayout *controlLayout = new QVBoxLayout(controlGroup);
    
    petButton = new QPushButton("🐾 Гладить");
    connect(petButton, &QPushButton::clicked, [this]() { onPetButtonClicked(); });
    petButton->setStyleSheet("QPushButton { font-size: 16px; padding: 10px; background-color: #ffb6c1; }");
    controlLayout->addWidget(petButton);
    
    controlLayout->addWidget(new QLabel("Счастье:"));
    happinessBar = new QProgressBar();
    happinessBar->setRange(0, 100);
    happinessBar->setValue(happiness);
    happinessBar->setStyleSheet("QProgressBar { text-align: center; }");
    controlLayout->addWidget(happinessBar);
    
    controlLayout->addWidget(new QLabel("Клики:"));
    clickCounter = new QLCDNumber();
    clickCounter->setSegmentStyle(QLCDNumber::Flat);
    clickCounter->setDigitCount(4);
    clickCounter->display(clickCount);
    clickCounter->setStyleSheet("QLCDNumber { background-color: #000; color: #0f0; }");
    controlLayout->addWidget(clickCounter);
    
    ecchiModeCheckBox = new QCheckBox("Ecchi Mode 🔞");
    connect(ecchiModeCheckBox, &QCheckBox::toggled, [this](bool enabled) { onEcchiModeToggled(enabled); });
    controlLayout->addWidget(ecchiModeCheckBox);
    
    controlLayout->addWidget(new QLabel("Скорость падения счастья:"));
    speedSlider = new QSlider(Qt::Horizontal);
    speedSlider->setRange(1, 10);
    speedSlider->setValue(5);

    speedSlider->setStyleSheet(
        "QSlider::groove:horizontal {"
        "    border: 1px solid #bbb;"
        "    height: 10px;"
        "    background: #eee;"
        "    margin: 2px 0;"
        "    border-radius: 5px;"
        "}"
        "QSlider::handle:horizontal {"
        "    background: #ff69b4;"
        "    border: 1px solid #ff1493;"
        "    width: 20px;"
        "    height: 20px;"
        "    margin: -7px 0;"
        "    border-radius: 10px;"
        "}"
        "QSlider::sub-page:horizontal {"
        "    background: #ffb6c1;"
        "    border-radius: 5px;"
        "}"
    );

    connect(speedSlider, &QSlider::valueChanged, [this](int value) { 
        onSpeedChanged(value); 
    });

    controlLayout->addWidget(speedSlider);
    
    rightLayout->addWidget(controlGroup);
    
    QGroupBox *wardrobeGroup = new QGroupBox("Гардероб");
    QVBoxLayout *wardrobeMainLayout = new QVBoxLayout(wardrobeGroup);
    
    wardrobeScrollArea = new QScrollArea();
    wardrobeWidget = new QWidget();
    wardrobeLayout = new QVBoxLayout(wardrobeWidget);
    
    for (const QString &outfit : OUTFITS) {
        QString outfitName = outfit;
        QPushButton *outfitButton = new QPushButton(outfitName.replace("_", " ").toUpper());
        outfitButton->setStyleSheet("QPushButton { padding: 5px; margin: 2px; }");
        connect(outfitButton, &QPushButton::clicked, [this, outfit]() {
            onOutfitChanged(outfit);
        });
        wardrobeLayout->addWidget(outfitButton);
    }
    
    wardrobeLayout->addStretch();
    wardrobeScrollArea->setWidget(wardrobeWidget);
    wardrobeScrollArea->setWidgetResizable(true);
    wardrobeMainLayout->addWidget(wardrobeScrollArea);
    
    rightLayout->addWidget(wardrobeGroup);
    rightLayout->addStretch();
    
    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addLayout(rightLayout, 1);
    
    setWindowTitle("Waifu Mood Manager ❤️");
    resize(800, 600);
}

void WaifuManager::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        QPoint pos = characterLabel->mapFrom(this, event->pos());
        
        if (ecchiMode) {
            if (characterLabel->rect().contains(pos)) {
                updateCharacterImage("shy");
                isShy = true;
                shynessTimer->start(2000);
            }
        } else {
            if (HEAD_AREA.contains(pos)) {
                happiness = qMin(100, happiness + 5);
                happinessBar->setValue(happiness);
                
                if (happiness >= 80) {
                    updateCharacterImage("happy");
                } else if (happiness >= 40) {
                    updateCharacterImage("normal");
                } else {
                    updateCharacterImage("sad");
                }
            }
        }
    }
    QMainWindow::mousePressEvent(event);
}

void WaifuManager::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_S) {
        happiness = 100;
        happinessBar->setValue(happiness);
        updateCharacterImage("happy");
    }
    QMainWindow::keyPressEvent(event);
}

void WaifuManager::onPetButtonClicked() {
    clickCount++;
    clickCounter->display(clickCount);
    
    happiness = qMin(100, happiness + 10);
    happinessBar->setValue(happiness);
    
    if (happiness >= 80) {
        updateCharacterImage("happy");
    } else if (happiness >= 40) {
        updateCharacterImage("normal");
    } else {
        updateCharacterImage("sad");
    }
}

void WaifuManager::onEcchiModeToggled(bool enabled) {
    ecchiMode = enabled;
    applyEcchiMode(enabled);
}

void WaifuManager::onSpeedChanged(int value) {
    int interval = 3000 / value;
    happinessTimer->setInterval(interval);
}

void WaifuManager::onOutfitChanged(const QString &outfit) {
    currentOutfit = outfit;
    updateCharacterImage(isShy ? "shy" : (happiness >= 80 ? "happy" : (happiness >= 40 ? "normal" : "sad")));
}

void WaifuManager::onHappinessDecay() {
    if (happiness > 0) {
        happiness = qMax(0, happiness - speedSlider->value());
        happinessBar->setValue(happiness);
        
        if (!isShy) {
            if (happiness >= 80) {
                updateCharacterImage("happy");
            } else if (happiness >= 40) {
                updateCharacterImage("normal");
            } else {
                updateCharacterImage("sad");
            }
        }
    }
}

void WaifuManager::applyEcchiMode(bool enabled) {
    QString styleSheet;
    if (enabled) {
        styleSheet = "QMainWindow { background-color: #2d1b69; }"
                    "QGroupBox { color: #ff69b4; font-weight: bold; }"
                    "QPushButton { background-color: #ff1493; color: white; }"
                    "QProgressBar::chunk { background-color: #ff69b4; }"
                    "QLCDNumber { background-color: #2d1b69; color: #ff69b4; }";
    } else {
        styleSheet = "QMainWindow { background-color: #f0f0f0; }"
                    "QGroupBox { color: #333; font-weight: bold; }"
                    "QPushButton { background-color: #e0e0e0; }"
                    "QProgressBar::chunk { background-color: #4caf50; }"
                    "QLCDNumber { background-color: #000; color: #0f0; }";
    }
    setStyleSheet(styleSheet);
}

void WaifuManager::updateCharacterImage(const QString &mood) {
    QString fileName;

    if (mood == "shy") fileName = "1.8.jpg";
    else if (mood == "happy") fileName = "1.2.jpg";
    else if (mood == "sad") fileName = "1.0.jpg";
    else fileName = "1.1.jpg";

    QString relativePath = "labs/lab_1.3/" + fileName;
    
    QPixmap pixmap(relativePath);

    if (pixmap.isNull()) {
        QString emoji = (mood == "shy") ? "😳" : (mood == "happy") ? "😊" : (mood == "sad") ? "😢" : "😐";
        QString outfitText = currentOutfit.replace("_", " ").toUpper();
        
        characterLabel->setPixmap(QPixmap());
        characterLabel->setStyleSheet(
            "QLabel {"
            "   background-color: #ffe0f0;"
            "   border: 2px solid #ff69b4;"
            "   border-radius: 15px;"
            "   font-size: 24px;"
            "}"
        );
        characterLabel->setText(QString("%1\n\n%2\n\nСчастье: %3%").arg(emoji).arg(outfitText).arg(happiness));
        
    } else {
        characterLabel->setText("");
        characterLabel->setPixmap(pixmap.scaled(characterLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
        
        characterLabel->setAlignment(Qt::AlignCenter);
        characterLabel->setStyleSheet(
            "QLabel {"
            "   border: 3px solid #ff69b4;"
            "   border-radius: 15px;"
            "   background-color: #222;"
            "}"
        );
    }
}

void WaifuManager::resetShyness() {
    isShy = false;
    if (happiness >= 80) {
        updateCharacterImage("happy");
    } else if (happiness >= 40) {
        updateCharacterImage("normal");
    } else {
        updateCharacterImage("sad");
    }
}

int main(int argc, char *argv[]) {
    QApplication app(argc, argv);
    
    WaifuManager window;
    window.show();
    
    return app.exec();
}
