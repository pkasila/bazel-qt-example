#ifndef GRAMMAR_H
#define GRAMMAR_H

#include <QWidget>
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
#include <QPushButton>
#include <QDialog>
#include <QRadioButton>
#include <QButtonGroup>
#include <QSoundEffect>

struct QuestionGrammar {
    QString que;
    QString correctAns;
    QString hint;
    QString ans[4];
};

class Grammar : public QWidget
{
    Q_OBJECT
public:
    explicit Grammar(QWidget *parent = nullptr);
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
private slots:
    void checkans();

signals:
    void start();
    void end();

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

    int seconds = 60;
    int attempts = 6;

    int numTasks = 10;

    int difficulty = 0;
    int localScore = 0;

    QSoundEffect* correctAns;
    QSoundEffect* notCorrectAns;
    QSoundEffect* levelComplete;
    QSoundEffect* lost;
    QSoundEffect* click;

    std::vector<QuestionGrammar> questionsInfoBeginer = {
        {
            "I __ to school every day.",
            "go",
            "Здесь надо исполльзовать Present Simple",
            {"go", "going", "goes", "went"}
        },
        {
            "She __ a book now.",
            "is reading",
            "Используйте Present Continuous",
            {"reads", "is reading", "read", "reading"}
        },
        {
            "They __ football yesterday.",
            "played",
            "используйте Past Simple для завершённых действий",
            {"play", "plays", "played", "were playing"}
        },
        {
            "We __ never __ to Paris.",
            "have, been",
            "Используйте Present Perfect",
            {"have, been", "had, been", "has, been", "were, being"}
        },
        {
            "__ you __ TV last night?",
            "Did, watch",
            "Вопрос в Past Simple",
            {"Do, watch", "Did, watch", "Were, watching", "Have, watched"}
        },
        {
            "There __ many people at the party.",
            "were",
            "'There is/are'",
            {"was", "were", "is", "are"}
        },
        {
            "He __ swim when he was 5.",
            "could",
            "Модальный глагол 'could' для past ability",
            {"can", "could", "may", "might"}
        },
        {
            "This is __ book.",
            "my",
            "Это Possessive adjectives",
            {"me", "my", "I", "mine"}
        },
        {
            "She is __ than her sister.",
            "taller",
            "Это Comparative adjectives, средняя степень сравнения",
            {"tall", "taller", "tallest", "more tall"}
        },
        {
            "They __ coming to the party.",
            "are",
            "Present Continuous для будущих договоренностей",
            {"is", "are", "am", "be"}
        },
        {
            "I have __ finished my homework.",
            "just",
            "Present Perfect вместе с 'just'",
            {"just", "yet", "already", "never"}
        },
        {
            "Would you like __ coffee?",
            "some",
            "Предложения с 'some'",
            {"some", "any", "a", "an"}
        },
        {
            "She __ her teeth twice a day.",
            "brushes",
            "Present Simple о привычках (от 3-го лица)",
            {"brush", "brushes", "is brushing", "brushed"}
        },
        {
            "We __ to the cinema last weekend.",
            "went",
            "Past Simple, неправильные глаголы",
            {"go", "goes", "went", "gone"}
        },
        {
            "It's very cold. You __ wear a coat.",
            "should",
            "Совет со словом 'should'",
            {"should", "must", "can", "would"}
        },
        {
            "There isn't __ milk in the fridge.",
            "any",
            "Отрицание с 'any'",
            {"some", "any", "a", "the"}
        },
        {
            "I'm __ tired to go out.",
            "too",
            "'Too' перед прилагательными",
            {"too", "very", "so", "enough"}
        },
        {
            "This is the __ movie I've ever seen.",
            "best",
            "Прилагательные в превосходной степени",
            {"good", "better", "best", "well"}
        },
        {
            "She __ to music when I called.",
            "was listening",
            "Past Continuous для прерванных действий",
            {"listened", "was listening", "is listening", "listens"}
        },
        {
            "They have lived here __ 2010.",
            "since",
            "Present Perfect со словом 'since'",
            {"for", "since", "from", "in"}
        },
        {
            "You __ eat so much sugar.",
            "shouldn't",
            "Отрицание совет",
            {"shouldn't", "mustn't", "don't have to", "can't"}
        },
        {
            "I __ my keys. Can you help me find them?",
            "have lost",
            "Present Perfect о последних событиях",
            {"lose", "lost", "have lost", "am losing"}
        },
        {
            "He __ his homework yet.",
            "hasn't finished",
            "Present Perfect, отрицание",
            {"didn't finish", "hasn't finished", "doesn't finish", "isn't finishing"}
        },
        {
            "How long __ you been here?",
            "have",
            "Вопрос в Present Perfect",
            {"have", "has", "are", "do"}
        },
        {
            "She __ to the supermarket when she met her friend.",
            "was going",
            "Past Continuous для фоновых действий",
            {"went", "was going", "goes", "is going"}
        },
        {
            "If it rains, we __ stay at home.",
            "will",
            "В одном из мест стоит глагол в будущей форме. После if не может стоять глагол в будущем времени.",
            {"will", "would", "should", "can"}
        },
        {
            "This exercise is __ difficult for me.",
            "too",
            "'Too' или 'enough'",
            {"too", "very", "enough", "so"}
        },
        {
            "She speaks English __ than her brother.",
            "better",
            "Сравнительная",
            {"good", "well", "better", "best"}
        },
        {
            "I __ already seen this movie.",
            "have",
            "Present Perfect со словом 'already'",
            {"have", "has", "had", "am"}
        },
        {
            "They __ their grandparents next week.",
            "are visiting",
            "Present Continuous в будущем",
            {"visit", "are visiting", "visited", "will visit"}
        },
        {
            "You look tired. You __ go to bed early.",
            "should",
            "Совет со словом 'should'",
            {"should", "must", "can", "would"}
        },
        {
            "How __ cheese do you need?",
            "much",
            "much или any",
            {"many", "much", "some", "any"}
        },
        {
            "She __ her homework before dinner.",
            "had done",
            "Past Perfect",
            {"did", "had done", "has done", "was doing"}
        },
        {
            "I wish I __ play the piano.",
            "could",
            "'Wish' с 'could'",
            {"can", "could", "will", "would"}
        },
        {
            "The train __ at 9:00 tomorrow.",
            "leaves",
            "Present Simple для расписаний",
            {"leaves", "is leaving", "will leave", "left"}
        },
        {
            "He's not here. He __ to the bank.",
            "has gone",
            "Present Perfect со словом 'gone'",
            {"has gone", "has been", "went", "goes"}
        },
        {
            "This is __ interesting book.",
            "an",
            "Мы не знаем конкретная это книга или нет",
            {"a", "an", "the", "-"}
        },
        {
            "She __ to be a doctor when she grows up.",
            "wants",
            "Present Simple для будущих намерений",
            {"want", "wants", "is wanting", "will want"}
        },
        {
            "I __ my phone at home yesterday.",
            "left",
            "Past Simple неправильные глаголы",
            {"leave", "leaves", "left", "leaving"}
        },
        {
            "We __ each other for ten years.",
            "have known",
            "Present Perfect со словом 'for'",
            {"know", "knew", "have known", "are knowing"}
        },
        {
            "If I __ you, I would apologize.",
            "were",
            "Второе условное",
            {"am", "was", "were", "would be"}
        },
        {
            "This is __ most expensive restaurant in town.",
            "the",
            "Превосходная степень с the",
            {"a", "an", "the", "-"}
        },
        {
            "She __ her keys and can't get in.",
            "has lost",
            "Present Perfect для текущей актуальности",
            {"loses", "lost", "has lost", "is losing"}
        },
        {
            "How __ children do they have?",
            "many",
            "Вопросы о счетных существительных",
            {"many", "much", "long", "often"}
        },
        {
            "I __ breakfast when the phone rang.",
            "was having",
            "Прошлые непрерывные прерванные действия",
            {"had", "was having", "have", "am having"}
        },
        {
            "You __ wear a uniform at this school.",
            "have to",
            "Обязательство с \"have to\"",
            {"have to", "must", "should", "can"}
        },
        {
            "She's the girl __ lives next door.",
            "who",
            "Относительные местоимения",
            {"which", "who", "where", "whose"}
        },
        {
            "I __ to the cinema since January.",
            "haven't been",
            "Present Perfect, отрицание",
            {"didn't go", "haven't been", "don't go", "am not going"}
        },
        {
            "This coffee is __ hot to drink.",
            "too",
            "'Too'",
            {"too", "very", "enough", "so"}
        },
        {
            "They __ already left when we arrived.",
            "had",
            "Past Perfect",
            {"have", "had", "were", "did"}
        }
    };

    QButtonGroup* buttonGroup;
    QRadioButton* radio1;
    QRadioButton* radio2;
    QRadioButton* radio3;
    QRadioButton* radio4;

    std::vector<QuestionGrammar> questionsInfoInter = {
        {
            "By next year, I __ my degree.",
            "will have completed",
            "Будущее Идеально подходит для завершенных действий до наступления будущего времени",
            {"will complete", "will have completed", "will be completing", "complete"}
        },
        {
            "If I __ about the traffic, I would have left earlier.",
            "had known",
            "Третье условие для нереальных ситуаций в прошлом",
            {"knew", "had known", "would know", "have known"}
        },
        {
            "She suggested that he __ a doctor.",
            "see",
            "Сослагательное наклонение после слова \"suggest\"",
            {"sees", "see", "saw", "would see"}
        },
        {
            "Not only __ late, but he also forgot the documents.",
            "was he",
            "Инверсия после отрицательных наречий",
            {"he was", "was he", "did he be", "he is"}
        },
        {
            "I wish you __ interrupting me all the time!",
            "would stop",
            "'Wish' с 'would' для обозначения раздражающих привычек",
            {"stop", "would stop", "stopped", "had stopped"}
        },
        {
            "The report __ by the time the meeting started.",
            "had been finished",
            "Past Perfect Passive",
            {"had finished", "had been finished", "was finished", "finished"}
        },
        {
            "Hardly __ home when the phone rang.",
            "had I arrived",
            "Инверсия 'hardly...when'",
            {"I had arrived", "had I arrived", "I arrived", "did I arrive"}
        },
        {
            "You look exhausted. You __ been working too hard.",
            "must have",
            "Способы дедукции (в прошлом)",
            {"must have", "should have", "could have", "might have"}
        },
        {
            "I'd rather you __ smoking.",
            "gave up",
            "'Would rather' с past simple",
            {"give up", "gave up", "would give up", "had given up"}
        },
        {
            "No sooner __ the news than she burst into tears.",
            "had she heard",
            "Инверсия с 'no sooner...than'",
            {"she had heard", "had she heard", "she heard", "did she hear"}
        },
        {
            "This time next week, I __ on a beach in Spain.",
            "will be lying",
            "Future Continuous для выполнения действий в будущем",
            {"will lie", "will be lying", "will have lain", "lie"}
        },
        {
            "The project __ by the end of this month.",
            "will have been completed",
            "Future Perfect Passive",
            {"will complete", "will have completed", "will be completing", "will have been completed"}
        },
        {
            "Were I __, I would accept their offer.",
            "in your position",
            "Инверсия в условных обозначениях (формальная)",
            {"in your position", "you", "your position", "position"}
        },
        {
            "She __ have taken the money - she wasn't even there!",
            "can't",
            "Способы дедукции (отрицательные)",
            {"mustn't", "can't", "might not", "wouldn't"}
        },
        {
            "I'd prefer __ to the cinema rather than stay at home.",
            "to go",
            "'Would prefer' с инфинитивом",
            {"go", "to go", "going", "went"}
        },
        {
            "Little __ that the decision would change her life.",
            "did she know",
            "Инверсия с 'little'",
            {"she knew", "did she know", "she knows", "knows she"}
        },
        {
            "By 2025, scientists __ a cure for this disease.",
            "will have discovered",
            "Future Perfect за достижения",
            {"will discover", "will have discovered", "are discovering", "discover"}
        },
        {
            "Supposing you __ the job, would you move abroad?",
            "were offered",
            "Кондишнлс с 'supposing'",
            {"were offered", "offered", "would offer", "had offered"}
        },
        {
            "Not until last year __ how to drive.",
            "did I learn",
            "Инверсия с 'not until'",
            {"I learned", "did I learn", "I had learned", "learned I"}
        },
        {
            "The package __ when I got to the post office.",
            "had already been sent",
            "Past Perfect Passive",
            {"had already sent", "had already been sent", "was already sent", "already sent"}
        },
        {
            "Were she __, she would explain everything.",
            "here",
            "Инверсия в условных обозначениях (формальная)",
            {"here", "there", "present", "available"}
        },
        {
            "You __ told me you were vegetarian - I wouldn't have cooked meat!",
            "might have",
            "Modals for criticism (past)",
            {"should have", "might have", "could have", "must have"}
        },
        {
            "So disgusting __ that nobody could eat it.",
            "was the food",
            "Inversion with 'so + adjective'",
            {"the food was", "was the food", "the food is", "is the food"}
        },
        {
            "I'd sooner you __ me the truth.",
            "told",
            "'Would sooner' with past simple",
            {"tell", "told", "would tell", "had told"}
        },
        {
            "The manuscript __ by the end of the week.",
            "will have been translated",
            "Future Perfect Passive",
            {"will translate", "will have translated", "will be translating", "will have been translated"}
        },
        {
            "Rarely __ such a beautiful sunset.",
            "have I seen",
            "Inversion with 'rarely'",
            {"I have seen", "have I seen", "I saw", "saw I"}
        },
        {
            "He __ have received my email - I sent it yesterday.",
            "should",
            "Modals of expectation",
            {"should", "must", "might", "could"}
        },
        {
            "Had I __ the consequences, I would have acted differently.",
            "foreseen",
            "Inversion in third conditional",
            {"foreseen", "foresaw", "would foresee", "had foresaw"}
        },
        {
            "The results __ by the time we arrived.",
            "had already been announced",
            "Past Perfect Passive",
            {"had already announced", "had already been announced", "were already announced", "already announced"}
        },
        {
            "Only when __ understand what had happened.",
            "did I",
            "Inversion with 'only when'",
            {"I did", "did I", "I had", "had I"}
        },
        {
            "You __ left a note - I had no idea where you'd gone!",
            "might have",
            "Modals for criticism (past)",
            {"should have", "might have", "could have", "must have"}
        },
        {
            "Such __ the demand that we had to print more copies.",
            "was",
            "Inversion with 'such'",
            {"was", "is", "were", "are"}
        },
        {
            "I'd just as soon __ at home tonight.",
            "stay",
            "'Would just as soon' with base verb",
            {"stay", "to stay", "stayed", "staying"}
        },
        {
            "The new bridge __ by next summer.",
            "will have been built",
            "Future Perfect Passive",
            {"will build", "will have built", "will be building", "will have been built"}
        },
        {
            "Never before __ so embarrassed.",
            "had I felt",
            "Inversion with 'never before'",
            {"I had felt", "had I felt", "I felt", "felt I"}
        },
        {
            "They __ have missed the train - it's not like them to be late.",
            "may",
            "Modals of deduction (past)",
            {"must", "may", "should", "would"}
        },
        {
            "Were the situation __, we would take immediate action.",
            "to worsen",
            "Conditionals with 'were + infinitive'",
            {"worsen", "to worsen", "worsened", "worsening"}
        },
        {
            "The documents __ before the meeting started.",
            "had been prepared",
            "Past Perfect Passive",
            {"had prepared", "had been prepared", "were prepared", "prepared"}
        },
        {
            "Only by working hard __ achieve your goals.",
            "can you",
            "Inversion with 'only by'",
            {"you can", "can you", "you could", "could you"}
        },
        {
            "You __ have called - I was worried sick!",
            "might",
            "Modals for criticism (past)",
            {"should", "might", "could", "must"}
        },
        {
            "So quickly __ that few people noticed it.",
            "did the incident occur",
            "Inversion with 'so + adverb'",
            {"the incident occurred", "did the incident occur", "occurred the incident", "the incident occurs"}
        },
        {
            "I'd just as soon you __ me the whole story.",
            "told",
            "'Would just as soon' with past simple",
            {"tell", "told", "would tell", "had told"}
        },
        {
            "All the tickets __ by the end of today.",
            "will have been sold",
            "Future Perfect Passive",
            {"will sell", "will have sold", "will be selling", "will have been sold"}
        },
        {
            "Seldom __ such dedication in a young athlete.",
            "have I seen",
            "Inversion with 'seldom'",
            {"I have seen", "have I seen", "I saw", "saw I"}
        },
        {
            "He __ have forgotten our anniversary - he never does!",
            "can't",
            "Modals of deduction (negative past)",
            {"mustn't", "can't", "might not", "wouldn't"}
        },
        {
            "Had they __ earlier, they could have helped us.",
            "arrived",
            "Inversion in third conditional",
            {"arrived", "arrive", "would arrive", "had arrive"}
        },
        {
            "The decision __ before the manager returned.",
            "had already been made",
            "Past Perfect Passive",
            {"had already made", "had already been made", "was already made", "already made"}
        },
        {
            "Only after __ realize my mistake.",
            "did I",
            "Inversion with 'only after'",
            {"I did", "did I", "I had", "had I"}
        },
        {
            "You __ have warned me about the traffic!",
            "could",
            "Modals for criticism (past)",
            {"should", "could", "must", "might"}
        },
        {
            "Such __ his anger that he couldn't speak.",
            "was",
            "Inversion with 'such'",
            {"was", "is", "were", "are"}
        }
    };

    std::vector<QuestionGrammar> questionsInfoAdvanced = {
        {
            "Had it not been for your intervention, the deal __ through.",
            "would have fallen",
            "Смешанные условия, подчеркивающие прошлые последствия",
            {"would fall", "would have fallen", "had fallen", "fell"}
        },
        {
            "Not for a moment __ that the allegations were true.",
            "did I believe",
            "Инверсия с отрицательными наречиями для придания особого значения",
            {"I believed", "did I believe", "I had believed", "believed I"}
        },
        {
            "Such __ the complexity of the issue that experts disagreed.",
            "was",
            "Инверсия с \"таким\" для достижения драматического эффекта",
            {"was", "were", "is", "are"}
        },
        {
            "The minister is said __ the controversial bill.",
            "to have drafted",
            "Совершенный инфинитив в пассивных отчетных структурах",
            {"to draft", "to have drafted", "to be drafting", "having drafted"}
        },
        {
            "Were the negotiations __, it would jeopardize the entire agreement.",
            "to break down",
            "Были + инфинитив для обозначения гипотетической будущей ситуацииs",
            {"breaking down", "to break down", "broken down", "break down"}
        },
        {
            "Little __ how profoundly the discovery would change science.",
            "did they realize",
            "Инверсия с \"небольшим\" акцентом",
            {"they realized", "did they realize", "they had realized", "realized they"}
        },
        {
            "The manuscript is believed __ in the 15th century.",
            "to have been written",
            "Пассивный совершенный инфинитив в отчетных структурах",
            {"to write", "to have written", "to have been written", "being written"}
        },
        {
            "Had the evidence __ more convincing, the verdict might have been different.",
            "been",
            "Перевернутое прошедшее совершенное условное",
            {"been", "was", "had been", "were"}
        },
        {
            "No sooner __ one crisis than another emerged.",
            "had they averted",
            "Инверсия с \"не раньше... чем\" для упорядочивания прошлых событий",
            {"they had averted", "had they averted", "they averted", "averted they"}
        },
        {
            "The proposal leaves much __.",
            "to be desired",
            "Пассивный инфинитив для подразумеваемой критики",
            {"to desire", "to be desired", "desiring", "desired"}
        },
        {
            "Were the situation __, drastic measures would be required.",
            "to deteriorate further",
            "Были + инфинитив для обозначения гипотетического будущего",
            {"deteriorating further", "to deteriorate further", "deteriorated further", "further deteriorating"}
        },
        {
            "Only by examining all possibilities __ reach a valid conclusion.",
            "can one",
            "Формальная инверсия с помощью 'only by'",
            {"one can", "can one", "one could", "could one"}
        },
        {
            "The legislation is understood __ by the opposition party.",
            "to be being challenged",
            "Непрерывный пассивный инфинитив в репортаже",
            {"to challenge", "to be challenging", "to be challenged", "to be being challenged"}
        },
        {
            "Such __ the public outcry that the policy was reconsidered.",
            "was",
            "Инверсия 'such' для придания драматического эффекта",
            {"was", "were", "is", "are"}
        },
        {
            "Had the precautions __ taken, the accident could have been avoided.",
            "been",
            "Перевернутый пассивный совершенный условный",
            {"been", "being", "had been", "were"}
        },
        {
            "At no time __ aware of the breach.",
            "was the CEO",
            "Инверсия с 'at no time'",
            {"the CEO was", "was the CEO", "the CEO had been", "had the CEO been"}
        },
        {
            "The findings are thought __ light on the ancient civilization.",
            "to shed new",
            "Сложный инфинитив в отчетных структурах",
            {"to shed new", "to be shedding new", "to have shed new", "shedding new"}
        },
        {
            "Were the company __ losses, redundancies would be inevitable.",
            "to sustain further",
            "Were + infinitive для гипотетического будущего",
            {"sustaining further", "to sustain further", "sustained further", "further sustaining"}
        },
        {
            "Not until the autopsy was completed __ the cause of death.",
            "did investigators establish",
            "Инверсия с 'not until'",
            {"investigators established", "did investigators establish", "had investigators established", "investigators had established"}
        },
        {
            "The theory is considered __ by most experts in the field.",
            "to have been discredited",
            "Perfect passive infinitive в отчетности",
            {"to discredit", "to have discredited", "to have been discredited", "discrediting"}
        },
        {
            "Had the warning signs __ heeded, the disaster might have been averted.",
            "been",
            "Перевернутый пассивный совершенный условный",
            {"been", "be", "had been", "were"}
        },
        {
            "Under no circumstances __ the confidential documents.",
            "should you disclose",
            "Инверсия с 'under no circumstances'",
            {"you should disclose", "should you disclose", "you disclose", "disclose you"}
        },
        {
            "The manuscript is believed __ by the author in his final years.",
            "to have been composed",
            "Perfect passive infinitive in reporting",
            {"to compose", "to have composed", "to have been composed", "composing"}
        },
        {
            "Were the evidence __, the case would be reopened.",
            "to emerge",
            "Were + infinitive for hypothetical future",
            {"emerging", "to emerge", "emerged", "emerge"}
        },
        {
            "Only when all factors are considered __ appreciate the complexity.",
            "can one truly",
            "Formal inversion with 'only when'",
            {"one can truly", "can one truly", "one truly can", "truly can one"}
        },
        {
            "The treaty is said __ by both parties.",
            "to have been violated",
            "Perfect passive infinitive in reporting",
            {"to violate", "to have violated", "to have been violated", "violating"}
        },
        {
            "Had the data __ properly analyzed, the error would have been detected.",
            "been",
            "Inverted passive perfect conditional",
            {"been", "be", "had been", "were"}
        },
        {
            "In no way __ responsible for the mishap.",
            "was the technician",
            "Inversion with 'in no way'",
            {"the technician was", "was the technician", "the technician had been", "had the technician been"}
        },
        {
            "The phenomenon is understood __ over centuries.",
            "to have been evolving",
            "Perfect continuous infinitive in reporting",
            {"to evolve", "to have evolved", "to have been evolving", "evolving"}
        },
        {
            "Were the trend __, significant changes would be required.",
            "to continue",
            "Were + infinitive for hypothetical future",
            {"continuing", "to continue", "continued", "continue"}
        },
        {
            "Not only __ the financial loss, but the reputational damage was considerable.",
            "was there",
            "Inversion with 'not only'",
            {"there was", "was there", "there had been", "had there been"}
        },
        {
            "The technique is thought __ in ancient times.",
            "to have been employed",
            "Perfect passive infinitive in reporting",
            {"to employ", "to have employed", "to have been employed", "employing"}
        },
        {
            "Had the protocol __ followed, the breach wouldn't have occurred.",
            "been",
            "Inverted passive perfect conditional",
            {"been", "be", "had been", "were"}
        },
        {
            "On no account __ access to these files.",
            "should unauthorized personnel be granted",
            "Inversion with 'on no account'",
            {"unauthorized personnel should be granted", "should unauthorized personnel be granted", "be granted unauthorized personnel should", "granted should unauthorized personnel be"}
        },
        {
            "The artifact is believed __ during the Ming dynasty.",
            "to have been crafted",
            "Perfect passive infinitive in reporting",
            {"to craft", "to have crafted", "to have been crafted", "crafting"}
        },
        {
            "Were the allegations __, it would cause a political scandal.",
            "to prove true",
            "Were + infinitive for hypothetical future",
            {"proving true", "to prove true", "proved true", "prove true"}
        },
        {
            "Only by considering all perspectives __ a balanced view.",
            "can one form",
            "Formal inversion with 'only by'",
            {"one can form", "can one form", "one could form", "could one form"}
        },
        {
            "The incident is reported __ multiple witnesses.",
            "to have been seen by",
            "Perfect passive infinitive with agent",
            {"to see by", "to have seen by", "to have been seen by", "seeing by"}
        },
        {
            "Had proper precautions __, the accident could have been prevented.",
            "been taken",
            "Inverted passive perfect conditional",
            {"taken", "been taken", "had been taken", "were taken"}
        },
        {
            "Under no circumstances __ the integrity of the experiment.",
            "should the results compromise",
            "Inversion with 'under no circumstances'",
            {"the results should compromise", "should the results compromise", "the results compromise should", "compromise should the results"}
        },
        {
            "The theory is understood __ several times since its inception.",
            "to have been revised",
            "Perfect passive infinitive in reporting",
            {"to revise", "to have revised", "to have been revised", "revising"}
        },
        {
            "Were the findings __, it would revolutionize the field.",
            "to be confirmed",
            "Were + passive infinitive for hypothetical future",
            {"being confirmed", "to be confirmed", "confirmed", "confirming"}
        },
        {
            "Not until the investigation was complete __ the full extent of the damage.",
            "did authorities realize",
            "Inversion with 'not until'",
            {"authorities realized", "did authorities realize", "authorities had realized", "had authorities realized"}
        },
        {
            "The procedure is said __ in medieval Europe.",
            "to have originated",
            "Perfect infinitive in reporting structures",
            {"to originate", "to have originated", "to be originating", "originating"}
        },
        {
            "Had the warning signs __ noticed earlier, the crisis might have been averted.",
            "been",
            "Inverted passive perfect conditional",
            {"been", "be", "had been", "were"}
        },
        {
            "In no way __ the outcome of the clinical trials.",
            "were the results skewed",
            "Inversion with 'in no way'",
            {"the results were skewed", "were the results skewed", "the results had been skewed", "had the results been skewed"}
        },
        {
            "The technique is believed __ over generations.",
            "to have been perfected",
            "Perfect passive infinitive in reporting",
            {"to perfect", "to have perfected", "to have been perfected", "perfecting"}
        },
        {
            "Were the hypothesis __, it would challenge fundamental assumptions.",
            "to hold true",
            "Were + infinitive for hypothetical future",
            {"holding true", "to hold true", "held true", "hold true"}
        },
        {
            "Only through meticulous analysis __ these subtle patterns.",
            "can researchers identify",
            "Formal inversion with 'only through'",
            {"researchers can identify", "can researchers identify", "researchers could identify", "could researchers identify"}
        }
    };

    std::vector<QuestionGrammar> currentQuestions = questionsInfoBeginer;

    int currentQuestionIndex = 0;

    int emplCorrect = 0;

    QTimer* timer;

    int tasksD = 0;

    int wellDone = 0;
};

#endif // GRAMMAR_H
