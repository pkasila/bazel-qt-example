#include "questionrepository.h"
#include <algorithm>
#include <random>

std::vector<Question> QuestionRepository::getQuestions(int difficultyLevel, int count, int filterType) {
    std::vector<Question> allQuestions = {
        // Translations (Multiple answers supported)
        {QuestionType::Translation, "Translate: 'Apple'", {"яблоко", "яблочко", "райское яблочко"}, {}, "Hint: a red or green fruit"},
        {QuestionType::Translation, "Translate: 'Good morning'", {"доброе утро", "доброго утра"}, {}, "Hint: Use this phrase before 12 PM"},
        {QuestionType::Translation, "Translate: 'I am learning C++'", {"я изучаю c++", "я учу c++", "я учу плюсы", "изучаю c++"}, {}, "Hint: First person singular"},
        {QuestionType::Translation, "Translate: 'University'", {"университет", "универ", "вуз", "высшее учебное заведение"}, {}, "Hint: BSU is one of these"},
        {QuestionType::Translation, "Translate: 'Always check memory leaks'", {"всегда проверяй утечки памяти", "всегда проверяйте утечки памяти", "проверяй утечку памяти"}, {}, "Hint: C++ best practice"},
        {QuestionType::Translation, "Translate: 'Software Engineer'", {"инженер-программист", "софтвер инжинер", "программист", "разработчик"}, {}, "Hint: Your future job title"},
        {QuestionType::Translation, "Translate: 'I love programming'", {"я люблю программирование", "я люблю программировать", "мне нравится программировать"}, {}, "Hint: Think about why you are here"},
        {QuestionType::Translation, "Translate: 'Window'", {"окно", "окошко"}, {}, "Hint: GUI component or house part"},
        
        // RU -> EN Translations
        {QuestionType::Translation, "Translate to EN: 'Яблоко'", {"apple", "an apple", "the apple"}, {}, "Hint: A fruit that keeps the doctor away"},
        {QuestionType::Translation, "Translate to EN: 'Я изучаю C++'", {"i learn c++", "i am learning c++", "i study c++", "i am studying c++"}, {}, "Hint: Present Continuous or Simple"},
        {QuestionType::Translation, "Translate to EN: 'Окно'", {"window", "the window", "a window"}, {}, "Hint: You look through it"},
        {QuestionType::Translation, "Translate to EN: 'Разработчик ПО'", {"software engineer", "software developer", "developer", "programmer"}, {}, "Hint: You"},
        {QuestionType::Translation, "Translate to EN: 'Без труда не вытащишь и рыбку из пруда'", {"no pain no gain", "no cross no crown"}, {}, "Hint: Famous EN idiom"},
        {QuestionType::Translation, "Translate to EN: 'Университет'", {"university", "college", "the university", "a university"}, {}, "Hint: BSU is one"},
        
        // Grammar
        {QuestionType::Grammar, "Choose the correct verb: He ___ to school every day.", {"goes"}, {"go", "goes", "going", "gone"}, "Hint: Present Simple (he/she/it)"},
        {QuestionType::Grammar, "I ___ a great movie yesterday.", {"saw"}, {"see", "saw", "seen", "seeing"}, "Hint: Past Simple"},
        {QuestionType::Grammar, "They ___ playing football right now.", {"are"}, {"is", "are", "do", "does"}, "Hint: Present Continuous for plural"},
        {QuestionType::Grammar, "If I ___ you, I would study harder.", {"were"}, {"was", "were", "am", "be"}, "Hint: Second conditional idiom"},
        {QuestionType::Grammar, "She ___ finished her homework before her mom arrived.", {"had"}, {"has", "had", "have", "was"}, "Hint: Past Perfect"},
        {QuestionType::Grammar, "I have been ___ here for two hours.", {"waiting"}, {"wait", "waited", "waiting", "awaits"}, "Hint: Present Perfect Continuous"}
    };

    std::vector<Question> filteredQuestions;
    for (const auto& q : allQuestions) {
        if (filterType == 1 && q.type != QuestionType::Translation) continue;
        if (filterType == 2 && q.type != QuestionType::Grammar) continue;
        filteredQuestions.push_back(q);
    }
    
    std::random_device rd;
    std::mt19937 g(rd());
    std::shuffle(filteredQuestions.begin(), filteredQuestions.end(), g);

    if (count > static_cast<int>(filteredQuestions.size())) {
        count = filteredQuestions.size();
    }
    
    return std::vector<Question>(filteredQuestions.begin(), filteredQuestions.begin() + count);
}
