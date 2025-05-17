#include "translation.h"
#include <QVBoxLayout>
#include <QButtonGroup>
#include <QMessageBox>
#include <QDebug>

void Translation::SetUI(){
    vertical = new QVBoxLayout(this);
    auto title = new QLabel("This is translation page", this);
    title->setStyleSheet("font-size: 28pt; font-weight: bold; color: white; margin-bottom: 10px;text-align: center;");
    title->setAlignment(Qt::AlignCenter);
    startButton = new QPushButton("Start Quiz", this);
    vertical->addWidget(title);
    vertical->addWidget(startButton);
    connect(startButton, &QPushButton::clicked, this, &Translation::StartGame);
    setLayout(vertical);
}

Translation::Translation(QWidget* parent) : QWidget(parent){
    SetUI();
    buttons = new QButtonGroup(this);
}

Translation::~Translation(){

}

void Translation::NextQuestion(){
    if(questionNumber >= questions.size()){
        emit EndGame();
        RenewQuiz();
        return;
    }
    auto current = questions[questionNumber];
    questionText->setText(current.first);
    if(mode == Mode::TRANSLATION){
            for (int i = 0; i < 4; ++i) {
            QAbstractButton* btn = buttons->button(i);
            if (!btn) {
                qWarning() << "Button" << i << "is null!";
                continue;
            }

            if (i >= current.second.size()) {
                qWarning() << "No text available for button" << i;
                btn->setText("");
            } else {
                btn->setText(current.second[i]);
            }
        }
    }

}

void Translation::CheckAnswer(){
    qDebug() << "number: " << questionNumber;
    if(mode == Mode::TRANSLATION){
        int id = buttons->checkedId();
        if(id == answers[questionNumber]){
            QMessageBox::information(this, "You are right", "Let's go to the next question");
            ++right_answers;
        } else{
            QMessageBox::warning(this, "This isn't right answer", "Let's go to the next question");
        }
    } else if(mode == Mode::WRITING){
        qDebug() << insertText->toPlainText();
        qDebug() << questions[questionNumber].second[answers[questionNumber]];
        if(insertText->toPlainText().toLower() == questions[questionNumber].second[answers[questionNumber]].toLower()){
            QMessageBox::information(this, "You are right", "Let's go to the next question");
            ++right_answers;
        } else{
            QMessageBox::warning(this, "This isn't right answer", "Let's go to the next question");
        }
        insertText->clear();
    }
    ++questionNumber;
    NextQuestion();
}


void Translation::StartGame(){
    right_answers = 0;
    startButton->setVisible(false);
    auto current = questions[questionNumber];
    questionText = new QLabel(current.first, this);
    questionText->setStyleSheet("font-size: 28pt; font-weight: bold; color: white; margin-bottom: 8px;text-align: center;");
    questionText->setAlignment(Qt::AlignCenter);
    vertical->addWidget(questionText);
    if(mode == Mode::TRANSLATION){
        for(int i = 0; i != 4; ++i){
            auto curr_btn = new QRadioButton(current.second[i], this);
            curr_btn->setStyleSheet(
                "QRadioButton {"
                "    border: 2px solid #3498db;"
                "    border-radius: 10px;"
                "    padding: 8px;"
                "    background: #f8f9fa;"
                "    color: #2c3e50;"
                "}"
                "QRadioButton::indicator {"
                "    width: 16px;"
                "    height: 16px;"
                "}"
                "QRadioButton:hover {"
                "    border-color: #2980b9;"
                "}"
            );
            buttons->addButton(curr_btn, i);
            vertical->addWidget(curr_btn);
        }
    } else if(mode == Mode::WRITING){
        insertText = new QTextEdit(this);
        insertText->setStyleSheet(R"(
        QTextEdit {
                border: 2px solid #ccc !important;
                background-color: white;
                color: black;
                border-radius: 4px;
                padding: 8px;
            }
            QTextEdit:focus {
                border: 2px solid #4285F4 !important;
            }
            QTextEdit:hover {
                border: 2px solid blue !important;
            }
    )");
        insertText->setMaximumHeight(150);
        vertical->addWidget(insertText);
    }
    checkButton = new QPushButton("Check answer", this);
    vertical->addWidget(checkButton);
    connect(checkButton, &QPushButton::clicked, this, &Translation::CheckAnswer);
}

int Translation::GetRightAnswers(){
    return right_answers;
}

int Translation::GetQuestionsQuantity(){
    return questions.size();
}

void Translation::RenewQuiz(){
    right_answers = 0;
    questionNumber = 0;
    if(mode == Mode::TRANSLATION){
        qDeleteAll(buttons->buttons());
    } else if(mode == Mode::WRITING){
        insertText->deleteLater();
    }
    startButton->setVisible(true);
    checkButton->deleteLater();
    questionText->deleteLater();
}

void Translation::SetMode(Mode val){
    mode = val;
}
