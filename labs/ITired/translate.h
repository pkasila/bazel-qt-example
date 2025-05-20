#ifndef TRANSLATE_H
#define TRANSLATE_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QWidget>
#include <QTextEdit>
#include <QLabel>
#include <QProgressBar>
#include <QMenuBar>
#include <QTimer>
#include <QMediaPlayer>
#include <QSoundEffect>

struct QuestionTranslate {
    QString que;
    QString correctAns;
    QString hint;
};

class Translate : public QWidget
{
    Q_OBJECT
public:
    explicit Translate(QWidget *parent = nullptr);
    int advInd = 0;
    int intInd = 0;
    int begInd = 0;
    void reset();

    void setupDiffculty(int ind);

    void update();

    void startExercise() { emit start(); qDebug() << "Start";    }

    void endExercise() { emit end(); }

    int getScore() { return localScore; }

    void nullScore() {
        localScore = 0;
    }
signals:
    void start();
    void end();

private slots:
    //void setupQuestion(int index);
    //void nextQuestion(QString& que);
    void checkans();
private:

    void showDialog(QString str);

    QLabel* attemptsL;
    QLabel* timeL;
    QLabel* scoreL;
    QPushButton* helpButton;
    QLabel* question;
    QTextEdit* textEdit;
    QProgressBar* progress;
    QPushButton* submit;

    QSoundEffect* correctAns;
    QSoundEffect* notCorrectAns;
    QSoundEffect* levelComplete;
    QSoundEffect* lost;
    QSoundEffect* click;

    int seconds = 60;
    int attempts = 6;

    int numTasks = 10;

    int difficulty = 0;
    int localScore = 0;

    std::vector<QuestionTranslate> questionsInfoBeginer = {
                                                    {"Она идёт", "She goes", "Используйте 'she' и правильную форму глагола 'go'"},
                                                    {"Я читаю книгу", "I read a book", "Используйте 'I' и правильную форму глагола 'read'"},
                                                    {"Они работают", "They work", "Используйте 'they' и правильную форму глагола 'work'"},
                                                    {"Он спит", "He sleeps", "Используйте 'he' и правильную форму глагола 'sleep'"},
                                                    {"Мы живём здесь", "We live here", "Используйте 'we' и правильную форму глагола 'live'"},
                                                    {"Ты играешь", "You play", "Используйте 'you' и правильную форму глагола 'play'"},
                                                    {"Солнце светит", "The sun shines", "Используйте 'the sun' и правильную форму глагола 'shine'"},
                                                    {"Они слушают музыку", "They listen to music", "Используйте 'they' и правильную форму глагола 'listen'"},
                                                    {"Я учу английский", "I learn English", "Используйте 'I' и правильную форму глагола 'learn'"},
                                                    {"Она смотрит фильм", "She watches a movie", "Используйте 'she' и правильную форму глагола 'watch'"},
                                                    {"Ты ешь яблоко", "You eat an apple", "Используйте 'you' и правильную форму глагола 'eat'"},
                                                    {"Мы гуляем в парке", "We walk in the park", "Используйте 'we' и правильную форму глагола 'walk'"},
                                                    {"Он играет на гитаре", "He plays the guitar", "Используйте 'he' и правильную форму глагола 'play'"},
                                                    {"Они бегают", "They run", "Используйте 'they' и правильную форму глагола 'run'"},
                                                    {"Я пишу письмо", "I write a letter", "Используйте 'I' и правильную форму глагола 'write'"},
                                                    {"Она готовит ужин", "She cooks dinner", "Используйте 'she' и правильную форму глагола 'cook'"},
                                                    {"Ты смотришь телевизор", "You watch TV", "Используйте 'you' и правильную форму глагола 'watch'"},
                                                    {"Мы помогаем другу", "We help a friend", "Используйте 'we' и правильную форму глагола 'help'"},
                                                    {"Они пьют чай", "They drink tea", "Используйте 'they' и правильную форму глагола 'drink'"},
                                                    {"Я сплю ночью", "I sleep at night", "Используйте 'I' и правильную форму глагола 'sleep'"},
                                                    {"Она пишет книгу", "She writes a book", "Используйте 'she' и правильную форму глагола 'write'"},
                                                    {"Ты учишься в школе", "You study at school", "Используйте 'you' и правильную форму глагола 'study'"},
                                                    {"Мы идём домой", "We go home", "Используйте 'we' и правильную форму глагола 'go'"},
                                                    {"Он читает газету", "He reads a newspaper", "Используйте 'he' и правильную форму глагола 'read'"},
                                                    {"Они разговаривают", "They talk", "Используйте 'they' и правильную форму глагола 'talk'"},
                                                    {"Я пою песню", "I sing a song", "Используйте 'I' и правильную форму глагола 'sing'"},
                                                    {"Она улыбается", "She smiles", "Используйте 'she' и правильную форму глагола 'smile'"},
                                                    {"Ты рисуешь картину", "You draw a picture", "Используйте 'you' и правильную форму глагола 'draw'"},
                                                    {"Мы смотрим фильм", "We watch a movie", "Используйте 'we' и правильную форму глагола 'watch'"},
                                                    {"Он играет в футбол", "He plays football", "Используйте 'he' и правильную форму глагола 'play'"},
                                                    {"Они готовят завтрак", "They make breakfast", "Используйте 'they' и правильную форму глагола 'make'"},
                                                    {"Я слушаю музыку", "I listen to music", "Используйте 'I' и правильную форму глагола 'listen'"},
                                                    {"Она работает в офисе", "She works in an office", "Используйте 'she' и правильную форму глагола 'work'"},
                                                    {"Ты читаешь журнал", "You read a magazine", "Используйте 'you' и правильную форму глагола 'read'"},
                                                    {"Мы отдыхаем", "We rest", "Используйте 'we' и правильную форму глагола 'rest'"},
                                                    {"Он плавает в бассейне", "He swims in the pool", "Используйте 'he' и правильную форму глагола 'swim'"},
                                                    {"Они танцуют", "They dance", "Используйте 'they' и правильную форму глагола 'dance'"},
                                                    {"Я пишу стихотворение", "I write a poem", "Используйте 'I' и правильную форму глагола 'write'"},
                                                    {"Она играет в шахматы", "She plays chess", "Используйте 'she' и правильную форму глагола 'play'"},
                                                    {"Ты идёшь в магазин", "You go to the store", "Используйте 'you' и правильную форму глагола 'go'"},
                                                    {"Мы говорим по телефону", "We talk on the phone", "Используйте 'we' и правильную форму глагола 'talk'"},
                                                    {"Он читает книгу", "He reads a book", "Используйте 'he' и правильную форму глагола 'read'"},
                                                    {"Они едят ужин", "They eat dinner", "Используйте 'they' и правильную форму глагола 'eat'"},
                                                    {"Я бегаю утром", "I run in the morning", "Используйте 'I' и правильную форму глагола 'run'"},
                                                    {"Она учится в университете", "She studies at university", "Используйте 'she' и правильную форму глагола 'study'"},
                                                    {"Ты играешь на пианино", "You play the piano", "Используйте 'you' и правильную форму глагола 'play'"},
                                                    };
    std::vector<QuestionTranslate> questionsInfoInter = {
                                                    {"Она привыкла к этому", "She is used to it", "Используйте 'she' и правильную форму выражения 'be used to'"},
                                                    {"Я с нетерпением жду отпуска", "I am looking forward to my vacation", "Используйте 'I' и правильную форму выражения 'look forward to'"},
                                                    {"Они избегают говорить о проблеме", "They avoid talking about the issue", "Используйте 'they' и правильную форму глагола 'avoid'"},
                                                    {"Он отказался участвовать в соревновании", "He refused to participate in the competition", "Используйте 'he' и правильную форму глагола 'refuse'"},
                                                    {"Мы привыкли работать в команде", "We are accustomed to working in a team", "Используйте 'we' и правильную форму выражения 'be accustomed to'"},
                                                    {"Ты сомневаешься в своём решении", "You doubt your decision", "Используйте 'you' и правильную форму глагола 'doubt'"},
                                                    {"Солнце исчезло за горизонтом", "The sun disappeared beyond the horizon", "Используйте 'the sun' и правильную форму глагола 'disappear'"},
                                                    {"Они спорят по поводу планов", "They argue about the plans", "Используйте 'they' и правильную форму глагола 'argue'"},
                                                    {"Я убеждён, что это правда", "I am convinced that it is true", "Используйте 'I' и правильную форму выражения 'be convinced'"},
                                                    {"Она предлагает пойти в музей", "She suggests going to the museum", "Используйте 'she' и правильную форму глагола 'suggest'"},
                                                    {"Ты упомянул интересную идею", "You mentioned an interesting idea", "Используйте 'you' и правильную форму глагола 'mention'"},
                                                    {"Мы обсуждаем важный вопрос", "We discuss an important issue", "Используйте 'we' и правильную форму глагола 'discuss'"},
                                                    {"Он признал свою ошибку", "He admitted his mistake", "Используйте 'he' и правильную форму глагола 'admit'"},
                                                    {"Они наслаждаются путешествием", "They enjoy the trip", "Используйте 'they' и правильную форму глагола 'enjoy'"},
                                                    {"Я надеюсь на лучший результат", "I hope for a better result", "Используйте 'I' и правильную форму глагола 'hope'"},
                                                    {"Она зависит от своей семьи", "She depends on her family", "Используйте 'she' и правильную форму глагола 'depend'"},
                                                    {"Ты справился с задачей", "You managed to complete the task", "Используйте 'you' и правильную форму выражения 'manage to'"},
                                                    {"Мы выразили своё мнение", "We expressed our opinion", "Используйте 'we' и правильную форму глагола 'express'"},
                                                    {"Он сомневается в правильности решения", "He doubts the correctness of the decision", "Используйте 'he' и правильную форму глагола 'doubt'"},
                                                    {"Они решились на изменение", "They decided to make a change", "Используйте 'they' и правильную форму выражения 'decide to'"},
                                                    {"Я стремлюсь к успеху", "I strive for success", "Используйте 'I' и правильную форму глагола 'strive'"},
                                                    {"Она отказалась от предложения", "She declined the offer", "Используйте 'she' и правильную форму глагола 'decline'"},
                                                    {"Ты убедил меня попробовать", "You convinced me to try", "Используйте 'you' и правильную форму выражения 'convince to'"},
                                                    {"Мы оцениваем ситуацию", "We evaluate the situation", "Используйте 'we' и правильную форму глагола 'evaluate'"},
                                                    {"Он планирует путешествие", "He plans the trip", "Используйте 'he' и правильную форму глагола 'plan'"},
                                                    {"Они изучают новую тему", "They study a new topic", "Используйте 'they' и правильную форму глагола 'study'"},
                                                    {"Я надеюсь на удачу", "I hope for luck", "Используйте 'I' и правильную форму глагола 'hope'"},
                                                    {"Она выражает свои чувства", "She expresses her feelings", "Используйте 'she' и правильную форму глагола 'express'"},
                                                    {"Ты убедил друга принять участие", "You persuaded your friend to participate", "Используйте 'you' и правильную форму выражения 'persuade to'"},
                                                    {"Мы обсуждаем возможные решения", "We discuss possible solutions", "Используйте 'we' и правильную форму глагола 'discuss'"},
                                                    {"Он предлагает изменить стратегию", "He suggests changing the strategy", "Используйте 'he' и правильную форму глагола 'suggest'"},
                                                    {"Они спорят о результатах", "They argue about the results", "Используйте 'they' и правильную форму глагола 'argue'"},
                                                    {"Я изучаю новый подход", "I study a new approach", "Используйте 'I' и правильную форму глагола 'study'"},
                                                    {"Она убеждена в успехе", "She is convinced of success", "Используйте 'she' и правильную форму выражения 'be convinced'"},
                                                    {"Ты управляешь командой", "You manage the team", "Используйте 'you' и правильную форму глагола 'manage'"},
                                                    {"Мы анализируем проблему", "We analyze the problem", "Используйте 'we' и правильную форму глагола 'analyze'"},
                                                    {"Он разъясняет ситуацию", "He explains the situation", "Используйте 'he' и правильную форму глагола 'explain'"},
                                                    {"Они признают ошибку", "They admit the mistake", "Используйте 'they' и правильную форму глагола 'admit'"},
                                                    {"Я стараюсь быть объективным", "I try to be objective", "Используйте 'I' и правильную форму глагола 'try'"},
                                                    {"Она исследует новые возможности", "She explores new opportunities", "Используйте 'she' и правильную форму глагола 'explore'"},
                                                    {"Ты оцениваешь эффективность", "You evaluate efficiency", "Используйте 'you' и правильную форму глагола 'evaluate'"},
                                                    {"Мы следуем рекомендациям", "We follow the recommendations", "Используйте 'we' и правильную форму глагола 'follow'"},
                                                    {"Он описывает детали проекта", "He describes the project details", "Используйте 'he' и правильную форму глагола 'describe'"},
                                                    {"Они уточняют условия", "They clarify the conditions", "Используйте 'they' и правильную форму глагола 'clarify'"},
                                                    {"Я выражаю свою точку зрения", "I express my point of view", "Используйте 'I' и правильную форму глагола 'express'"},
                                                    };
    std::vector<QuestionTranslate> questionsInfoAdvanced = {
                                                    {"Она настаивает на своём мнении, несмотря на доказательства", "She insists on her opinion despite the evidence", "Используйте 'she' и правильную форму выражения 'insist on'"},
                                                    {"Я склонен полагать, что это верно", "I am inclined to believe that it is true", "Используйте 'I' и правильную форму выражения 'be inclined to'"},
                                                    {"Они стремятся к достижению амбициозных целей", "They aspire to achieve ambitious goals", "Используйте 'they' и правильную форму глагола 'aspire'"},
                                                    {"Он признаёт сложность ситуации", "He acknowledges the complexity of the situation", "Используйте 'he' и правильную форму глагола 'acknowledge'"},
                                                    {"Мы не можем недооценивать важность этого вопроса", "We cannot underestimate the importance of this issue", "Используйте 'we' и правильную форму выражения 'cannot underestimate'"},
                                                    {"Ты не должен пренебрегать деталями", "You must not overlook the details", "Используйте 'you' и правильную форму глагола 'overlook'"},
                                                    {"Солнце озарило горизонт, создавая величественное зрелище", "The sun illuminated the horizon, creating a magnificent spectacle", "Используйте 'the sun' и правильную форму глагола 'illuminate'"},
                                                    {"Они восхищаются его целеустремлённостью", "They admire his determination", "Используйте 'they' и правильную форму глагола 'admire'"},
                                                    {"Я категорически возражаю против этого решения", "I strongly object to this decision", "Используйте 'I' и правильную форму выражения 'object to'"},
                                                    {"Она призывает к более взвешенному подходу", "She advocates for a more balanced approach", "Используйте 'she' и правильную форму глагола 'advocate'"},
                                                    {"Ты опроверг доводы оппонента", "You refuted the opponent’s arguments", "Используйте 'you' и правильную форму глагола 'refute'"},
                                                    {"Мы анализируем возможные последствия", "We scrutinize the possible consequences", "Используйте 'we' и правильную форму глагола 'scrutinize'"},
                                                    {"Он делает выводы на основе эмпирических данных", "He draws conclusions based on empirical data", "Используйте 'he' и правильную форму выражения 'draw conclusions'"},
                                                    {"Они интерпретируют результаты исследования по-разному", "They interpret the research findings differently", "Используйте 'they' и правильную форму глагола 'interpret'"},
                                                    {"Я подчеркиваю необходимость срочных мер", "I emphasize the urgency of measures", "Используйте 'I' и правильную форму глагола 'emphasize'"},
                                                    {"Она стремится к профессиональному совершенству", "She strives for professional excellence", "Используйте 'she' и правильную форму выражения 'strive for'"},
                                                    {"Ты ставишь под сомнение традиционные методы", "You challenge traditional methods", "Используйте 'you' и правильную форму глагола 'challenge'"},
                                                    {"Мы оцениваем надёжность источника информации", "We assess the reliability of the information source", "Используйте 'we' и правильную форму глагола 'assess'"},
                                                    {"Он избегает излишнего упрощения сложных вопросов", "He avoids oversimplifying complex issues", "Используйте 'he' и правильную форму выражения 'avoid oversimplifying'"},
                                                    {"Они склонны игнорировать противоположные точки зрения", "They tend to ignore opposing viewpoints", "Используйте 'they' и правильную форму выражения 'tend to'"},
                                                    {"Я осознаю возможные последствия своих действий", "I am aware of the possible consequences of my actions", "Используйте 'I' и правильную форму выражения 'be aware of'"},
                                                    {"Она разоблачает недочёты предложенной стратегии", "She exposes the flaws of the proposed strategy", "Используйте 'she' и правильную форму глагола 'expose'"},
                                                    {"Ты формулируешь свои мысли с предельной ясностью", "You articulate your thoughts with utmost clarity", "Используйте 'you' и правильную форму глагола 'articulate'"},
                                                    {"Мы разрабатываем инновационные решения", "We devise innovative solutions", "Используйте 'we' и правильную форму глагола 'devise'"},
                                                    {"Он оспаривает устоявшиеся догмы", "He disputes established dogmas", "Используйте 'he' и правильную форму глагола 'dispute'"},
                                                    {"Они настаивают на независимой проверке данных", "They insist on an independent data review", "Используйте 'they' и правильную форму выражения 'insist on'"},
                                                    {"Я утверждаю, что этот аргумент несостоятелен", "I assert that this argument is invalid", "Используйте 'I' и правильную форму глагола 'assert'"},
                                                    {"Она ставит под сомнение общепринятые предположения", "She questions conventional assumptions", "Используйте 'she' и правильную форму глагола 'question'"},
                                                    {"Ты формулируешь критический анализ проблемы", "You formulate a critical analysis of the issue", "Используйте 'you' и правильную форму глагола 'formulate'"},
                                                    {"Мы идентифицируем ключевые аспекты проблемы", "We identify the key aspects of the problem", "Используйте 'we' и правильную форму глагола 'identify'"},
                                                    {"Он резюмирует основные положения теории", "He summarizes the main points of the theory", "Используйте 'he' и правильную форму глагола 'summarize'"},
                                                    {"Они исследуют причинно-следственные связи", "They investigate causal relationships", "Используйте 'they' и правильную форму глагола 'investigate'"},
                                                    {"Я выдвигаю гипотезу о возможных сценариях", "I propose a hypothesis about possible scenarios", "Используйте 'I' и правильную форму глагола 'propose'"},
                                                    {"Она рассматривает альтернативные подходы", "She considers alternative approaches", "Используйте 'she' и правильную форму глагола 'consider'"},
                                                    {"Ты подкрепляешь доводы убедительными доказательствами", "You substantiate your arguments with compelling evidence", "Используйте 'you' и правильную форму глагола 'substantiate'"},
                                                    {"Мы определяем взаимосвязь между явлениями", "We determine the correlation between phenomena", "Используйте 'we' и правильную форму глагола 'determine'"},
                                                    {"Он анализирует возможные последствия политических решений", "He examines the possible consequences of political decisions", "Используйте 'he' и правильную форму глагола 'examine'"},
                                                    {"Они учитывают многообразие мнений", "They take into account the diversity of opinions", "Используйте 'they' и правильную форму выражения 'take into account'"},
                                                    {"Я признаю необходимость корректировки стратегии", "I acknowledge the need to adjust the strategy", "Используйте 'I' и правильную форму глагола 'acknowledge'"},
                                                    };

    std::vector<QuestionTranslate> currentQuestions = questionsInfoBeginer;

    int currentQuestionIndex = 0;

    int emplCorrect = 0;

    QTimer* timer;

    int tasksD = 0;

    int wellDone = 0;
};

#endif // TRANSLATE_H
