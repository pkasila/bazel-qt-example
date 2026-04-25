#include "exercise_data.h"

#include <algorithm>
#include <random>

DifficultyParams GetDifficultyParams(Difficulty difficulty) {
    switch (difficulty) {
        case Difficulty::Easy:
            return {5, 120, 5, 10};
        case Difficulty::Medium:
            return {8, 90, 3, 20};
        case Difficulty::Hard:
            return {10, 60, 2, 30};
    }
    return {5, 120, 5, 10};
}

std::vector<TranslationTask> GetTranslationPool(Difficulty difficulty) {
    std::vector<TranslationTask> easy = {
      {"Hello", "Привет", "Informal greeting"},
      {"Good morning", "Доброе утро", "Morning greeting"},
      {"Thank you", "Спасибо", "Expression of gratitude"},
      {"Goodbye", "До свидания", "Formal farewell"},
      {"Yes", "Да", "Affirmation"},
      {"No", "Нет", "Negation"},
      {"Please", "Пожалуйста", "Polite request word"},
      {"Water", "Вода", "H2O"},
      {"Cat", "Кот", "Small domestic animal"},
      {"Dog", "Собака", "Man's best friend"},
      {"House", "Дом", "A building where people live"},
      {"Book", "Книга", "You read it"},
      {"Apple", "Яблоко", "A common fruit"},
      {"Red", "Красный", "Color of blood"},
      {"Blue", "Синий", "Color of the sky"},
      {"Green", "Зеленый", "Color of grass"},
      {"One", "Один", "Number 1"},
      {"Two", "Два", "Number 2"},
      {"Three", "Три", "Number 3"},
      {"Friend", "Друг", "A person you like"},
      {"Mother", "Мать", "Female parent"},
      {"Father", "Отец", "Male parent"},
      {"Boy", "Мальчик", "Young male"},
      {"Girl", "Девочка", "Young female"},
      {"Sun", "Солнце", "Star in our sky"},
      {"Moon", "Луна", "Earth's satellite"},
      {"Car", "Машина", "A four-wheeled vehicle"},
      {"Tree", "Дерево", "Large plant with a trunk"},
      {"Flower", "Цветок", "A plant blossom"},
      {"Table", "Стол", "Furniture for eating or working"},
      {"Chair", "Стул", "Furniture for sitting"},
      {"Window", "Окно", "Glass opening in a wall"},
      {"Door", "Дверь", "Entryway"},
      {"Pen", "Ручка", "Writing instrument"},
      {"Pencil", "Карандаш", "Wooden writing instrument"},
      {"Day", "День", "Opposite of night"},
      {"Night", "Ночь", "Opposite of day"},
      {"Milk", "Молоко", "White liquid from cows"},
      {"Bread", "Хлеб", "Baked food"},
      {"Fish", "Рыба", "Aquatic animal"}
    };

    std::vector<TranslationTask> medium = {
      {"I love this city", "Я люблю этот город", "Express affection for a place"},
      {"Where is the library?", "Где библиотека?", "Asking for a location"},
      {"What time is it?", "Который час?", "Asking about time"},
      {"I am a student", "Я студент", "Introducing your occupation"},
      {"The weather is nice today", "Сегодня хорошая погода", "Talking about weather"},
      {"I want to eat", "Я хочу есть", "Expressing hunger"},
      {"How are you?", "Как дела?", "Asking about well-being"},
      {"My name is Anna", "Меня зовут Анна", "Introducing yourself"},
      {"I speak Russian", "Я говорю по-русски", "Talking about language"},
      {"This is very beautiful", "Это очень красиво", "Complimenting something"},
      {"Let's go home", "Пойдём домой", "Suggestion to go home"},
      {"I don't understand", "Я не понимаю", "Expressing confusion"},
      {"Where are you from?", "Откуда ты?", "Asking about origin"},
      {"Can you help me?", "Можешь мне помочь?", "Asking for assistance"},
      {"I like listening to music", "Мне нравится слушать музыку", "Expressing a hobby"},
      {"How much does it cost?", "Сколько это стоит?", "Asking for a price"},
      {"I need to buy a ticket", "Мне нужно купить билет", "Planning a trip"},
      {"What are you doing?", "Что ты делаешь?", "Asking about current activity"},
      {"I have a question", "У меня есть вопрос", "Seeking information"},
      {"See you tomorrow", "До завтра", "Saying goodbye until next day"},
      {"I am very tired", "Я очень устал", "Expressing fatigue"},
      {"She is my sister", "Она моя сестра", "Introducing a family member"},
      {"He works at a bank", "Он работает в банке", "Talking about occupation"},
      {"We live in Moscow", "Мы живем в Москве", "Talking about residence"},
      {"Are you busy today?", "Ты сегодня занят?", "Checking availability"},
      {"It is raining", "Идет дождь", "Describing weather"},
      {"I forgot my password", "Я забыл свой пароль", "Common IT problem"},
      {"Do you have a pet?", "У тебя есть питомец?", "Asking about animals"},
      {"I usually wake up at 7", "Я обычно просыпаюсь в 7", "Describing daily routine"},
      {"Let's watch a movie", "Давай посмотрим фильм", "Suggesting an activity"}
    };

    std::vector<TranslationTask> hard = {
      {"Despite the rain, we went for a walk", "Несмотря на дождь, мы пошли гулять", "Despite = несмотря на + accusative"},
      {"She has been studying French for three years", "Она изучает французский уже три года", "Duration with уже + present tense"},
      {"If I had known, I would have come earlier", "Если бы я знал, я бы пришёл раньше", "Subjunctive mood: если бы + past tense"},
      {"The book that you recommended was very interesting", "Книга, которую вы порекомендовали, была очень интересной", "Relative clause with которую (accusative)"},
      {"We need to finish this project by Friday", "Нам нужно закончить этот проект к пятнице", "Need = нужно, by = к + dative"},
      {"He said that he would call tomorrow", "Он сказал, что позвонит завтра", "Reported speech with что"},
      {"I am glad that you came", "Я рад, что вы пришли", "Expression of gladness with что"},
      {"Could you help me, please?", "Не могли бы вы мне помочь?", "Polite request with не могли бы"},
      {"The train arrives at half past seven", "Поезд прибывает в половине восьмого", "Time: half past = в половине + genitive of next hour"},
      {"I have never been to Moscow", "Я никогда не был в Москве", "Double negation: никогда не"},
      {"She asked me to close the window", "Она попросила меня закрыть окно", "Ask someone to do = попросить + accusative + infinitive"},
      {"It seems to me that it is going to snow", "Мне кажется, что будет снег", "Impersonal: мне кажется, что..."},
      {"The faster we start, the sooner we will finish", "Чем быстрее мы начнем, тем скорее закончим", "Comparative correlation"},
      {"I was about to leave when he called", "Я собирался уходить, когда он позвонил", "Interrupted action in the past"},
      {"There is no doubt that they will win the match", "Нет никаких сомнений, что они выиграют матч", "Expressing certainty"},
      {"She takes after her mother in both appearance and character", "Она похожа на свою мать и внешне, и по характеру", "Idiomatic comparison"},
      {"Had it not been for his help, I would have failed", "Если бы не его помощь, я бы потерпел неудачу", "Third conditional inversion"},
      {"It is highly likely that the event will be postponed", "Весьма вероятно, что мероприятие будет отложено", "Expressing probability"},
      {"He is not used to waking up so early", "Он не привык просыпаться так рано", "Habits with 'used to'"},
      {"The scenery was so breathtaking that we stopped to take photos", "Пейзаж был настолько захватывающим, что мы остановились сделать фотографии", "Result clause with so... that"},
      {"By the time you arrive, I will have cooked dinner", "К тому времени, как ты приедешь, я уже приготовлю ужин", "Future perfect"},
      {"She complained of being treated unfairly", "Она жаловалась на то, что с ней обращаются несправедливо", "Gerund after preposition"},
      {"I'd rather you didn't smoke in here", "Я бы предпочел, чтобы вы здесь не курили", "Preference with 'rather'"},
      {"He is rumored to have resigned from his position", "Ходят слухи, что он ушел в отставку со своей должности", "Passive reporting verb"}
    };

    switch (difficulty) {
        case Difficulty::Easy:
            return easy;
        case Difficulty::Medium:
            return medium;
        case Difficulty::Hard:
            return hard;
    }
    return easy;
}

std::vector<GrammarTask> GetGrammarPool(Difficulty difficulty) {
    std::vector<GrammarTask> easy = {
      {"I ___ a student.", {"am", "is", "are", "be"}, 0, "First person singular uses 'am'."},
      {"She ___ a book every day.", {"reads", "read", "reading", "is reading"}, 0, "Present simple for daily habits."},
      {"We ___ to school together.", {"go", "goes", "going", "went"}, 0, "Plural subject takes the base verb."},
      {"He ___ water.", {"drinks", "drink", "drinking", "drunk"}, 0, "Third person singular adds 's'."},
      {"___ you like music?", {"Do", "Does", "Are", "Is"}, 0, "Use 'Do' for second person questions."},
      {"The cat ___ on the sofa.", {"sits", "sit", "sitting", "is sit"}, 0, "Third person singular adds 's'."},
      {"I ___ coffee.", {"love", "loves", "loving", "am love"}, 0, "First person singular uses the base verb."},
      {"They ___ in the park.", {"walk", "walks", "walking", "are walk"}, 0, "Plural subject takes the base verb."},
      {"There ___ an apple on the table.", {"is", "are", "am", "be"}, 0, "Singular noun uses 'is'."},
      {"There ___ two birds in the tree.", {"are", "is", "am", "be"}, 0, "Plural noun uses 'are'."},
      {"___ is your name?", {"What", "Who", "Where", "When"}, 0, "Use 'What' to ask for things."},
      {"___ old are you?", {"How", "What", "Who", "Where"}, 0, "Age is asked with 'How'."},
      {"My brother ___ playing football.", {"is", "are", "am", "do"}, 0, "Present continuous for third person singular."},
      {"They ___ not like cheese.", {"do", "does", "are", "is"}, 0, "Negative present simple uses 'do not' for plural."},
      {"She ___ not have a car.", {"does", "do", "is", "has"}, 0, "Negative present simple uses 'does not' for singular."},
      {"Look! The bus ___ coming.", {"is", "are", "does", "do"}, 0, "Present continuous for actions happening now."},
      {"___ you swimming right now?", {"Are", "Do", "Is", "Does"}, 0, "Present continuous questions use 'Are' for 'you'."},
      {"He has two ___.", {"children", "childs", "child", "childrens"}, 0, "Irregular plural of child."},
      {"I need some ___.", {"water", "waters", "a water", "two waters"}, 0, "Water is uncountable."},
      {"This is ___ umbrella.", {"an", "a", "the", "-"}, 0, "Use 'an' before a vowel sound."},
      {"She drives ___ fast.", {"very", "much", "many", "a lot"}, 0, "Use 'very' to intensify adjectives/adverbs."},
      {"It is ___ hot today.", {"too", "much", "many", "a"}, 0, "Use 'too' for excessive degree."},
      {"I can ___ English.", {"speak", "speaks", "to speak", "speaking"}, 0, "Modals are followed by base verb."},
      {"You must ___ quietly.", {"listen", "to listen", "listening", "listens"}, 0, "'Must' is followed by base verb."}
    };

    std::vector<GrammarTask> medium = {
      {"Yesterday I ___ to the cinema.", {"went", "go", "will go", "am going"}, 0, "Past tense of 'go' is 'went'."},
      {"She ___ a letter yesterday.", {"wrote", "writes", "will write", "write"}, 0, "Past tense of 'write' is 'wrote'."},
      {"We ___ this book tomorrow.", {"will read", "read", "reading", "readed"}, 0, "Future tense uses 'will'."},
      {"If I ___, I would have helped.", {"had known", "knew", "know", "will know"}, 0, "Third conditional uses past perfect."},
      {"The children ___ in the garden every day.", {"play", "played", "will play", "playing"}, 0, "Present simple for habits."},
      {"She ___ English very well.", {"speaks", "speak", "is speaking", "spoken"}, 0, "Third person singular present simple."},
      {"I ___ you tomorrow.", {"will call", "call", "called", "am call"}, 0, "Future action uses 'will'."},
      {"The students ___ the exam last week.", {"passed", "pass", "will pass", "passes"}, 0, "Past simple for completed actions."},
      {"We ___ here for two hours.", {"have been sitting", "are sitting", "sit", "sat"}, 0, "Present perfect continuous for ongoing actions."},
      {"He ___ this song when he was little.", {"sang", "sings", "will sing", "sing"}, 0, "Past simple of 'sing' is 'sang'."},
      {"I ___ my keys. I can't find them.", {"have lost", "lost", "lose", "am losing"}, 0, "Present perfect for past events with present results."},
      {"She is ___ than her sister.", {"taller", "tallest", "more tall", "tall"}, 0, "Comparative adjectives add '-er'."},
      {"This is the ___ movie I've ever seen.", {"best", "better", "good", "most good"}, 0, "Superlative of 'good' is 'best'."},
      {"He ___ be at home, his car is outside.", {"must", "can", "should", "might"}, 0, "Deduction using 'must'."},
      {"You ___ smoke in a hospital.", {"must not", "don't have to", "needn't", "couldn't"}, 0, "Prohibition uses 'must not'."},
      {"I was reading a book when the phone ___.", {"rang", "ring", "was ringing", "has rung"}, 0, "Interrupted action in the past."},
      {"They ___ dinner at 8 PM yesterday.", {"were having", "had", "have", "have had"}, 0, "Past continuous for specific time in the past."},
      {"I am interested ___ learning Spanish.", {"in", "on", "at", "about"}, 0, "Interested takes the preposition 'in'."},
      {"She is good ___ playing tennis.", {"at", "in", "on", "with"}, 0, "Good takes the preposition 'at'."},
      {"He apologized ___ being late.", {"for", "about", "of", "to"}, 0, "Apologize takes the preposition 'for'."},
      {"I look forward ___ meeting you.", {"to", "for", "at", "on"}, 0, "Look forward takes the preposition 'to'."},
      {"Unless you ___, you will miss the train.", {"hurry", "will hurry", "hurried", "don't hurry"}, 0, "First conditional with 'unless' (if not)."},
      {"I ___ a shower when the water went cold.", {"was taking", "took", "take", "am taking"}, 0, "Past continuous interrupted action."},
      {"He told me that he ___ tired.", {"was", "is", "has been", "will be"}, 0, "Reported speech shifts tense back."}
    };

    std::vector<GrammarTask> hard = {
      {"The book ___ you gave me was interesting.", {"which", "who", "whom", "whose"}, 0, "Use 'which' or 'that' for things."},
      {"I went ___ the store.", {"to", "in", "at", "on"}, 0, "Direction uses 'to'."},
      {"We talked about ___ happened yesterday.", {"what", "that", "which", "who"}, 0, "Use 'what' as a relative pronoun for things."},
      {"She said ___ she would come later.", {"that", "what", "which", "when"}, 0, "Reported speech uses 'that'."},
      {"___ the difficulties, he succeeded.", {"Despite", "In spite", "Although", "Even"}, 0, "'Despite' does not take 'of'."},
      {"I want you ___ on time.", {"to come", "come", "coming", "came"}, 0, "'Want someone to do something'."},
      {"The city ___ we lived was very beautiful.", {"where", "which", "that", "when"}, 0, "Use 'where' for locations."},
      {"He left without ___ anything.", {"saying", "say", "said", "to say"}, 0, "Prepositions are followed by the -ing form."},
      {"The ___, the better.", {"sooner", "soon", "soonest", "more soon"}, 0, "Comparative structure: the + comparative."},
      {"It was time for him ___ home.", {"to go", "going", "go", "went"}, 0, "'Time for someone to do something'."},
      {"She acted ___ nothing had happened.", {"as if", "like if", "even if", "if"}, 0, "'As if' is used for hypothetical situations."},
      {"I couldn't help ___ about it.", {"thinking", "to think", "think", "thought"}, 0, "'Can't help' is followed by the -ing form."},
      {"Hardly ___ the house when it started raining.", {"had I left", "I had left", "did I left", "I left"}, 0, "Inversion after 'Hardly'."},
      {"Not only ___ the exam, but he also got the highest score.", {"did he pass", "he passed", "passed he", "he did pass"}, 0, "Inversion after 'Not only'."},
      {"If I ___ you, I wouldn't do that.", {"were", "was", "am", "have been"}, 0, "Subjunctive mood in second conditional."},
      {"I suggest that she ___ a doctor.", {"see", "sees", "saw", "will see"}, 0, "Subjunctive mood after 'suggest'."},
      {"By next month, I ___ here for ten years.", {"will have worked", "will work", "have worked", "work"}, 0, "Future perfect for duration up to a future point."},
      {"The manager had the secretary ___ the report.", {"type", "to type", "typing", "typed"}, 0, "Causative 'have' takes the bare infinitive."},
      {"I got my hair ___ yesterday.", {"cut", "to cut", "cutting", "cutted"}, 0, "Causative 'get' for services takes past participle."},
      {"He is known ___ a great athlete in his youth.", {"to have been", "to be", "being", "having been"}, 0, "Perfect infinitive for past states."},
      {"___ having studied hard, he failed the test.", {"Despite", "Although", "Even though", "However"}, 0, "'Despite' is followed by a noun phrase or gerund."},
      {"I regret ___ you that the flight has been cancelled.", {"to inform", "informing", "inform", "informed"}, 0, "'Regret to inform' is standard for bad news."},
      {"I clearly remember ___ the door.", {"locking", "to lock", "lock", "locked"}, 0, "'Remember doing' is for past memories."},
      {"It's high time we ___.", {"left", "leave", "leaving", "will leave"}, 0, "'High time' takes the past tense."}
    };

    switch (difficulty) {
        case Difficulty::Easy:
            return easy;
        case Difficulty::Medium:
            return medium;
        case Difficulty::Hard:
            return hard;
    }
    return easy;
}

template <typename T>
static std::vector<T> SampleN(const std::vector<T>& pool, int n) {
    std::vector<T> shuffled = pool;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::shuffle(shuffled.begin(), shuffled.end(), gen);
    int count = std::min(n, static_cast<int>(shuffled.size()));
    shuffled.resize(count);
    return shuffled;
}

std::vector<TranslationTask> SampleTranslationTasks(Difficulty difficulty) {
    auto params = GetDifficultyParams(difficulty);
    return SampleN(GetTranslationPool(difficulty), params.task_count);
}

std::vector<GrammarTask> SampleGrammarTasks(Difficulty difficulty) {
    auto params = GetDifficultyParams(difficulty);
    return SampleN(GetGrammarPool(difficulty), params.task_count);
}

bool CheckTranslation(const QString& user_input, const QString& expected) {
    auto normalize = [](const QString& s) {
        QString result = s.trimmed().toLower();
        result.remove(QChar('.'));
        result.remove(QChar(','));
        result.remove(QChar('!'));
        result.remove(QChar('?'));
        result.remove(QChar(';'));
        result.remove(QChar(':'));
        result = result.simplified();
        return result;
    };
    return normalize(user_input) == normalize(expected);
}
