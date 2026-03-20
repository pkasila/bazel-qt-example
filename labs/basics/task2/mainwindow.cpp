// NOLINTBEGIN(cppcoreguidelines-owning-memory, readability-identifier-naming)
#include "mainwindow.h"

#include <QApplication>
#include <QFileDialog>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QKeyEvent>
#include <QMenu>
#include <QMenuBar>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QTimer>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent), textDisplay(new QLabel(this)), alignmentGroup(new QActionGroup(this)) {
    SetupUi();
    startNewSession();
    connect(&statsTimer, &QTimer::timeout, this, &MainWindow::updateStats);
    statsTimer.start(500);
}

MainWindow::~MainWindow() = default;

void MainWindow::SetupUi() {
    auto* centralWidget = new QWidget(this);
    auto* mainLayout = new QVBoxLayout(centralWidget);
    // TEXT DISPLAY
    trainingText = "The quick brown fox jumps over the lazy dog";
    textDisplay->setAlignment(Qt::AlignCenter);
    textDisplay->setWordWrap(false);
    textDisplay->setStyleSheet(
        "font-size: 28px; font-family: monospace; margin: 30px; color: #2c3e50;");

    QFontMetrics metrics(textDisplay->font());
    int charWidth = metrics.horizontalAdvance('X');
    textDisplay->setMinimumWidth((charPerLine * charWidth) + 40);

    textDisplay->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    // PROGRESS BAR
    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(false);
    progressBar->setStyleSheet(
        "QProgressBar { height: 12px; border-radius: 6px; background: #ecf0f1; }"
        "QProgressBar::chunk { background: #3498db; border-radius: 6px; }");

    // PAUSE
    textPaused = new QLabel("PAUSED", this);
    textPaused->setAlignment(Qt::AlignCenter);
    textPaused->setStyleSheet(
        "font-size: 32px; font-weight: bold; color: #3498db;"
        "background-color: rgba(255, 255, 255, 0.8);"
        "border-radius: 12px; padding: 15px;");

    auto* pauseShadow = new QGraphicsDropShadowEffect(this);
    pauseShadow->setBlurRadius(15);
    pauseShadow->setColor(QColor(0, 0, 0, 100));
    pauseShadow->setOffset(2, 2);
    textPaused->setGraphicsEffect(pauseShadow);
    textPaused->hide();

    // MENU
    auto* menuBar = new QMenuBar(this);
    QMenu* fileMenu = menuBar->addMenu("File");
    setMenuBar(menuBar);

    auto* openAction = new QAction("Open File", this);
    openAction->setShortcut(QKeySequence::Open);
    alignmentGroup->addAction(openAction);
    fileMenu->addAction(openAction);

    // STATS
    auto* statsContainer = new QFrame(this);
    statsContainer->setStyleSheet("border-radius: 8px; padding: 12px;");

    auto* topLayout = new QHBoxLayout;
    statsLabel = new QLabel("WPM: 0.00 | Accuracy: 0.0% | Time: 00:00");
    statsLabel->setStyleSheet("font-size: 16px; color: #34495e; font-weight: 500;");
    auto* statsInnerLayout = new QHBoxLayout(statsContainer);
    statsInnerLayout->addWidget(statsLabel);
    topLayout->addWidget(statsContainer);
    topLayout->setAlignment(Qt::AlignCenter);

    // SALUTE LABEL
    saluteEffect = new QLabel(this);
    saluteEffect->setAlignment(Qt::AlignCenter);
    saluteEffect->setStyleSheet(
        "font-size: 72px;"
        "background: transparent;");
    saluteEffect->setText("🎉");
    saluteEffect->setFixedSize(100, 100);
    saluteEffect->hide();

    saluteAnimGroup = new QParallelAnimationGroup(this);

    auto* posAnim = new QPropertyAnimation(saluteEffect, "pos", this);
    posAnim->setDuration(1200);
    posAnim->setEasingCurve(QEasingCurve::OutQuad);

    auto* fadeAnim = new QPropertyAnimation(saluteEffect, "windowOpacity", this);
    fadeAnim->setDuration(1200);
    fadeAnim->setStartValue(1.0);
    fadeAnim->setEndValue(0.0);

    saluteAnimGroup->addAnimation(posAnim);
    saluteAnimGroup->addAnimation(fadeAnim);

    // KEYBOARD
    auto* keyboardTopLayout = new QHBoxLayout;
    auto* keyboardContainerLayout = new QVBoxLayout;
    radioEnableKeyboard = new QRadioButton("Show Keyboard", this);
    radioEnableKeyboard->setChecked(true);
    radioEnableKeyboard->setStyleSheet("color: #34495e; font-size: 14px;");
    radioEnableKeyboard->setFocusPolicy(Qt::NoFocus);

    keyboardWidget = new KeyboardWidget(this);
    auto* keyboardLayout = new QHBoxLayout;
    keyboardLayout->addWidget(keyboardWidget);

    layoutComboBox = new QComboBox(this);
    layoutComboBox->setFocusPolicy(Qt::NoFocus);
    layoutComboBox->addItem("English", static_cast<int>(KeyboardWidget::Layout::English));
    layoutComboBox->addItem("Russian", static_cast<int>(KeyboardWidget::Layout::Russian));

    keyboardTopLayout->addWidget(radioEnableKeyboard, 0, Qt::AlignLeft);
    keyboardTopLayout->addWidget(layoutComboBox);
    keyboardTopLayout->addStretch();

    keyboardContainerLayout->addLayout(keyboardLayout);
    keyboardContainerLayout->addLayout(keyboardTopLayout);

    // LAYOUTS
    mainLayout->addWidget(progressBar);
    mainLayout->addLayout(topLayout);
    mainLayout->addWidget(textDisplay);
    mainLayout->addLayout(keyboardContainerLayout);
    mainLayout->addWidget(textPaused);
    mainLayout->setAlignment(textPaused, Qt::AlignCenter);
    setCentralWidget(centralWidget);

    // CONNECTIONS
    connect(openAction, &QAction::triggered, this, &MainWindow::loadTrainingText);
    connect(radioEnableKeyboard, &QRadioButton::toggled, keyboardWidget, &QWidget::setVisible);
    connect(radioEnableKeyboard, &QRadioButton::toggled, layoutComboBox, &QWidget::setVisible);
    connect(
        layoutComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::onLayoutChanged);
}

void MainWindow::onLayoutChanged(int index) {
    QVariant data = layoutComboBox->itemData(index);
    auto layout = static_cast<KeyboardWidget::Layout>(data.toInt());
    keyboardWidget->setLayout(layout);
}

void MainWindow::loadTrainingText() {
    QString fileName =
        QFileDialog::getOpenFileName(this, "Open File", "", "Text Files (*.txt);;All Files (*)");

    if (fileName.isEmpty()) {
        return;
    }

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, "Error", "Could not open the file.");
        return;
    }

    QTextStream in(&file);
    QString filteredText;
    for (const QChar& c : in.readAll()) {
        if (c.isLetterOrNumber() || c.isSpace() || c.isPunct()) {
            filteredText.append(c);
        }
    }
    trainingText = filteredText;
    file.close();

    startNewSession();
}

void MainWindow::resizeEvent(QResizeEvent* event) {
    QMainWindow::resizeEvent(event);
    QFontMetrics metrics(textDisplay->font());
    int availableWidth = textDisplay->width() - 80;
    charPerLine = std::min(80, availableWidth / metrics.averageCharWidth());
    startNewSession();
}

void MainWindow::startNewSession() {
    currentLineIndex = 0;
    currentPositionInLine = 0;
    totalTyped = 0;
    correctTyped = 0;
    sessionActive = false;
    activeTimer.invalidate();
    pausedTimer.invalidate();
    activems = 0;
    pausedms = 0;
    updateStats();

    if (!trainingText.isEmpty()) {
        Qt::Key key = static_cast<Qt::Key>(trainingText.at(0).toUpper().unicode());
        keyboardWidget->highlightKey(key, false);
    }

    trainingLines.clear();
    for (int i = 0; i < trainingText.length(); i += charPerLine) {
        trainingLines.append(trainingText.mid(i, charPerLine));
    }

    updateDisplay();
}

void MainWindow::updateDisplay() {
    if (trainingLines.isEmpty() || currentLineIndex >= trainingLines.size()) {
        textDisplay->setText("");
        return;
    }

    QString currentLine = trainingLines[currentLineIndex];
    QString formattedCurrentLine;
    for (int i = 0; i < currentLine.length(); ++i) {
        QString color;
        if (i < currentPositionInLine) {
            color = "green";
        } else if (i == currentPositionInLine) {
            color = sessionActive ? "blue" : "gray";
        } else {
            color = "gray";
        }

        QChar character = currentLine[i];
        if (character == ' ') {
            character = QChar(0x00B7);  // '·'
        }

        formattedCurrentLine +=
            QString("<span style='color: %1'>%2</span>").arg(color).arg(character);
    }

    QChar nextChar;
    if (currentPositionInLine < currentLine.length()) {
        nextChar = currentLine[currentPositionInLine];
    }
    QString nextLine;
    if (currentLineIndex + 1 < trainingLines.size()) {
        nextLine = trainingLines[currentLineIndex + 1];
        nextLine.replace(' ', QChar(0x00B7));
        if (currentPositionInLine >= currentLine.length()) {
            nextChar = trainingLines[currentLineIndex + 1].at(0);
            QString firstChar = QString("<span style='color: blue;'>%1</span>").arg(nextLine.at(0));
            nextLine.remove(0, 1);
            nextLine = firstChar + QString("<span style='color: gray'>%1</span>").arg(nextLine);
        } else {
            nextLine = QString("<span style='color: gray'>%1</span>").arg(nextLine);
        }
    }

    if (!nextChar.isNull()) {
        Qt::Key key = static_cast<Qt::Key>(nextChar.toUpper().unicode());

        if (keyboardWidget->currentLayout == KeyboardWidget::Layout::Russian) {
            static QHash<QChar, Qt::Key> ruToEnKeys = {
              {QString("Й")[0], Qt::Key_Q},
              {QString("Ц")[0], Qt::Key_W},
              {QString("У")[0], Qt::Key_E},
              {QString("К")[0], Qt::Key_R},
              {QString("Е")[0], Qt::Key_T},
              {QString("Н")[0], Qt::Key_Y},
              {QString("Г")[0], Qt::Key_U},
              {QString("Ш")[0], Qt::Key_I},
              {QString("Щ")[0], Qt::Key_O},
              {QString("З")[0], Qt::Key_P},
              {QString("Ф")[0], Qt::Key_A},
              {QString("Ы")[0], Qt::Key_S},
              {QString("В")[0], Qt::Key_D},
              {QString("А")[0], Qt::Key_F},
              {QString("П")[0], Qt::Key_G},
              {QString("Р")[0], Qt::Key_H},
              {QString("О")[0], Qt::Key_J},
              {QString("Л")[0], Qt::Key_K},
              {QString("Д")[0], Qt::Key_L},
              {QString("Ж")[0], Qt::Key_Semicolon},
              {QString("Я")[0], Qt::Key_Z},
              {QString("Ч")[0], Qt::Key_X},
              {QString("С")[0], Qt::Key_C},
              {QString("М")[0], Qt::Key_V},
              {QString("И")[0], Qt::Key_B},
              {QString("Т")[0], Qt::Key_N},
              {QString("Ь")[0], Qt::Key_M},
              {QString("Б")[0], Qt::Key_Comma},
              {QString("Ю")[0], Qt::Key_Period},
              {'.', Qt::Key_Slash}  // Добавляем это сопоставление
            };

            if (ruToEnKeys.contains(nextChar.toUpper())) {
                key = ruToEnKeys[nextChar.toUpper()];
            }
        }

        keyboardWidget->highlightKey(key, false);
    }

    textDisplay->setText(formattedCurrentLine + "<br/>" + nextLine);

    qsizetype totalChars = trainingText.length();
    int currentPos = (currentLineIndex * charPerLine) + currentPositionInLine;
    int progress = (totalChars > 0)
                       ? static_cast<int>((static_cast<qsizetype>(currentPos * 100)) / totalChars)
                       : 0;
    progressBar->setValue(progress);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (trainingText.isEmpty()) {
        return;
    }
    if (event->key() == Qt::Key_Escape) {
        pauseSession();
        textPaused->show();
        return;
    }
    if (!sessionActive) {
        resumeSession();
        textPaused->hide();
    }
    if (event->key() == Qt::Key_Tab) {
        startNewSession();
        pauseSession();
    } else if (event->key() == Qt::Key_Backspace) {
        handleBackspace();
    } else if (!event->text().isEmpty()) {
        processInput(event->text());
    }

    QMainWindow::keyPressEvent(event);
}

void MainWindow::pauseSession() {
    if (!sessionActive) {
        return;
    }
    if (activeTimer.isValid()) {
        activems += activeTimer.elapsed();
    }
    statsTimer.stop();
    activeTimer.invalidate();
    pausedTimer.start();
    sessionActive = false;
}

void MainWindow::resumeSession() {
    if (sessionActive) {
        return;
    }
    if (pausedTimer.isValid()) {
        pausedms += pausedTimer.elapsed();
    }
    statsTimer.start(500);
    pausedTimer.invalidate();
    activeTimer.start();
    sessionActive = true;
}

void MainWindow::processInput(const QString& input) {
    if (trainingLines.isEmpty() || currentLineIndex >= trainingLines.size()) {
        startNewSession();
        return;
    }

    QString currentLine = trainingLines[currentLineIndex];
    if (currentPositionInLine >= currentLine.length()) {
        currentLineIndex++;
        currentPositionInLine = 0;
        if (currentLineIndex >= trainingLines.size()) {
            showCompletionMessage();
            return;
        }
        currentLine = trainingLines[currentLineIndex];
    }

    bool correct = (input == currentLine.at(currentPositionInLine));
    if (correct) {
        currentPositionInLine++;
        correctTyped++;
    }
    totalTyped++;

    updateDisplay();

    if (currentLineIndex >= trainingLines.size() ||
        (currentLineIndex == trainingLines.size() - 1 &&
         currentPositionInLine >= trainingLines[currentLineIndex].length())) {
        showCompletionMessage();
    }
}

void MainWindow::handleBackspace() {
    if (currentPositionInLine > 0) {
        currentPositionInLine--;
        totalTyped++;
        if (correctTyped > 0) {
            correctTyped--;
        }
    } else if (currentLineIndex > 0) {
        currentLineIndex--;
        currentPositionInLine = static_cast<int>(trainingLines[currentLineIndex].length()) - 1;
        totalTyped++;
        if (correctTyped > 0) {
            correctTyped--;
        }
    }

    updateDisplay();
}

void MainWindow::showCompletionMessage() {
    pauseSession();

    QPoint startPos = rect().center() + QPoint(0, 100);
    saluteEffect->move(startPos);
    saluteEffect->setWindowOpacity(1.0);
    saluteEffect->show();
    saluteEffect->raise();

    auto* posAnim = dynamic_cast<QPropertyAnimation*>(saluteAnimGroup->animationAt(0));
    posAnim->setStartValue(startPos);
    posAnim->setEndValue(startPos - QPoint(0, 150));

    saluteAnimGroup->start();

    QTimer::singleShot(1500, [this]() { saluteEffect->hide(); });
}

void MainWindow::updateStats() {
    if (sessionActive) {
        pauseSession();
        resumeSession();
    }
    qint64 elapsed = activems;

    int totalSeconds = static_cast<int>(elapsed / 1000);
    int minutes = totalSeconds / 60;
    int seconds = totalSeconds % 60;
    QString timeString = QString("%1:%2")
                             .arg(minutes, 2, 10, QLatin1Char('0'))
                             .arg(seconds, 2, 10, QLatin1Char('0'));

    double minutesDouble = static_cast<double>(elapsed) / 60000.0;
    double wpm = (minutesDouble > 0) ? (correctTyped / 5.0) / minutesDouble : 0.0;
    double accuracy = (totalTyped > 0) ? (correctTyped * 100.0 / totalTyped) : 0.0;

    statsLabel->setText(
        QString("<b>WPM:</b> <span style='color: #2980b9;'>%1</span> | "
                "<b>Accuracy:</b> <span style='color: #27ae60;'>%2%</span> | "
                "<b>Time:</b> <span style='color: #8e44ad;'>%3</span>")
            .arg(wpm, 0, 'f', 2)
            .arg(accuracy, 0, 'f', 1)
            .arg(timeString));
}

// NOLINTEND(cppcoreguidelines-owning-memory, readability-identifier-naming)