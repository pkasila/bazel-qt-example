#pragma once
#include <QString>
#include <vector>

struct TranslationTask {
    QString prompt;
    QString expectedAnswer;
    QString hint;
};

struct GrammarTask {
    QString sentencePart1;
    QString sentencePart2;
    std::vector<QString> options;
    int correctIndex;
    QString hint;
};

// Генерация пула заданий
class TaskDatabase {
public:
    static std::vector<TranslationTask> getTranslationTasks() {
        return {
            {"Apple", "яблоко", "Это фрукт, любимый фрукт Стива Джобса."},
            {"The cat is sleeping", "кот спит", "Настоящее время. Кот (он)."},
            {"I like learning foreign languages", "мне нравится изучать иностранные языки", "Глагол 'нравится' требует дательного падежа местоимения."},
            {"Software engineering", "программная инженерия", "То, чем вы сейчас занимаетесь на C++."},
            {"Always code as if the guy who ends up maintaining your code will be a violent psychopath", 
             "всегда пишите код так как будто сопровождать его будет склонный к насилию психопат", 
             "Знаменитая цитата Джона Вудса. Не забудьте пунктуацию алгоритм простит."}
        };
    }

    static std::vector<GrammarTask> getGrammarTasks() {
        return {
            {"She ", " to the gym every day.", {"go", "goes", "going"}, 1, "Present Simple для 3-го лица ед. ч. требует окончания -es."},
            {"If I had a million dollars, I ", " a yacht.", {"would buy", "will buy", "bought"}, 0, "Second Conditional: If + Past Simple, ... would + Verb."},
            {"By the time you arrive, we ", " dinner.", {"will finish", "will have finished", "finished"}, 1, "Действие завершится к определенному моменту в будущем (Future Perfect)."},
            {"I am looking forward to ", " you.", {"see", "seeing", "saw"}, 1, "После 'look forward to' используется герундий (V-ing)."},
            {"The building ", " in 1999.", {"was built", "is built", "built"}, 0, "Пассивный залог в прошедшем времени (Past Simple Passive)."}
        };
    }
};
