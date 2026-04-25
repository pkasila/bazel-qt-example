#include "translation_widget.h"

TranslationWidget::TranslationWidget(QWidget *parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , questionLabel(nullptr)
    , questionDisplay(nullptr)
    , answerInput(nullptr)
    , submitButton(nullptr)
    , nextButton(nullptr)
    , feedbackLabel(nullptr)
    , progressLabel(nullptr)
    , questionGroupBox(nullptr)
    , currentQuestionIndex(0)
    , correctAnswers(0)
    , totalQuestions(5)
    , mistakesCount(0)
    , maxMistakes(3)
    , difficulty(1)
    , exerciseStarted(false)
{
    setupUI();
    applyTheme();
}

void TranslationWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    titleLabel = new QLabel("Translation Exercise", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #8B4789; margin: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Progress label
    progressLabel = new QLabel("Question 1 of 5", this);
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setStyleSheet("font-size: 14px; color: #FF69B4; margin: 5px;");
    mainLayout->addWidget(progressLabel);
    
    // Question group
    questionGroupBox = new QGroupBox("Translate to Russian:", this);
    questionGroupBox->setStyleSheet(
        "QGroupBox {"
        "font-weight: bold;"
        "color: #8B4789;"
        "border: 2px solid #DDA0DD;"
        "border-radius: 10px;"
        "margin-top: 10px;"
        "padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "subcontrol-origin: margin;"
        "left: 10px;"
        "padding: 0 5px 0 5px;"
        "}"
    );
    
    QVBoxLayout* questionLayout = new QVBoxLayout(questionGroupBox);
    
    questionDisplay = new QTextEdit(this);
    questionDisplay->setReadOnly(true);
    questionDisplay->setMaximumHeight(100);
    questionDisplay->setStyleSheet(
        "QTextEdit {"
        "background: #FFF0F5;"
        "border: 1px solid #DDA0DD;"
        "border-radius: 8px;"
        "padding: 10px;"
        "font-size: 16px;"
        "color: #8B4789;"
        "}"
    );
    questionLayout->addWidget(questionDisplay);
    
    mainLayout->addWidget(questionGroupBox);
    
    // Answer input
    QLabel* answerLabel = new QLabel("Your answer:", this);
    answerLabel->setStyleSheet("font-size: 14px; color: #8B4789; margin: 5px;");
    mainLayout->addWidget(answerLabel);
    
    answerInput = new QLineEdit(this);
    answerInput->setStyleSheet(
        "QLineEdit {"
        "background: #FFF0F5;"
        "border: 2px solid #DDA0DD;"
        "border-radius: 8px;"
        "padding: 8px;"
        "font-size: 14px;"
        "color: #8B4789;"
        "}"
        "QLineEdit:focus {"
        "border: 2px solid #FF69B4;"
        "}"
    );
    connect(answerInput, &QLineEdit::returnPressed, this, &TranslationWidget::checkAnswer);
    mainLayout->addWidget(answerInput);
    
    // Feedback label
    feedbackLabel = new QLabel("", this);
    feedbackLabel->setAlignment(Qt::AlignCenter);
    feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px;");
    mainLayout->addWidget(feedbackLabel);
    
    // Buttons
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    
    submitButton = new QPushButton("Submit", this);
    submitButton->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #FF69B4, stop:1 #FF1493);"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px 20px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #FF1493, stop:1 #C71585);"
        "}"
        "QPushButton:disabled {"
        "background: #DDA0DD;"
        "color: #8B4789;"
        "}"
    );
    connect(submitButton, &QPushButton::clicked, this, &TranslationWidget::checkAnswer);
    buttonLayout->addWidget(submitButton);
    
    nextButton = new QPushButton("Next Question", this);
    nextButton->setVisible(false);
    nextButton->setStyleSheet(
        "QPushButton {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #DA70D6, stop:1 #8B4789);"
        "color: white;"
        "border: none;"
        "border-radius: 10px;"
        "padding: 10px 20px;"
        "font-size: 14px;"
        "font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "background: qlineargradient(x1:0, y1:0, x2:0, y2:1, "
        "stop:0 #8B4789, stop:1 #663366);"
        "}"
    );
    connect(nextButton, &QPushButton::clicked, this, &TranslationWidget::showNextQuestion);
    buttonLayout->addWidget(nextButton);
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // Initially disabled until exercise starts
    questionGroupBox->setEnabled(false);
    answerInput->setEnabled(false);
    submitButton->setEnabled(false);
}

void TranslationWidget::applyTheme()
{
    // Theme is applied through stylesheets in setupUI
}

void TranslationWidget::startExercise(int difficulty)
{
    this->difficulty = difficulty;
    currentQuestionIndex = 0;
    correctAnswers = 0;
    mistakesCount = 0;
    exerciseStarted = true;
    
    loadQuestions(difficulty);
    
    // Enable UI
    questionGroupBox->setEnabled(true);
    answerInput->setEnabled(true);
    submitButton->setEnabled(true);
    
    showQuestion(0);
}

void TranslationWidget::loadQuestions(int difficulty)
{
    questions.clear();
    
    if (difficulty == 1) {
        // Easy questions - basic phrases
        questions.append({"Bonjour", "Hello", "Common greeting"});
        questions.append({"Merci", "Thank you", "Polite expression"});
        questions.append({"Au revoir", "Goodbye", "Farewell"});
        questions.append({"Oui", "Yes", "Affirmative"});
        questions.append({"Non", "No", "Negative"});
        questions.append({"S'il vous plaît", "Please", "Polite request"});
        questions.append({"Excusez-moi", "Excuse me", "Getting attention"});
        questions.append({"Je m'appelle...", "My name is...", "Introduction"});
        questions.append({"Comment allez-vous?", "How are you?", "Greeting"});
        questions.append({"J'adore", "I love", "Strong positive feeling"});
    } else if (difficulty == 2) {
        // Medium questions - more complex phrases
        questions.append({"Je voudrais un café", "I would like a coffee", "Ordering"});
        questions.append({"Où est la gare?", "Where is the station?", "Asking directions"});
        questions.append({"Combien ça coûte?", "How much does it cost?", "Shopping"});
        questions.append({"Je ne comprends pas", "I don't understand", "Communication"});
        questions.append({"Parlez-vous anglais?", "Do you speak English?", "Language inquiry"});
        questions.append({"Je suis en vacances", "I'm on vacation", "Travel context"});
        questions.append({"Quelle heure est-il?", "What time is it?", "Asking time"});
        questions.append({"J'ai faim", "I'm hungry", "Basic need"});
        questions.append({"Le temps est beau", "The weather is nice", "Weather"});
        questions.append({"Je cherche l'hôtel", "I'm looking for the hotel", "Navigation"});
    } else {
        // Hard questions - advanced phrases
        questions.append({"J'aimerais réserver une table", "I would like to reserve a table", "Restaurant"});
        questions.append({"Pourriez-vous m'aider?", "Could you help me?", "Polite request"});
        questions.append({"Je suis désolé du retard", "I'm sorry for being late", "Apology"});
        questions.append({"C'est magnifique!", "It's magnificent!", "Compliment"});
        questions.append({"Je voudrais acheter ce livre", "I would like to buy this book", "Shopping"});
        questions.append({"Quel est le prix de cet article?", "What is the price of this item?", "Shopping"});
        questions.append({"Je me sens mieux aujourd'hui", "I feel better today", "Health"});
        questions.append({"Pourriez-vous répéter s'il vous plaît?", "Could you repeat please?", "Clarification"});
        questions.append({"Je dois prendre le métro", "I must take the subway", "Transportation"});
        questions.append({"C'est une belle journée", "It's a beautiful day", "Weather compliment"});
    }
    
    // Shuffle and select totalQuestions
    std::random_shuffle(questions.begin(), questions.end());
    
    if (questions.size() > totalQuestions) {
        questions = questions.mid(0, totalQuestions);
    }
    
    totalQuestions = questions.size();
}

void TranslationWidget::showQuestion(int index)
{
    if (index >= questions.size()) {
        // Exercise completed
        bool success = (correctAnswers >= totalQuestions - maxMistakes);
        int score = success ? (correctAnswers * 10 * difficulty) : 0;
        
        emit exerciseCompleted(success, score);
        return;
    }
    
    currentQuestionIndex = index;
    const Question& question = questions[index];
    
    questionDisplay->setPlainText(question.french);
    answerInput->clear();
    answerInput->setFocus();
    
    feedbackLabel->setText("");
    feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px;");
    
    progressLabel->setText(QString("Question %1 of %2").arg(index + 1).arg(totalQuestions));
    
    emit progressUpdated(index + 1, totalQuestions);
    
    submitButton->setVisible(true);
    nextButton->setVisible(false);
    answerInput->setEnabled(true);
}

void TranslationWidget::checkAnswer()
{
    if (!exerciseStarted || currentQuestionIndex >= questions.size()) {
        return;
    }
    
    QString userAnswer = answerInput->text().trimmed().toLower();
    QString correctAnswer = questions[currentQuestionIndex].russian.toLower();
    
    // Simple matching - could be enhanced with fuzzy matching
    bool isCorrect = (userAnswer == correctAnswer);
    
    if (isCorrect) {
        correctAnswers++;
        feedbackLabel->setText("Correct! Well done! " + questions[currentQuestionIndex].hint);
        feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #32CD32; font-weight: bold;");
    } else {
        mistakesCount++;
        feedbackLabel->setText(QString("Incorrect. The correct answer is: %1").arg(questions[currentQuestionIndex].russian));
        feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #DC143C; font-weight: bold;");
        
        if (mistakesCount >= maxMistakes) {
            emit exerciseFailed("Too many mistakes! You made 3 incorrect answers.");
            return;
        }
    }
    
    answerInput->setEnabled(false);
    submitButton->setVisible(false);
    nextButton->setVisible(true);
    nextButton->setFocus();
}

void TranslationWidget::showNextQuestion()
{
    showQuestion(currentQuestionIndex + 1);
}
