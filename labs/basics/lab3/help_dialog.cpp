#include "help_dialog.h"

HelpDialog::HelpDialog(QWidget *parent)
    : QDialog(parent)
    , titleLabel(nullptr)
    , helpContent(nullptr)
    , closeButton(nullptr)
{
    setupUI();
    applyTheme();
}

void HelpDialog::setupUI()
{
    setWindowTitle("Help - French Princess Learning");
    setModal(true);
    resize(600, 500);
    
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    
    // Title
    titleLabel = new QLabel("Help & Tips", this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #8B4789; margin: 10px;");
    mainLayout->addWidget(titleLabel);
    
    // Help content
    helpContent = new QTextEdit(this);
    helpContent->setReadOnly(true);
    helpContent->setHtml(
        "<h3 style='color: #8B4789;'>Welcome to French Princess Learning! </h3>"
        
        "<h4 style='color: #FF69B4;'>Translation Exercise:</h4>"
        "<p style='color: #8B4789;'>" + getTranslationHelp() + "</p>"
        
        "<h4 style='color: #FF69B4;'>Grammar Exercise:</h4>"
        "<p style='color: #8B4789;'>" + getGrammarHelp() + "</p>"
        
        "<h4 style='color: #FF69B4;'>General Tips:</h4>"
        "<ul style='color: #8B4789;'>"
        "<li>Take your time to read each question carefully</li>"
        "<li>Use the hints provided to learn grammar rules</li>"
        "<li>Practice regularly to improve your French</li>"
        "<li>Don't worry about mistakes - they're part of learning!</li>"
        "<li>Press 'H' key anytime to show this help window</li>"
        "</ul>"
        
        "<h4 style='color: #FF69B4;'>Scoring System:</h4>"
        "<p style='color: #8B4789;'>"
        "Complete all exercises correctly to earn points:<br>"
        "Translation: 10 points per correct answer × difficulty level<br>"
        "Grammar: 15 points per correct answer × difficulty level<br>"
        "You can make up to 3 mistakes per exercise."
        "</p>"
        
        "<h4 style='color: #FF69B4;'>Time Limit:</h4>"
        "<p style='color: #8B4789;'>"
        "Each exercise has a 5-minute time limit. Use your time wisely!"
        "</p>"
    );
    
    helpContent->setStyleSheet(
        "QTextEdit {"
        "background: #FFF0F5;"
        "border: 2px solid #DDA0DD;"
        "border-radius: 10px;"
        "padding: 15px;"
        "font-size: 12px;"
        "color: #8B4789;"
        "}"
    );
    
    mainLayout->addWidget(helpContent);
    
    // Close button
    QHBoxLayout* buttonLayout = new QHBoxLayout();
    buttonLayout->addStretch();
    
    closeButton = new QPushButton("Close", this);
    closeButton->setStyleSheet(
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
    );
    
    connect(closeButton, &QPushButton::clicked, this, &QDialog::accept);
    buttonLayout->addWidget(closeButton);
    
    mainLayout->addLayout(buttonLayout);
}

void HelpDialog::applyTheme()
{
    // Theme is applied through stylesheets in setupUI
}

QString HelpDialog::getTranslationHelp()
{
    return "In translation exercises, you'll see French phrases that need to be translated to Russian. "
           "Pay attention to:<br>"
           "1. Basic greetings and expressions<br>"
           "2. Common verbs and their conjugations<br>"
           "3. Prepositions and articles<br>"
           "4. Word order in French sentences<br>"
           "5. Context and meaning of phrases";
}

QString HelpDialog::getGrammarHelp()
{
    return "Grammar exercises test your knowledge of French grammar rules. "
           "Focus on:<br>"
           "1. Verb conjugations (present, past, future tenses)<br>"
           "2. Subject-verb agreement<br>"
           "3. Articles (definite and indefinite)<br>"
           "4. Adjective agreement<br>"
           "5. Pronouns and their correct usage<br>"
           "6. Reflexive verbs<br>"
           "7. Conditional and subjunctive moods (advanced)";
}
