# Language Duel

Лабораторная работа №3 по Qt: приложение для изучения иностранного языка в духе Duolingo. Проект собирается через Bazel и лежит в `labs/basics/language_duel`.

## Запуск

```bash
cd /Users/mike/qtprojects/bazel-qt-example/labs/basics
bazel run //language_duel:language_duel
```

Если Bazel держит старый сервер или lock-файл:

```bash
bazel shutdown
rm -f MODULE.bazel.lock
bazel run //language_duel:language_duel
```

## Что закрыто по заданию

- `Layout`: все экраны собраны на `QHBoxLayout`/`QVBoxLayout`, окно нормально растягивается.
- `Dynamic widgets`: разные упражнения переключаются через `QStackedWidget`.
- `Menu`: есть `Training`, `Settings`, `Help`; сложность меняется через отдельный `QDialog`.
- `PushButton + TextEdit`: режим `Translation` принимает текстовый перевод и проверяет его по кнопке `Submit`.
- `PushButton + RadioBox`: режим `Grammar` показывает варианты через `QRadioButton`.
- `Progress bar`: прогресс показывает номер текущего задания из случайно выбранного набора.
- `Grading system`: баллы начисляются только после завершения всей серии, есть лимит ошибок.
- `Timer`: сессия завершается, если время истекло.
- `Help`: клавиша `H` открывает подсказку по текущему упражнению.
- `Design`: отдельная боковая панель, цветные статусы, карточки упражнений, итоговое окно статистики.

## Бонусные фичи

- Cat coach: в интерфейсе есть рисованный Qt-маскот Professor Cat, который меняет настроение и комментирует ответы.
- Resize-safe layout: боковая панель прокручивается в маленьком окне, кнопки, карточки и маскот имеют стабильные размеры; при растягивании окна расширяется рабочая область упражнения.
- Audio feedback + Sound Lab: правильный ответ, ошибка и таймер имеют разные системные сигналы; в `Labs -> Sound Lab...` есть кнопки `Success chime`, `Error thump`, `Timer panic`, `Combo beat`, `Meow Morse`, `Focus metronome` и переключатель звука.
- Advanced string matching: перевод сравнивается не простым `==`, а через нормализацию регистра, пунктуации, диакритики, `е/ё`, расстояние Левенштейна, покрытие слов и устойчивость к перестановке слов.
- Hybrid semantic similarity model: перевод может быть засчитан по смыслу, даже если слова отличаются; модель `local-hybrid-semantic-v3.0` использует нормализацию, repair mixed Latin/Cyrillic, translit, synonym/concept graph, polite-request handling, negation/gender/person guards и semantic confidence.
- Semantic AI Lab: через меню `Labs -> Semantic AI Lab` можно сравнить любые две фразы, увидеть semantic score, confidence, coverage, precision, matched/missing/extra concepts и guard-решение.
- Semantic probe: цель `//language_duel:semantic_similarity_probe` проверяет реальные парафразы по разным заданиям, например `Она любит зеленый чай`, `Oна любит зеленый чай`, `ona lyubit zelyony chay`, `Кошка находится под столом`, `Мы изучаем английский язык`, `Мой приятель проживает в Минске`, `Открой окно пожалуйста`.
- Randomized sessions: каждый старт выбирает новый набор заданий.
- Daily Cat Quest: отдельный ежедневный hard-mode челлендж с бонусными очками и daily streak.
- CatAI ML ensemble: локальная online logistic regression модель плюс kNN memory model предсказывают риск ошибки по сложности, режиму, прогрессу, времени, ошибкам, streak, подсказкам и daily challenge.
- CatAI Tutor Lab: через меню `Labs -> CatAI Tutor Lab` можно открыть окно с признаками, весами модели, количеством обучающих примеров, размером kNN-памяти, logistic risk, memory risk, ensemble risk и рекомендацией.
- Explainable AI layer: Tutor Lab показывает top feature attributions, то есть какие признаки сильнее всего подняли или снизили риск ошибки, почти как мини-SHAP для защиты лабораторной.
- Adaptive coach: Professor Cat анализирует текущий режим, время, ошибки, прогресс и CatAI risk, затем дает контекстный совет.
- Achievements: есть бейджи `First Win`, `Perfect Run`, `Hot Streak`, `Speed Finish`, `Hard Mode`, `Fuzzy Master`, `Scholar`, `Daily Cat`, `Three-Day Cat`, `Semantic AI`, `Semantic AI Pro`, `CatAI Trainee`, `CatAI Whisperer`, `CatAI Ensemble`, `Explainable CatAI`.
- Persistent progress: очки, лучший streak, daily streak, завершенные сессии, бейджи, веса CatAI и kNN-память сохраняются через `QSettings`.
- Review mistakes: ошибки текущей сессии можно посмотреть через меню `Training -> Review mistakes`.
- Ask Professor Cat: отдельный пункт меню с короткими забавными советами по прохождению.
- Combo/scoring: награда зависит от сложности, streak, скорости, точности и количества использованных подсказок.

## Структура

```text
basics/
├── BUILD
├── MODULE.bazel
├── README.md
├── WORKSPACE
└── language_duel/
    ├── BUILD.bazel
    ├── main.cpp
    ├── core/
    │   ├── cat_ai_model.h
    │   ├── cat_ai_model.cpp
    │   ├── models.h
    │   ├── practice_engine.h
    │   ├── practice_engine.cpp
    │   ├── semantic_similarity_model.h
    │   ├── semantic_similarity_model.cpp
    │   ├── semantic_similarity_model_test.cpp
    │   ├── text_matcher.h
    │   └── text_matcher.cpp
    └── ui/
        ├── difficulty_dialog.h
        ├── difficulty_dialog.cpp
        ├── exercise_pages.h
        ├── exercise_pages.cpp
        ├── main_window.h
        └── main_window.cpp
```

## Цель Bazel

```text
//language_duel:language_duel
```
