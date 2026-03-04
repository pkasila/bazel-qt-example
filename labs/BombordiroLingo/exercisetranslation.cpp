#include "exercisetranslation.h"
#include "constants.h"

#include <QMessageBox>

ExerciseTranslation::ExerciseTranslation(QWidget *parent) : QWidget(parent), currentQuestion(0), attemptsLeft(Constants::MAX_ATTEMPTS)
{
    layout = new QVBoxLayout(this);

    questionLabel = new QLabel(this);
    questionLabel->setWordWrap(true);
    questionLabel->setAlignment(Qt::AlignCenter);

    answerEdit = new QTextEdit(this);
    answerEdit->setPlaceholderText("Enter your translation here...");
    answerEdit->setMaximumHeight(100);

    layout->addWidget(questionLabel);
    layout->addWidget(answerEdit);

    setLayout(layout);
}

void ExerciseTranslation::startExercise(const QString &difficulty)
{
    generateQuestions(difficulty);
    currentQuestion = 0;
    attemptsLeft = Constants::MAX_ATTEMPTS;
    showNextQuestion();
}

bool ExerciseTranslation::checkAnswer()
{
    QString userAnswer = answerEdit->toPlainText().trimmed();
    QString correctAnswer = answers[currentQuestion].trimmed();

    if (userAnswer.compare(correctAnswer, Qt::CaseInsensitive) == 0) {
        currentQuestion++;
        if (currentQuestion < questions.size()) {
            showNextQuestion();
        }
        return true;
    } else {
        attemptsLeft--;
        if (attemptsLeft <= 0) {
            QMessageBox::warning(this, "Attempts exceeded", "You've used all your attempts. Exercise will be reset.");
            startExercise("");
            return false;
        }
        return false;
    }
}

void ExerciseTranslation::generateQuestions(const QString &difficulty)
{
    questions.clear();
    answers.clear();

    if (difficulty == Constants::EASY_DIFFICULTY) {
        questions << "Hello" << "Goodbye" << "Thank you" << "Please" << "Yes"
                  << "No" << "My name is" << "I don't understand" << "How much?" << "Where?";
        answers << "Привет" << "До свидания" << "Спасибо" << "Пожалуйста" << "Да"
                << "Нет" << "Меня зовут" << "Я не понимаю" << "Сколько?" << "Где?";
    } else if (difficulty == Constants::MEDIUM_DIFFICULTY) {
        questions << "I would like to order coffee" << "Where is the nearest bank?"
                  << "What time is it?" << "How do I get to the museum?" << "I don't speak Russian well"
                  << "Could you help me?" << "What is your name?" << "I'm lost" << "How much does this cost?" << "When does the train leave?";
        answers << "Я хотел бы заказать кофе" << "Где ближайший банк?"
                << "Который час?" << "Как пройти к музею?" << "Я плохо говорю по-русски"
                << "Не могли бы вы мне помочь?" << "Как вас зовут?" << "Я заблудился" << "Сколько это стоит?" << "Когда отправляется поезд?";
    } else {
        questions << "Despite the bad weather, we decided to go for a walk in the park"
                  << "The scientific conference brought together researchers from all over the world"
                  << "Modern technologies have significantly changed our way of life"
                  << "The novel, which was published last year, became a bestseller"
                  << "Economic stability is essential for the country's development"
                  << "Environmental protection should be a priority for every government"
                  << "The artist's works are exhibited in the most prestigious galleries"
                  << "Historical events often influence contemporary political decisions"
                  << "Psychological research shows that human behavior is complex"
                  << "International cooperation is necessary to solve global problems";
        answers << "Несмотря на плохую погоду, мы решили пойти гулять в парк"
                << "Научная конференция собрала исследователей со всего мира"
                << "Современные технологии значительно изменили наш образ жизни"
                << "Роман, который был опубликован в прошлом году, стал бестселлером"
                << "Экономическая стабильность важна для развития страны"
                << "Защита окружающей среды должна быть приоритетом для каждого правительства"
                << "Работы художника выставлены в самых престижных галереях"
                << "Исторические события часто влияют на современные политические решения"
                << "Психологические исследования показывают, что человеческое поведение сложно"
                << "Международное сотрудничество необходимо для решения глобальных проблем";
    }
}

void ExerciseTranslation::showNextQuestion()
{
    if (currentQuestion < questions.size()) {
        questionLabel->setText(questions[currentQuestion]);
        answerEdit->clear();
    }
}
