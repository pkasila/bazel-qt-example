#include "main_window.h"

#include "exercise.h"

#include <QApplication>
#include <QAudioOutput>
#include <QButtonGroup>
#include <QCoreApplication>
#include <QFontDatabase>
#include <QInputDialog>
#include <QMessageBox>
#include <QPalette>
#include <QRadioButton>
#include <QShortcut>
#include <QStyleFactory>
#include <QTextEdit>
#include <QUrl>

// NOLINTBEGIN

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , remainingTime_(0)
    , currentScore_(0)
    , currentExercise_(0)
    , totalExercises_(5)
    , difficultyLevel_(1)
    , maxAttempts_(3)
    , currentAttempts_(0) {
    qApp->setStyle(QStyleFactory::create("Fusion"));

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(25, 25, 25));
    darkPalette.setColor(QPalette::AlternateBase, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);
    qApp->setPalette(darkPalette);

    setupUI();
    setupMenuBar();
    setupExerciseWidgets();

    QShortcut* helpShortcut = new QShortcut(QKeySequence("H"), this);
    connect(helpShortcut, &QShortcut::activated, this, &MainWindow::showHelp);
}

void MainWindow::setupUI() {
    QWidget* centralWidget = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    QHBoxLayout* topBar = new QHBoxLayout();
    topBar->setSpacing(20);

    QString labelStyle = "QLabel { color: white; font-size: 14px; font-weight: bold; }";

    scoreLabel_ = new QLabel("Score: 0", this);
    scoreLabel_->setStyleSheet(labelStyle);
    timerLabel_ = new QLabel("Time: 0:00", this);
    timerLabel_->setStyleSheet(labelStyle);

    topBar->addWidget(scoreLabel_);
    topBar->addStretch();
    topBar->addWidget(timerLabel_);
    mainLayout->addLayout(topBar);

    progressBar_ = new QProgressBar(this);
    progressBar_->setRange(0, totalExercises_);
    progressBar_->setValue(0);
    progressBar_->setStyleSheet(
        "QProgressBar {"
        "    border: 2px solid grey;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "    background-color: #2A2A2A;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #2A82DA;"
        "    border-radius: 3px;"
        "}");
    mainLayout->addWidget(progressBar_);

    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->setSpacing(10);

    QString buttonStyle =
        "QPushButton {"
        "    background-color: #2A82DA;"
        "    color: white;"
        "    border: none;"
        "    padding: 10px 20px;"
        "    border-radius: 5px;"
        "    font-size: 14px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3A92EA;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1A72CA;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #666666;"
        "}";

    translationButton_ = new QPushButton("Translation", this);
    translationButton_->setStyleSheet(buttonStyle);
    translationButton_->setMinimumHeight(40);

    grammarButton_ = new QPushButton("Grammar", this);
    grammarButton_->setStyleSheet(buttonStyle);
    grammarButton_->setMinimumHeight(40);

    buttonLayout->addWidget(translationButton_);
    buttonLayout->addWidget(grammarButton_);
    mainLayout->addLayout(buttonLayout);

    stackedWidget_ = new QStackedWidget(this);
    mainLayout->addWidget(stackedWidget_);

    submitButton_ = new QPushButton("Submit", this);
    submitButton_->setStyleSheet(buttonStyle);
    submitButton_->setMinimumHeight(40);
    submitButton_->setEnabled(false);
    mainLayout->addWidget(submitButton_);

    setCentralWidget(centralWidget);
    resize(1080, 840);

    connect(translationButton_, &QPushButton::clicked, this, &MainWindow::showTranslationExercise);
    connect(grammarButton_, &QPushButton::clicked, this, &MainWindow::showGrammarExercise);
    connect(submitButton_, &QPushButton::clicked, this, &MainWindow::checkAnswer);

    exerciseTimer_ = new QTimer(this);
    connect(exerciseTimer_, &QTimer::timeout, this, &MainWindow::updateTimer);

    auto* audio1 = new QAudioOutput(this);
    audio1->setVolume(1.0f);

    auto* audio2 = new QAudioOutput(this);
    audio2->setVolume(0.5f);

    QString appDir = QCoreApplication::applicationDirPath();

    successSound_ = new QMediaPlayer(this);
    successSound_->setAudioOutput(audio1);
    successSound_->setSource(QUrl::fromLocalFile(appDir + "/sounds/success.wav"));

    failedSound_ = new QMediaPlayer(this);
    failedSound_->setAudioOutput(audio2);
    failedSound_->setSource(QUrl::fromLocalFile(appDir + "/sounds/failed.wav"));

    successSound_->play();
}

void MainWindow::setupMenuBar() {
    QMenu* settingsMenu = menuBar()->addMenu("Settings");
    QAction* difficultyAction = settingsMenu->addAction("Set Difficulty");
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::showDifficultyDialog);
}

void MainWindow::setGrammarExercise() {
    switch (difficultyLevel_) {
        case 1:
            grammarPrompt_->setText(
                "Choose the correct form: " +
                easyGrammarExercise[currentExercises_[currentExercise_]]);
            break;
        case 2:
            grammarPrompt_->setText(
                "Choose the correct form: " +
                mediumGrammarExercise[currentExercises_[currentExercise_]]);
            break;
        case 3:
            grammarPrompt_->setText(
                "Choose the correct form: " +
                hardGrammarExercise[currentExercises_[currentExercise_]]);
            break;
    }
    for (int i = 0; i < 4; ++i) {
        switch (difficultyLevel_) {
            case 1:
                currentGrammarOptions_[i]->setText(
                    easyGrammarExerciseOptions[currentExercises_[currentExercise_] * 4 + i]);
                break;
            case 2:
                currentGrammarOptions_[i]->setText(
                    mediumGrammarExerciseOptions[currentExercises_[currentExercise_] * 4 + i]);
                break;
            case 3:
                currentGrammarOptions_[i]->setText(
                    hardGrammarExerciseOptions[currentExercises_[currentExercise_] * 4 + i]);
                break;
        }
    }
}

void MainWindow::setupExerciseWidgets() {
    translationWidget_ = new QWidget();
    QVBoxLayout* translationLayout = new QVBoxLayout(translationWidget_);
    translationLayout->setSpacing(15);
    translationLayout->setContentsMargins(20, 20, 20, 20);

    std::srand(std::time(0));
    while (currentExercises_.size() < totalExercises_) {
        int randomIndex = std::rand() % 94;
        if (std::find(currentExercises_.begin(), currentExercises_.end(), randomIndex) ==
            currentExercises_.end()) {
            currentExercises_.push_back(randomIndex);
        }
    }

    switch (difficultyLevel_) {
        case 1:
            translationPrompt_ = new QLabel(
                "Translate the following sentence: " +
                    easyTranslationExercise[currentExercises_[currentExercise_]],
                translationWidget_);
            break;
        case 2:
            translationPrompt_ = new QLabel(
                "Translate the following sentence: " +
                    mediumTranslationExercise[currentExercises_[currentExercise_]],
                translationWidget_);
            break;
        case 3:
            translationPrompt_ = new QLabel(
                "Translate the following sentence: " +
                    hardTranslationExercise[currentExercises_[currentExercise_]],
                translationWidget_);
            break;
    }
    translationPrompt_->setStyleSheet(
        "QLabel { color: white; font-size: 16px; font-weight: bold; }");

    translationInput_ = new QTextEdit(translationWidget_);
    translationInput_->setStyleSheet(
        "QTextEdit {"
        "    background-color: #2A2A2A;"
        "    color: white;"
        "    border: 2px solid #2A82DA;"
        "    border-radius: 5px;"
        "    padding: 10px;"
        "    font-size: 14px;"
        "}");
    translationInput_->setMinimumHeight(150);

    translationLayout->addWidget(translationPrompt_);
    translationLayout->addWidget(translationInput_);
    stackedWidget_->addWidget(translationWidget_);

    grammarWidget_ = new QWidget();
    grammarLayout_ = new QVBoxLayout(grammarWidget_);
    grammarLayout_->setSpacing(10); 
    grammarLayout_->setContentsMargins(20, 20, 20, 20);

    QWidget* grammarContainer = new QWidget(grammarWidget_);
    QVBoxLayout* grammarContainerLayout = new QVBoxLayout(grammarContainer);
    grammarContainerLayout->setSpacing(15);  
    grammarContainerLayout->setContentsMargins(0, 0, 0, 0);

    grammarPrompt_ = new QLabel(
        "Choose the correct form: " + easyGrammarExercise[currentExercises_[currentExercise_]],
        grammarContainer);
    grammarPrompt_->setStyleSheet(
        "QLabel {"
        "    color: white;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    padding: 5px;"
        "    margin-bottom: 10px;"
        "}");

    grammarOptions_ = new QButtonGroup(grammarWidget_);

    QString radioStyle =
        "QRadioButton {"
        "    color: white;"
        "    font-size: 14px;"
        "    padding: 8px;"
        "    margin: 2px;"
        "    background-color: #333333;"
        "    border-radius: 4px;"
        "}"
        "QRadioButton:hover {"
        "    background-color: #3A3A3A;"
        "}"
        "QRadioButton::indicator {"
        "    width: 18px;"
        "    height: 18px;"
        "}"
        "QRadioButton::indicator:unchecked {"
        "    border: 2px solid #666666;"
        "    border-radius: 9px;"
        "}"
        "QRadioButton::indicator:checked {"
        "    border: 2px solid #2A82DA;"
        "    border-radius: 9px;"
        "    background-color: #2A82DA;"
        "}";

    grammarContainerLayout->addWidget(grammarPrompt_);

    currentGrammarOptions_.resize(4);
    for (int i = 0; i < 4; ++i) {
        currentGrammarOptions_[i] = new QRadioButton(
            easyGrammarExerciseOptions[currentExercises_[currentExercise_] * 4 + i],
            grammarContainer);
        currentGrammarOptions_[i]->setStyleSheet(radioStyle);
        grammarOptions_->addButton(currentGrammarOptions_[i], i);
        grammarContainerLayout->addWidget(currentGrammarOptions_[i]);
    }

    grammarLayout_->addWidget(grammarContainer);
    grammarLayout_->addStretch();
    stackedWidget_->addWidget(grammarWidget_);
}

void MainWindow::showTranslationExercise() {
    stackedWidget_->setCurrentWidget(translationWidget_);
    submitButton_->setEnabled(true);
    startExercise();
}

void MainWindow::showGrammarExercise() {
    stackedWidget_->setCurrentWidget(grammarWidget_);
    submitButton_->setEnabled(true);
    startExercise();
}

void MainWindow::showDifficultyDialog() {
    QDialog* dialog = new QDialog(this);
    dialog->setWindowTitle("Select Difficulty Level");
    dialog->setFixedSize(750, 600);
    dialog->setStyleSheet(
        "QDialog {"
        "    background-color: #2A2A2A;"
        "    border-radius: 10px;"
        "}"
        "QLabel {"
        "    color: white;"
        "    font-size: 18px;"
        "    font-weight: bold;"
        "    padding: 10px;"
        "}");

    QVBoxLayout* mainLayout = new QVBoxLayout(dialog);
    mainLayout->setSpacing(20);
    mainLayout->setContentsMargins(30, 30, 30, 30);

    QLabel* titleLabel = new QLabel("Choose Your Challenge Level", dialog);
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    QString containerStyle =
        "QWidget {"
        "    background-color: #333333;"
        "    border-radius: 8px;"
        "    padding: 15px;"
        "    margin: 5px;"
        "}";

    QString buttonStyle =
        "QPushButton {"
        "    background-color: #2A82DA;"
        "    color: white;"
        "    border: none;"
        "    padding: 12px;"
        "    border-radius: 6px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "    min-width: 150px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #3A92EA;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #1A72CA;"
        "}";

    QString descStyle =
        "QLabel {"
        "    color: #CCCCCC;"
        "    font-size: 14px;"
        "    font-weight: normal;"
        "    padding: 5px;"
        "    line-height: 1.5;"
        "}";

    QWidget* easyContainer = new QWidget(dialog);
    easyContainer->setStyleSheet(containerStyle);
    QVBoxLayout* easyLayout = new QVBoxLayout(easyContainer);
    easyLayout->setSpacing(10);

    QPushButton* easyButton = new QPushButton("Easy", easyContainer);
    easyButton->setStyleSheet(buttonStyle);
    QLabel* easyDesc = new QLabel(
        "• Basic vocabulary and simple sentences\n• More time per exercise\n• 5 exercises",
        easyContainer);
    easyDesc->setStyleSheet(descStyle);
    easyDesc->setWordWrap(true);
    easyLayout->addWidget(easyButton);
    easyLayout->addWidget(easyDesc);
    mainLayout->addWidget(easyContainer);

    QWidget* mediumContainer = new QWidget(dialog);
    mediumContainer->setStyleSheet(containerStyle);
    QVBoxLayout* mediumLayout = new QVBoxLayout(mediumContainer);
    mediumLayout->setSpacing(10);

    QPushButton* mediumButton = new QPushButton("Medium", mediumContainer);
    mediumButton->setStyleSheet(buttonStyle);
    QLabel* mediumDesc = new QLabel(
        "• Intermediate vocabulary and complex sentences\n• Standard time limit\n• 7 exercises",
        mediumContainer);
    mediumDesc->setStyleSheet(descStyle);
    mediumDesc->setWordWrap(true);
    mediumLayout->addWidget(mediumButton);
    mediumLayout->addWidget(mediumDesc);
    mainLayout->addWidget(mediumContainer);

    QWidget* hardContainer = new QWidget(dialog);
    hardContainer->setStyleSheet(containerStyle);
    QVBoxLayout* hardLayout = new QVBoxLayout(hardContainer);
    hardLayout->setSpacing(10);

    QPushButton* hardButton = new QPushButton("Hard", hardContainer);
    hardButton->setStyleSheet(buttonStyle);
    QLabel* hardDesc = new QLabel(
        "• Advanced vocabulary and idioms\n• Shorter time limit\n• 10 exercises", hardContainer);
    hardDesc->setStyleSheet(descStyle);
    hardDesc->setWordWrap(true);
    hardLayout->addWidget(hardButton);
    hardLayout->addWidget(hardDesc);
    mainLayout->addWidget(hardContainer);

    connect(easyButton, &QPushButton::clicked, [this, dialog]() {
        difficultyLevel_ = 1;
        totalExercises_ = 5;
        maxTime_ = 600;
        resetExercise();
        dialog->accept();
    });
    connect(mediumButton, &QPushButton::clicked, [this, dialog]() {
        difficultyLevel_ = 2;
        totalExercises_ = 7;
        maxTime_ = 420;
        resetExercise();
        dialog->accept();
    });
    connect(hardButton, &QPushButton::clicked, [this, dialog]() {
        difficultyLevel_ = 3;
        totalExercises_ = 10;
        maxTime_ = 300;
        resetExercise();
        dialog->accept();
    });

    dialog->exec();
    // resetExercise();
}

void MainWindow::resetExercise() {
    currentExercises_.clear();
    while (currentExercises_.size() < totalExercises_) {
        int randomIndex = std::rand() % 94;
        if (std::find(currentExercises_.begin(), currentExercises_.end(), randomIndex) ==
            currentExercises_.end()) {
            currentExercises_.push_back(randomIndex);
        }
    }
    progressBar_->setRange(0, totalExercises_);
    progressBar_->setValue(0);
    currentExercise_ = 0;
    switch (difficultyLevel_) {
        case 1:
            translationPrompt_->setText(
                "Translate the following sentence: " +
                easyTranslationExercise[currentExercises_[currentExercise_]]);
            break;
        case 2:
            translationPrompt_->setText(
                "Translate the following sentence: " +
                mediumTranslationExercise[currentExercises_[currentExercise_]]);
            break;
        case 3:
            translationPrompt_->setText(
                "Translate the following sentence: " +
                hardTranslationExercise[currentExercises_[currentExercise_]]);
            break;
    }

    setGrammarExercise();
    startExercise();
}

void MainWindow::showHelp() {
    QString helpText;
    if (stackedWidget_->currentWidget() == translationWidget_) {
        helpText =
            "Translation Exercise Help:\n"
            "• Translate the sentence into English\n"
            "• Translate Ideas, Not Just Words\n"
            "• Master Idioms and Set Phrases\n"
            "• Match Formality and Style\n"
            "• Break Down Complex Sentences\n"
            "• Check for Natural Flow\n"
            ;
    } else {
        helpText =
            "Grammar Exercise Help:\n"
            "• Focus on Sentence Structure\n"
            "• Master Function Words (虚词)\n"
            "• Understand Word Order Flexibility\n"
            "• Watch Out for Aspect and Modal Particles\n"
            "• Practice in Context, Not Isolation"
            ;
    }

    QMessageBox::information(this, "Help", helpText);
}

void MainWindow::updateTimer() {
    remainingTime_--;
    timerLabel_->setText(QString("Time: %1:%2")
                             .arg(remainingTime_ / 60, 2, 10, QChar('0'))
                             .arg(remainingTime_ % 60, 2, 10, QChar('0')));

    if (remainingTime_ <= 0) {
        endExercise();
        QMessageBox::warning(this, "Time's Up", "The time for this exercise has expired!");
    }
}

void MainWindow::startExercise() {
    currentExercise_ = 0;
    currentAttempts_ = 0;
    remainingTime_ = maxTime_;
    progressBar_->setValue(0);
    exerciseTimer_->start(1000);
}

void MainWindow::endExercise() {
    exerciseTimer_->stop();
    submitButton_->setEnabled(false);
    if (currentExercise_ == totalExercises_) {
        updateScore(10);
        showResultDialog(true);
    }
}

void MainWindow::checkAnswer() {
    bool correct = false;

    if (stackedWidget_->currentWidget() == translationWidget_) {
        QString userAnswer = translationInput_->toPlainText().trimmed();
        QString correctAnswer;
        switch (difficultyLevel_) {
            case 1:
                correctAnswer = easyTranslationExerciseAnswers[currentExercises_[currentExercise_]];
                break;
            case 2:
                correctAnswer =
                    mediumTranslationExerciseAnswers[currentExercises_[currentExercise_]];
                break;
            case 3:
                correctAnswer = hardTranslationExerciseAnswers[currentExercises_[currentExercise_]];
                break;
        }
        correct = userAnswer.compare(correctAnswer, Qt::CaseInsensitive) == 0;
    } else {
        int selectedId = grammarOptions_->checkedId();
        switch (difficultyLevel_) {
            case 1:
                correct =
                    (selectedId == easyGrammarExerciseAnswers[currentExercises_[currentExercise_]]);
                break;
            case 2:
                correct =
                    (selectedId ==
                     mediumGrammarExerciseAnswers[currentExercises_[currentExercise_]]);
                break;
            case 3:
                correct =
                    (selectedId == hardGrammarExerciseAnswers[currentExercises_[currentExercise_]]);
                break;
        }
    }

    if (correct) {
        currentExercise_++;
        progressBar_->setValue(currentExercise_);
        successSound_->play();
        if (currentExercise_ == totalExercises_) {
            endExercise();
        } else {
            nextExercise();
        }
    } else {
        currentAttempts_++;
        failedSound_->play();
        if (currentAttempts_ >= maxAttempts_) {
            endExercise();
            showResultDialog(false);
        } else {
            QMessageBox::warning(
                this, "Incorrect",
                QString("Try again! %1 attempts remaining").arg(maxAttempts_ - currentAttempts_));
        }
    }
}

void MainWindow::nextExercise() {
    if (stackedWidget_->currentWidget() == translationWidget_) {
        translationInput_->clear();
        switch (difficultyLevel_) {
            case 1:
                translationPrompt_->setText(
                    "Translate the following sentence: " +
                    easyTranslationExercise[currentExercises_[currentExercise_]]);
                break;
            case 2:
                translationPrompt_->setText(
                    "Translate the following sentence: " +
                    mediumTranslationExercise[currentExercises_[currentExercise_]]);
                break;
            case 3:
                translationPrompt_->setText(
                    "Translate the following sentence: " +
                    hardTranslationExercise[currentExercises_[currentExercise_]]);
                break;
        }
    } else {
        grammarOptions_->setExclusive(false);
        for (QAbstractButton* button : grammarOptions_->buttons()) {
            button->setChecked(false);
        }
        grammarOptions_->setExclusive(true);
        setGrammarExercise();
    }
}

void MainWindow::updateScore(int points) {
    currentScore_ += points;
    scoreLabel_->setText(QString("Score: %1").arg(currentScore_));
}

void MainWindow::showResultDialog(bool success) {
    QString message =
        success ? QString("Congratulations! You completed the exercise with a score of %1!")
                      .arg(currentScore_)
                : "Exercise ended. Better luck next time!";
    QMessageBox::information(this, "Exercise Complete", message);
}

// NOLINTEND