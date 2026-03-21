# BSU Qt Project

В репозитории есть несколько небольших Qt-примеров и лабораторных заданий на Bazel.

## Актуальные задания

### Task 1: Ticket Trainer
- Код: `lab1_task1/`
- Запуск:

```bash
bazel run //lab1_task1:app
```

### Task 2: Habit Tracker
- Код: `lab1_task2/`
- Запуск:

```bash
bazel run //lab1_task2:task2
```

## Примечание
Если `bazel run` упрется в загрузку Qt-зависимостей, проверьте локальную конфигурацию `rules_qt` и установленный Qt в системе.
