#include "grammar_widget.h"

GrammarWidget::GrammarWidget(QWidget *parent)
    : QWidget(parent)
    , titleLabel(nullptr)
    , questionLabel(nullptr)
    , questionDisplay(nullptr)
    , answerGroup(nullptr)
    , submitButton(nullptr)
    , nextButton(nullptr)
    , feedbackLabel(nullptr)
    , progressLabel(nullptr)
    , questionGroupBox(nullptr)
    , answersGroupBox(nullptr)
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

void GrammarWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    titleLabel = new QLabel("Grammar Exercise", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 22px; font-weight: bold; color: #8B4789; margin: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Progress label
    progressLabel = new QLabel("Question 1 of 5", this);
    progressLabel->setAlignment(Qt::AlignCenter);
    progressLabel->setStyleSheet("font-size: 14px; color: #FF69B4; margin: 5px;");
    mainLayout->addWidget(progressLabel);
    
    // Question group
    questionGroupBox = new QGroupBox("Choose the correct option:", this);
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
    questionDisplay->setMaximumHeight(80);
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
    
    // Answers group
    answersGroupBox = new QGroupBox("Options:", this);
    answersGroupBox->setStyleSheet(
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
    
    QVBoxLayout* answersLayout = new QVBoxLayout(answersGroupBox);
    
    answerGroup = new QButtonGroup(this);
    
    // Create 4 radio buttons for options
    for (int i = 0; i < 4; ++i) {
        QRadioButton* radio = new QRadioButton(this);
        radio->setStyleSheet(
            "QRadioButton {"
            "color: #8B4789;"
            "font-size: 14px;"
            "padding: 5px;"
            "}"
            "QRadioButton::indicator {"
            "width: 18px;"
            "height: 18px;"
            "}"
            "QRadioButton::indicator::unchecked {"
            "border: 2px solid #DDA0DD;"
            "border-radius: 9px;"
            "background: #FFF0F5;"
            "}"
            "QRadioButton::indicator::checked {"
            "border: 2px solid #FF69B4;"
            "border-radius: 9px;"
            "background: #FF69B4;"
            "}"
        );
        answerRadioButtons.append(radio);
        answerGroup->addButton(radio, i);
        answersLayout->addWidget(radio);
    }
    
    mainLayout->addWidget(answersGroupBox);
    
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
    connect(submitButton, &QPushButton::clicked, this, &GrammarWidget::checkAnswer);
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
    connect(nextButton, &QPushButton::clicked, this, &GrammarWidget::showNextQuestion);
    buttonLayout->addWidget(nextButton);
    
    mainLayout->addLayout(buttonLayout);
    mainLayout->addStretch();
    
    // Initially disabled until exercise starts
    questionGroupBox->setEnabled(false);
    answersGroupBox->setEnabled(false);
    submitButton->setEnabled(false);
}

void GrammarWidget::applyTheme()
{
    // Theme is applied through stylesheets in setupUI
}

void GrammarWidget::startExercise(int difficulty)
{
    this->difficulty = difficulty;
    currentQuestionIndex = 0;
    correctAnswers = 0;
    mistakesCount = 0;
    exerciseStarted = true;
    
    loadQuestions(difficulty);
    
    // Enable UI
    questionGroupBox->setEnabled(true);
    answersGroupBox->setEnabled(true);
    submitButton->setEnabled(true);
    
    showQuestion(0);
}

void GrammarWidget::loadQuestions(int difficulty)
{
    questions.clear();
    
    if (difficulty == 1) {
        // Easy questions - basic verb conjugation
        questions.append({
            "Je (aller) à l'école.",
            "vais",
            {"vais", "va", "vont", "allez"},
            "Present tense of 'aller' for 'je'"
        });
        questions.append({
            "Elle (être) française.",
            "est",
            {"suis", "es", "est", "sommes"},
            "Present tense of 'être' for 'elle'"
        });
        questions.append({
            "Nous (avoir) un chat.",
            "avons",
            {"ai", "as", "a", "avons"},
            "Present tense of 'avoir' for 'nous'"
        });
        questions.append({
            "Tu (faire) tes devoirs.",
            "fais",
            "fais"
        });
        questions.append({
            "Ils (parler) français.",
            "parlent",
            {"parle", "parles", "parle", "parlent"},
            "Present tense of 'parler' for 'ils'"
        });
        questions.append({
            "Je (manger) une pomme.",
            "mange",
            {"mange", "manges", "mange", "mangent"},
            "Present tense of 'manger' for 'je'"
        });
        questions.append({
            "Vous (vouloir) du café?",
            "voulez",
            {"veux", "veux", "veut", "voulez"},
            "Present tense of 'vouloir' for 'vous'"
        });
        questions.append({
            "Elle (dire) bonjour.",
            "dit",
            {"dis", "dis", "dit", "disons"},
            "Present tense of 'dire' for 'elle'"
        });
    } else if (difficulty == 2) {
        // Medium questions - past tense and articles
        questions.append({
            "Hier, je (aller) au cinéma.",
            "suis allé",
            {"suis allé", "suis allée", "ai allé", "a allé"},
            "Passé composé of 'aller' for masculine 'je'"
        });
        questions.append({
            "Elle (manger) une pizza.",
            "a mangé",
            {"a mangé", "a mangée", "est mangé", "est mangée"},
            "Passé composé of 'manger' for 'elle'"
        });
        questions.append({
            "J'ai vu ___ film intéressant.",
            "un",
            {"le", "la", "un", "une"},
            "Indefinite article for masculine singular"
        });
        questions.append({
            "Elle aime ___ chocolat.",
            "le",
            {"le", "la", "les", "des"},
            "Definite article for masculine singular"
        });
        questions.append({
            "Nous (être) contents.",
            "étions",
            {"sommes", "étions", "serons", "serons"},
            "Imparfait of 'être' for 'nous'"
        });
        questions.append({
            "Demain, je (partir) à Paris.",
            "partirai",
            {"pars", "partirai", "partais", "partis"},
            "Futur simple of 'partir' for 'je'"
        });
        questions.append({
            "Il (faire) beau hier.",
            "faisait",
            {"fait", "faisait", "fera", "a fait"},
            "Imparfait of 'faire' for 'il'"
        });
        questions.append({
            "J'ai perdu ___ clés.",
            "mes",
            {"mon", "ma", "mes", "mien"},
            "Possessive adjective for plural"
        });
    } else {
        // Hard questions - complex grammar
        questions.append({
            "Si j'avais de l'argent, je (acheter) une voiture.",
            "achèterais",
            {"achète", "achèterais", "achèterai", "achetais"},
            "Conditionnel présent of 'acheter' for 'je'"
        });
        questions.append({
            "Après qu'il (finir) son travail, il partira.",
            "ait fini",
            {"a fini", "a fini", "ait fini", "avait fini"},
            "Subjonctif passé of 'finir'"
        });
        questions.append({
            "Il faut que tu (venir) rapidement.",
            "viennes",
            {"viens", "viennes", "venais", "viendras"},
            "Subjonctif présent of 'venir' for 'tu'"
        });
        questions.append({
            "Le livre que je (lire) est intéressant.",
            "lis",
            {"lis", "lis", "lit", "lisons"},
            "Present tense of 'lire' for 'je'"
        });
        questions.append({
            "Elle se (lever) à 7 heures chaque matin.",
            "lève",
            {"lève", "lèves", "leve", "levent"},
            "Reflexive verb 'se lever' for 'elle'"
        });
        questions.append({
            "Nous nous (souvenir) de nos vacances.",
            "souvenons",
            {"souvenons", "souvenons", "souvient", "souvenez"},
            "Reflexive verb 'se souvenir' for 'nous'"
        });
        questions.append({
            "Avant de (partir), il doit dire au revoir.",
            "partir",
            {"partir", "partir", "part", "parts"},
            "Infinitive after 'avant de'"
        });
        questions.append({
            "C'est ___ plus belle ville que j'ai jamais vue.",
            "la",
            {"le", "la", "les", "une"},
            "Definite article in superlative"
        });
    }
    
    // Shuffle and select totalQuestions
    std::random_shuffle(questions.begin(), questions.end());
    
    if (questions.size() > totalQuestions) {
        questions = questions.mid(0, totalQuestions);
    }
    
    totalQuestions = questions.size();
}

void GrammarWidget::showQuestion(int index)
{
    if (index >= questions.size()) {
        // Exercise completed
        bool success = (correctAnswers >= totalQuestions - maxMistakes);
        int score = success ? (correctAnswers * 15 * difficulty) : 0;
        
        emit exerciseCompleted(success, score);
        return;
    }
    
    currentQuestionIndex = index;
    const Question& question = questions[index];
    
    questionDisplay->setPlainText(question.sentence);
    
    // Set up answer options
    for (int i = 0; i < answerRadioButtons.size(); ++i) {
        if (i < question.options.size()) {
            answerRadioButtons[i]->setText(question.options[i]);
            answerRadioButtons[i]->setVisible(true);
        } else {
            answerRadioButtons[i]->setVisible(false);
        }
        answerRadioButtons[i]->setAutoExclusive(false);
        answerRadioButtons[i]->setChecked(false);
        answerRadioButtons[i]->setAutoExclusive(true);
    }
    
    feedbackLabel->setText("");
    feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px;");
    
    progressLabel->setText(QString("Question %1 of %2").arg(index + 1).arg(totalQuestions));
    
    emit progressUpdated(index + 1, totalQuestions);
    
    submitButton->setVisible(true);
    nextButton->setVisible(false);
    answersGroupBox->setEnabled(true);
}

void GrammarWidget::checkAnswer()
{
    if (!exerciseStarted || currentQuestionIndex >= questions.size()) {
        return;
    }
    
    int selectedAnswer = answerGroup->checkedId();
    if (selectedAnswer == -1) {
        feedbackLabel->setText("Please select an answer!");
        feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #FF8C00; font-weight: bold;");
        return;
    }
    
    QString userAnswer = answerRadioButtons[selectedAnswer]->text();
    QString correctAnswer = questions[currentQuestionIndex].correctAnswer;
    
    bool isCorrect = (userAnswer == correctAnswer);
    
    if (isCorrect) {
        correctAnswers++;
        feedbackLabel->setText("Correct! Well done! " + questions[currentQuestionIndex].hint);
        feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #32CD32; font-weight: bold;");
    } else {
        mistakesCount++;
        feedbackLabel->setText(QString("Incorrect. The correct answer is: %1").arg(correctAnswer));
        feedbackLabel->setStyleSheet("font-size: 14px; margin: 10px; color: #DC143C; font-weight: bold;");
        
        if (mistakesCount >= maxMistakes) {
            emit exerciseFailed("Too many mistakes! You made 3 incorrect answers.");
            return;
        }
    }
    
    answersGroupBox->setEnabled(false);
    submitButton->setVisible(false);
    nextButton->setVisible(true);
    nextButton->setFocus();
}

void GrammarWidget::showNextQuestion()
{
    showQuestion(currentQuestionIndex + 1);
}
