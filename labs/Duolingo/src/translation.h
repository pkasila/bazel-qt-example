#ifndef TRANSLATION_H
#define TRANSLATION_H

#include <QStringList>
#include <QLabel>
#include <QRadioButton>
#include <QPushButton>
#include <QVBoxLayout>
#include <QProgressBar>
#include <QTimer>
#include <QButtonGroup>
#include <QTextEdit>

enum Mode{
    TRANSLATION,
    WRITING
};

class Translation : public QWidget
{
    Q_OBJECT
public:
    explicit Translation(QWidget* parent = nullptr);
    ~Translation() override;

    int GetRightAnswers();
    int GetQuestionsQuantity();
    void SetMode(Mode val);


signals:
    void EndGame();

private:
    void SetUI();
    void NextQuestion();
    void CheckAnswer();
    void StartGame();
    void RenewQuiz();
    int questionNumber = 0;
    int right_answers = -1;

    Mode mode;

    std::vector<std::pair<QString, std::vector<QString>>> questions = {
        {"Apple",     {"Яблоко", "Груша", "Апельсин", "Банан"}},
        {"Sunshine",  {"Дождь","Солнечный свет",  "Ветер", "Облако"}},
        {"Freedom",   {"Свобода", "Рабство", "Тюрьма", "Зависимость"}},
        {"Mountain",  {"Река", "Долина", "Океан", "Гора"}},
        {"Whisper",   {"Шёпот", "Крик", "Пение", "Смех"}},
        {"Journey",   {"Дом", "Работа","Путешествие", "Сон"}},
        {"Melody",    {"Шум", "Мелодия", "Тишина", "Грохот"}},
        {"Courage",   {"Страх", "Сомнение", "Лень", "Смелость"}},
        {"Computer",  {"Компьютер", "Телефон", "Телевизор", "Холодильник"}},
        {"Butterfly", {"Бабочка", "Пчела", "Муравей", "Паук"}}
    };
    std::vector<int> answers = {0, 1, 0, 3, 0, 2, 1, 3, 0, 0};
    QVBoxLayout* vertical;
    QButtonGroup* buttons;
    QPushButton* checkButton;
    QPushButton* startButton;
    QLabel* questionText;
    QTextEdit* insertText;
};

#endif // TRANSLATION_H
