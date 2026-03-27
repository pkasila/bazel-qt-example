QT       += core gui widgets sql

TARGET = LanguageAppDuolingo
TEMPLATE = app

# Определяем версию C++ (минимум C++17 для некоторых возможностей Qt 6)
CONFIG += c++17

# Исходные файлы
SOURCES += \
    src/main.cpp \
    src/app/Application.cpp \
    src/app/SettingsManager.cpp \
    src/core/ExerciseGenerator.cpp \
    src/core/ScoringSystem.cpp \
    src/core/TimerLogic.cpp \
    src/db/DatabaseManager.cpp \
    src/db/ExerciseRepository.cpp \
    src/db/StatisticsRepository.cpp \
    src/ui/MainWindow.cpp \
    src/ui/DifficultyDialog.cpp \
    src/ui/HelpDialog.cpp \
    src/ui/ExerciseViewBase.cpp \
    src/ui/TranslationExerciseWidget.cpp \
    src/ui/GrammarExerciseWidget.cpp \
    src/ui/StatisticsViewWidget.cpp

# Заголовочные файлы
HEADERS += \
    src/app/Application.h \
    src/app/SettingsManager.h \
    src/models/ExerciseData.h \
    src/core/Exercise.h \
    src/core/ExerciseGenerator.h \
    src/core/ScoringSystem.h \
    src/core/TimerLogic.h \
    src/db/DatabaseManager.h \
    src/db/ExerciseRepository.h \
    src/db/StatisticsRepository.h \
    src/ui/MainWindow.h \
    src/ui/DifficultyDialog.h \
    src/ui/HelpDialog.h \
    src/ui/ExerciseViewBase.h \
    src/ui/TranslationExerciseWidget.h \
    src/ui/GrammarExerciseWidget.h \
    src/ui/StatisticsViewWidget.h

# Файлы ресурсов
RESOURCES += \
    resources/resources.qrc

# Для macOS, чтобы приложение выглядело как стандартное приложение
# MAC_BUNDLE_DATA.files = path/to/your/database.sqlite
# MAC_BUNDLE_DATA.path = Contents/Resources
# Это если бы база данных была отдельным файлом, который нужно скопировать.
# В нашем случае база создается в AppDataLocation, так что это не обязательно для БД.

# Указываем, где искать заголовочные файлы (если они не в стандартных путях)
INCLUDEPATH += src

# Если используете Qt 5, замените CONFIG += c++17 на CONFIG += c++11 или c++14 (в зависимости от вашей версии Qt 5)
# Для Qt 5 также может потребоваться TARGET = $$qtLibraryTarget($$TARGET)
# Если вы используете Qt 5, то некоторые части кода (например, QRandomGenerator::global()) могут потребовать адаптации.
# Этот .pro файл больше ориентирован на Qt 6.

# Опционально: для сборки в режиме release по умолчанию (если не указано qmake CONFIG+=debug)
# CONFIG += release

# Опционально: для вывода большего количества предупреждений компилятора
# QMAKE_CXXFLAGS += -Wall -Wextra
