#include "dev_levelup.h"

#include <QtWidgets>

DevLevelUp::DevLevelUp(QWidget* parent) : QWidget(parent) {
    setupUI();

    this->setFocusPolicy(Qt::StrongFocus);

    setWindowTitle("DevLevelUp: RPG Grind Simulator");
    resize(900, 600);
    setMinimumSize(700, 500);
}

void DevLevelUp::setupUI() {
    QHBoxLayout* mainLayout = new QHBoxLayout(this);

    QVBoxLayout* leftLayout = new QVBoxLayout();

    m_heroLabel = new QLabel("Junior Developer");
    m_heroLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #3498db;");
    leftLayout->addWidget(m_heroLabel);

    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Твой никнейм...");
    leftLayout->addWidget(new QLabel("GitHub Username:"));
    leftLayout->addWidget(m_nameEdit);

    m_hoursSpin = new QSpinBox();
    m_hoursSpin->setRange(0, 999'999);
    m_hoursSpin->setSuffix(" ч.");
    leftLayout->addWidget(new QLabel("Часов практики:"));
    leftLayout->addWidget(m_hoursSpin);

    m_hardcoreCheck = new QCheckBox("Hardcore Mode (No Help)");
    leftLayout->addWidget(m_hardcoreCheck);

    m_grindBtn = new QPushButton("ГРИНДИТЬ (XP +10)");
    m_grindBtn->setMinimumHeight(60);
    m_grindBtn->setCursor(Qt::PointingHandCursor);
    leftLayout->addWidget(m_grindBtn);

    leftLayout->addStretch();

    QVBoxLayout* rightLayout = new QVBoxLayout();

    QHBoxLayout* levelBox = new QHBoxLayout();
    levelBox->addWidget(new QLabel("LEVEL:"));
    m_levelDisplay = new QLCDNumber();
    m_levelDisplay->setSegmentStyle(QLCDNumber::Flat);
    m_levelDisplay->setFrameStyle(QFrame::NoFrame);
    m_levelDisplay->display(m_currentLevel);
    levelBox->addWidget(m_levelDisplay);
    rightLayout->addLayout(levelBox);

    m_rankLabel = new QLabel("Rank: Unpaid Intern");
    m_rankLabel->setStyleSheet("font-size: 18px; color: #e67e22; font-style: italic;");
    rightLayout->addWidget(m_rankLabel);

    m_xpBar = new QProgressBar();
    m_xpBar->setRange(0, 100);
    m_xpBar->setValue(0);
    m_xpBar->setFormat("XP: %v / 100");
    rightLayout->addWidget(m_xpBar);

    rightLayout->addWidget(new QLabel("Skills (Double click to master):"));
    m_skillList = new QListWidget();
    QStringList skills = {"C++ Pointers",   "Qt Signals/Slots", "Memory Management",
                          "Multithreading", "Algorithms",       "Clean Code"};
    for (const QString& s : skills) {
        QListWidgetItem* item = new QListWidgetItem(s);
        item->setData(Qt::UserRole, false);
        m_skillList->addItem(item);
    }
    rightLayout->addWidget(m_skillList);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addLayout(rightLayout, 2);

    connect(m_nameEdit, &QLineEdit::textChanged, this, &DevLevelUp::updateHeroTitle);
    connect(
        m_hoursSpin, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &DevLevelUp::onHoursChanged);
    connect(m_hardcoreCheck, &QCheckBox::toggled, this, &DevLevelUp::onHardcoreToggled);
    connect(m_grindBtn, &QPushButton::clicked, this, &DevLevelUp::onGrindClicked);
    connect(m_skillList, &QListWidget::itemDoubleClicked, this, &DevLevelUp::onSkillDoubleClicked);
}

void DevLevelUp::updateHeroTitle(const QString& name) {
    m_heroLabel->setText(name.isEmpty() ? "Junior Developer" : name);
}

void DevLevelUp::onHoursChanged(int hours) {
    if (hours > m_totalHours) {
        addExperience(hours - m_totalHours);
    }
    m_totalHours = hours;
}

void DevLevelUp::onHardcoreToggled(bool checked) {
    updateRank();
}

void DevLevelUp::onGrindClicked() {
    addExperience(10);
    QToolTip::showText(QCursor::pos(), "+10 XP! Гринд не остановить!");
}

void DevLevelUp::onSkillDoubleClicked(QListWidgetItem* item) {
    bool isMastered = item->data(Qt::UserRole).toBool();
    if (!isMastered) {
        item->setData(Qt::UserRole, true);
        item->setBackground(QColor("#27ae60"));
        item->setForeground(Qt::white);
        item->setText(item->text() + " [MASTERED]");
        addExperience(25);
    }
}

void DevLevelUp::addExperience(int amount) {
    m_currentXP += amount;
    while (m_currentXP >= 100) {
        m_currentXP -= 100;
        m_currentLevel++;
        m_levelDisplay->display(m_currentLevel);
        updateRank();
        QMessageBox::information(
            this, "LEVEL UP", QString("Поздравляем! Ваш новый уровень: %1").arg(m_currentLevel));
    }
    m_xpBar->setValue(m_currentXP);
}

void DevLevelUp::updateRank() {
    QString rank;
    if (m_currentLevel < 5) {
        rank = "Unpaid Intern";
    } else if (m_currentLevel < 10) {
        rank = "Junior Dev";
    } else if (m_currentLevel < 20) {
        rank = "Middle Dev";
    } else if (m_currentLevel < 40) {
        rank = "Senior Architect";
    } else {
        rank = "CTO / God of Code";
    }

    if (m_hardcoreCheck->isChecked()) {
        m_rankLabel->setText("Rank: " + rank + " [HARDCORE]");
        m_rankLabel->setStyleSheet("font-size: 18px; color: #e74c3c; font-weight: bold;");
    } else {
        m_rankLabel->setText("Rank: " + rank);
        m_rankLabel->setStyleSheet("font-size: 18px; color: #e67e22; font-style: italic;");
    }
}

void DevLevelUp::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_Space) {
        if (!m_nameEdit->hasFocus()) {
            onGrindClicked();
            event->accept();
            return;
        }
    }

    if (event->key() == Qt::Key_Escape) {
        m_nameEdit->clear();
        this->setFocus();
        event->accept();
        return;
    }

    if (event->key() == Qt::Key_R) {
        addExperience(15);
        event->accept();
        return;
    }

    QWidget::keyPressEvent(event);
}