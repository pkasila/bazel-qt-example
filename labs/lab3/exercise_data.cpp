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
  };

  std::vector<TranslationTask> medium = {
      {"I love this city", "Я люблю этот город",
       "Express affection for a place"},
      {"Where is the library?", "Где библиотека?", "Asking for a location"},
      {"What time is it?", "Который час?", "Asking about time"},
      {"I am a student", "Я студент", "Introducing your occupation"},
      {"The weather is nice today", "Сегодня хорошая погода",
       "Talking about weather"},
      {"I want to eat", "Я хочу есть", "Expressing hunger"},
      {"How are you?", "Как дела?", "Asking about well-being"},
      {"My name is Anna", "Меня зовут Анна", "Introducing yourself"},
      {"I speak Russian", "Я говорю по-русски", "Talking about language"},
      {"This is very beautiful", "Это очень красиво",
       "Complimenting something"},
      {"Let's go home", "Пойдём домой", "Suggestion to go home"},
      {"I don't understand", "Я не понимаю", "Expressing confusion"},
  };

  std::vector<TranslationTask> hard = {
      {"Despite the rain, we went for a walk",
       "Несмотря на дождь, мы пошли гулять",
       "Despite = несмотря на + accusative"},
      {"She has been studying French for three years",
       "Она изучает французский уже три года",
       "Duration with уже + present tense"},
      {"If I had known, I would have come earlier",
       "Если бы я знал, я бы пришёл раньше",
       "Subjunctive mood: если бы + past tense"},
      {"The book that you recommended was very interesting",
       "Книга, которую вы порекомендовали, была очень интересной",
       "Relative clause with которую (accusative)"},
      {"We need to finish this project by Friday",
       "Нам нужно закончить этот проект к пятнице",
       "Need = нужно, by = к + dative"},
      {"He said that he would call tomorrow",
       "Он сказал, что позвонит завтра",
       "Reported speech with что"},
      {"I am glad that you came", "Я рад, что вы пришли",
       "Expression of gladness with что"},
      {"Could you help me, please?",
       "Не могли бы вы мне помочь?",
       "Polite request with не могли бы"},
      {"The train arrives at half past seven",
       "Поезд прибывает в половине восьмого",
       "Time: half past = в половине + genitive of next hour"},
      {"I have never been to Moscow",
       "Я никогда не был в Москве",
       "Double negation: никогда не"},
      {"She asked me to close the window",
       "Она попросила меня закрыть окно",
       "Ask someone to do = попросить + accusative + infinitive"},
      {"It seems to me that it is going to snow",
       "Мне кажется, что будет снег",
       "Impersonal: мне кажется, что..."},
      {"Whoever it was, they didn't leave a message",
       "Кто бы это ни был, он не оставил сообщения",
       "Whoever it was = Кто бы это ни был"},
      {"By the time you arrive, we will have already eaten",
       "К тому времени как вы приедете, мы уже поедим",
       "By the time = К тому времени как"},
      {"Only after reading the document did he understand the problem",
       "Только прочитав документ, он понял проблему",
       "Gerund construction for priority"},
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
      {"Я ___ студент.", {"есть", "быть", "—", "имею"}, 2,
       "In Russian, the copula is omitted in present tense."},
      {"Она ___ книгу.", {"читает", "читать", "читал", "читаю"}, 0,
       "3rd person singular, present tense of читать."},
      {"Мы ___ в школу.", {"идём", "идёт", "идти", "идут"}, 0,
       "1st person plural of идти."},
      {"Он ___ воду.", {"пьёт", "пить", "пьём", "пью"}, 0,
       "3rd person singular of пить."},
      {"Ты ___ музыку?", {"слушаешь", "слушает", "слушаю", "слушать"}, 0,
       "2nd person singular of слушать."},
      {"Кот ___ на диване.", {"сидит", "сидеть", "сидят", "сижу"}, 0,
       "3rd person singular of сидеть."},
      {"Я ___ кофе.", {"люблю", "любит", "любить", "любим"}, 0,
       "1st person singular of любить."},
      {"Они ___ в парке.", {"гуляют", "гулять", "гуляет", "гуляю"}, 0,
       "3rd person plural of гулять."},
  };

  std::vector<GrammarTask> medium = {
      {"Вчера я ___ в кино.", {"ходил", "хожу", "пойду", "иду"}, 0,
       "Past tense, masculine of ходить."},
      {"Она ___ письмо вчера.", {"написала", "напишет", "пишет", "писать"}, 0,
       "Past tense, perfective, feminine of написать."},
      {"Мы ___ эту книгу завтра.", {"прочитаем", "читаем", "читали", "читать"},
       0, "Future tense, perfective of прочитать."},
      {"Если бы я ___, я бы помог.", {"знал", "знаю", "узнаю", "знать"}, 0,
       "Subjunctive mood uses past tense."},
      {"Дети ___ в саду каждый день.",
       {"играют", "играли", "будут играть", "играть"}, 0,
       "Present tense for habitual actions."},
      {"Она ___ по-английски очень хорошо.",
       {"говорит", "говорить", "сказала", "скажет"}, 0,
       "Present tense, 3rd person of говорить."},
      {"Я ___ тебе завтра.", {"позвоню", "звоню", "звонил", "звонить"}, 0,
       "Future tense, perfective of позвонить."},
      {"Студенты ___ экзамен на прошлой неделе.",
       {"сдали", "сдают", "сдадут", "сдавать"}, 0,
       "Past tense, perfective of сдать."},
      {"Мы ___ здесь уже два часа.",
       {"сидим", "сидели", "посидим", "сидеть"}, 0,
       "Present tense for ongoing duration with уже."},
      {"Он ___ эту песню, когда был маленьким.",
       {"пел", "поёт", "споёт", "петь"}, 0,
       "Past tense, imperfective of петь."},
  };

  std::vector<GrammarTask> hard = {
      {"Книга, ___ ты мне дал, была интересной.",
       {"которую", "которая", "которой", "которым"}, 0,
       "Accusative case of которая (feminine direct object)."},
      {"Я пошёл ___ магазин.", {"в", "на", "к", "из"}, 0,
       "Direction towards an enclosed space uses в + accusative."},
      {"Мы говорили о ___, что случилось вчера.",
       {"том", "этом", "тот", "тем"}, 0,
       "о том, что — about what happened."},
      {"Она сказала, ___ придёт позже.",
       {"что", "чтобы", "как", "когда"}, 0,
       "Reported speech uses что."},
      {"Несмотря ___ трудности, он справился.",
       {"на", "в", "к", "от"}, 0,
       "Несмотря на + accusative = despite."},
      {"Я хочу, ___ ты пришёл вовремя.",
       {"чтобы", "что", "как", "когда"}, 0,
       "Desire that someone does something = чтобы + past tense."},
      {"Город, ___ мы жили, был очень красивым.",
       {"в котором", "который", "которому", "которым"}, 0,
       "Prepositional case: в котором (in which)."},
      {"Он уехал, не ___ ничего.",
       {"сказав", "говоря", "сказать", "говорить"}, 0,
       "Perfective gerund (деепричастие) of сказать."},
      {"Чем ___ мы начнём, тем лучше.",
       {"раньше", "позже", "быстрее", "скорее"}, 0,
       "Чем раньше ... тем лучше = the sooner ... the better."},
      {"Ему ___ было идти домой.",
       {"пора", "нужно", "надо", "можно"}, 0,
       "Пора = it's time (+ dative + было for past)."},
      {"Она вела себя так, ___ будто ничего не случилось.",
       {"как", "что", "чтобы", "когда"}, 0,
       "Как будто = as if."},
      {"Я не мог не ___ об этом.",
       {"думать", "подумать", "думаю", "думал"}, 0,
       "Не мочь не + infinitive = can't help but."},
      {"Я бы ___ эту задачу, если бы у меня было время.",
       {"решил", "решу", "решал", "решать"}, 0,
       "Subjunctive mood requires past tense verb."},
      {"Куда бы ты ни ___, я всегда буду с тобой.",
       {"пошёл", "пойдёшь", "пойти", "шёл"}, 0,
       "Concessive-conditional clause uses past tense after 'ни'."},
      {"Он был единственным, кто ___ на этот вопрос.",
       {"ответил", "ответить", "ответит", "отвечал"}, 0,
       "Relative pronoun 'кто' takes masculine singular past tense here."},
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
    // collapse multiple spaces
    result = result.simplified();
    return result;
  };
  return normalize(user_input) == normalize(expected);
}
