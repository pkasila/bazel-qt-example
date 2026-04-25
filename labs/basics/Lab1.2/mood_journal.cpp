#include "mood_journal.h"
#include <QtGui/QKeyEvent>
#include <QtCore/QDate>

MoodJournal::MoodJournal(QWidget *parent)
    : QWidget(parent), starCount(0), glitterMode(false) {
    
    setWindowTitle("✨ Pastel Mood Journal ✨");
    setMinimumSize(500, 600);
    
    setupUI();
    applyGlitterStyle(false);
    updateStatusBar();
}

MoodJournal::~MoodJournal() {}

void MoodJournal::setupUI() {
    mainLayout = new QVBoxLayout(this);
    mainLayout->setSpacing(15);
    mainLayout->setContentsMargins(20, 20, 20, 20);
    
    // Calendar
    calendar = new QCalendarWidget(this);
    calendar->setGridVisible(true);
    connect(calendar, &QCalendarWidget::selectionChanged, this, &MoodJournal::onDateSelected);
    mainLayout->addWidget(calendar);
    
    // Mood section
    QLabel *moodLabel = new QLabel("Настроение:", this);
    mainLayout->addWidget(moodLabel);
    
    moodSlider = new QSlider(Qt::Horizontal, this);
    moodSlider->setRange(0, 100);
    moodSlider->setValue(50);
    moodSlider->setTickPosition(QSlider::TicksBelow);
    moodSlider->setTickInterval(25);
    connect(moodSlider, &QSlider::valueChanged, this, &MoodJournal::onMoodChanged);
    mainLayout->addWidget(moodSlider);
    
    // Magic progress bar
    QLabel *magicLabel = new QLabel("Шкала магии:", this);
    mainLayout->addWidget(magicLabel);
    
    magicBar = new QProgressBar(this);
    magicBar->setRange(0, 100);
    magicBar->setValue(50);
    mainLayout->addWidget(magicBar);
    
    // Thought input
    QLabel *thoughtLabel = new QLabel("Запиши свою мысль:", this);
    mainLayout->addWidget(thoughtLabel);
    
    thoughtInput = new QLineEdit(this);
    thoughtInput->setPlaceholderText("Что у тебя на душе?");
    connect(thoughtInput, &QLineEdit::returnPressed, this, &MoodJournal::onThoughtEntered);
    mainLayout->addWidget(thoughtInput);
    
    // Star catching section
    starLayout = new QHBoxLayout();
    
    catchStarButton = new QPushButton("🌟 Поймать звезду", this);
    connect(catchStarButton, &QPushButton::clicked, this, &MoodJournal::onCatchStarClicked);
    starLayout->addWidget(catchStarButton);
    
    starCountLabel = new QLabel("Звезд: 0", this);
    starLayout->addWidget(starCountLabel);
    
    starLayout->addStretch();
    mainLayout->addLayout(starLayout);
    
    // Glitter mode
    glitterCheckBox = new QCheckBox("✨ Добавить блесток", this);
    connect(glitterCheckBox, QOverload<int>::of(&QCheckBox::stateChanged), this, &MoodJournal::onGlitterToggled);
    mainLayout->addWidget(glitterCheckBox);
    
    // Status
    statusLabel = new QLabel("Добро пожаловать в уютный дневник настроения!", this);
    mainLayout->addWidget(statusLabel);
    
    mainLayout->addStretch();
}

void MoodJournal::onCatchStarClicked() {
    starCount++;
    starCountLabel->setText(QString("Звезд: %1").arg(starCount));
    updateStatusBar();
    
    if (starCount % 10 == 0) {
        statusLabel->setText(QString("🎉 Ура! %1 звезд собрано!").arg(starCount));
    }
}

void MoodJournal::onMoodChanged(int value) {
    magicBar->setValue(value);
    updateStatusBar();
    
    if (value >= 80) {
        statusLabel->setText("😊 Отличное настроение! Магия на максимуме!");
    } else if (value >= 50) {
        statusLabel->setText("😌 Хорошее настроение, продолжай в том же духе!");
    } else if (value >= 20) {
        statusLabel->setText("😔 Погрустнело... Может, поймаем звездочку?");
    } else {
        statusLabel->setText("😢 Очень грустно... Звезды помогут!");
    }
}

void MoodJournal::onThoughtEntered() {
    QString thought = thoughtInput->text().trimmed();
    if (!thought.isEmpty()) {
        statusLabel->setText("💭 Мысль сохранена в сердце...");
        thoughtInput->clear();
        
        // Add a small star bonus for sharing thoughts
        starCount++;
        starCountLabel->setText(QString("Звезд: %1").arg(starCount));
    }
}

void MoodJournal::onGlitterToggled(int state) {
    glitterMode = (state == Qt::Checked);
    applyGlitterStyle(glitterMode);
    updateStatusBar();
}

void MoodJournal::onDateSelected() {
    QDate selectedDate = calendar->selectedDate();
    statusLabel->setText(QString("📅 Выбрана дата: %1").arg(selectedDate.toString("dd.MM.yyyy")));
}

void MoodJournal::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_S) {
        onCatchStarClicked();
    } else {
        QWidget::keyPressEvent(event);
    }
}

void MoodJournal::applyGlitterStyle(bool enabled) {
    QString baseStyle = R"(
        QWidget {
            background-color: #FFF5F8;
            color: #8B7D8B;
            font-family: 'Arial', sans-serif;
            font-size: 12px;
        }
        QPushButton {
            background-color: #FFB6C1;
            border: none;
            border-radius: 15px;
            padding: 10px 20px;
            font-weight: bold;
            color: white;
        }
        QPushButton:hover {
            background-color: #FFA0B4;
        }
        QPushButton:pressed {
            background-color: #FF8FA3;
        }
        QLineEdit {
            border: 2px solid #FFB6C1;
            border-radius: 10px;
            padding: 8px;
            background-color: white;
            color: #8B7D8B;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 8px;
            background-color: #FFD1DC;
            border-radius: 4px;
        }
        QSlider::handle:horizontal {
            background-color: #FF69B4;
            border: none;
            width: 18px;
            height: 18px;
            border-radius: 9px;
            margin: -5px 0;
        }
        QProgressBar {
            border: none;
            border-radius: 10px;
            text-align: center;
            background-color: #FFD1DC;
            color: #8B7D8B;
            font-weight: bold;
        }
        QProgressBar::chunk {
            background-color: #FF69B4;
            border-radius: 8px;
        }
        QCheckBox {
            color: #8B7D8B;
            font-weight: bold;
        }
        QCheckBox::indicator {
            width: 18px;
            height: 18px;
            border-radius: 9px;
            border: 2px solid #FFB6C1;
            background-color: white;
        }
        QCheckBox::indicator:checked {
            background-color: #FF69B4;
            border-color: #FF69B4;
        }
        QCalendarWidget {
            background-color: white;
            color: #8B7D8B;
        }
        QCalendarWidget QToolButton {
            color: #8B7D8B;
            background-color: #FFB6C1;
            border-radius: 5px;
        }
        QCalendarWidget QAbstractItemView {
            background-color: white;
            selection-background-color: #FFB6C1;
            selection-color: white;
        }
    )";
    
    if (enabled) {
        baseStyle += R"(
            QWidget {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1, 
                    stop:0 #FFF5F8, stop:0.5 #FFE4E1, stop:1 #FFF5F8);
            }
            QPushButton {
                background: qradialgradient(cx:0.5, cy:0.5, radius:0.7,
                    fx:0.3, fy:0.3, stop:0 #FFD700, stop:1 #FF69B4);
                border: 2px solid #FFD700;
            }
            QPushButton:hover {
                background: qradialgradient(cx:0.5, cy:0.5, radius:0.7,
                    fx:0.3, fy:0.3, stop:0 #FFED4E, stop:1 #FF69B4);
                border: 2px solid #FFD700;
            }
            QLineEdit {
                border: 2px solid #FFD700;
                background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                    stop:0 white, stop:1 #FFFACD);
            }
            QSlider::handle:horizontal {
                background: qradialgradient(cx:0.5, cy:0.5, radius:0.5,
                    stop:0 #FFD700, stop:1 #FFA500);
                border: 2px solid #FFD700;
            }
            QProgressBar::chunk {
                background: qlineargradient(x1:0, y1:0, x2:1, y2:0,
                    stop:0 #FFD700, stop:0.5 #FFA500, stop:1 #FFD700);
            }
        )";
    }
    
    setStyleSheet(baseStyle);
}

void MoodJournal::updateStatusBar() {
    QString mood = QString("Настроение: %1%").arg(moodSlider->value());
    QString stars = QString("Звезд: %1").arg(starCount);
    QString glitter = glitterMode ? "✨ Блестящий режим" : "";
    
    QString fullStatus = QString("%1 | %2 %3").arg(mood, stars, glitter);
    
    if (QStatusBar *statusBar = findChild<QStatusBar*>()) {
        statusBar->showMessage(fullStatus);
    }
}

