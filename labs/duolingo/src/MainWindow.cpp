#include "MainWindow.h"

#include "AudioPlayer.h"
#include "ChangeDIfficultyDialog.h"
#include "ImageView.h"
#include "Settings.h"
#include "Sound.h"
#include "TaskDifficulty.h"
#include "TaskView.h"

#include <QComboBox>
#include <QFile>
#include <QKeySequence>
#include <QMainWindow>
#include <QMenuBar>
#include <QMessageBox>
#include <QShortcut>
#include <QStackedWidget>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , stacked_widget_(new QStackedWidget)
    , settings_(new Settings)
    , image_view_(new ImageView(kDuolingoPicPath))
    , task_view_(new TaskView(settings_, this))
    , difficulty_label_(new QLabel)
    , score_label_(new QLabel)
    , exercise_combo_box_(new QComboBox)
    , audio_player_(new AudioPlayer) {
    auto* central = new QWidget;
    setCentralWidget(central);
    setWindowTitle("Дуолинго");

    stacked_widget_->addWidget(image_view_);
    stacked_widget_->addWidget(task_view_);
    SetChill();

    auto* main_layout = new QVBoxLayout;

    auto* menu_bar = new QMenuBar;

    auto* settings_menu = new QMenu(QString("Настройки"));
    const auto* change_difficulty_action = settings_menu->addAction(QString("Выбрать сложность"));
    connect(change_difficulty_action, &QAction::triggered, this, [this]() {
        ChangeDIfficultyDialog(settings_, this).exec();
    });

    auto* help_menu = new QMenu(QString("Помощь"));
    const auto* help_action = help_menu->addAction(QString("Помощь"));
    connect(help_action, &QAction::triggered, this, &MainWindow::ShowHelpMessage);

    const auto* help_shortcut = new QShortcut(QKeySequence("Ctrl+H"), this);
    connect(help_shortcut, &QShortcut::activated, this, &MainWindow::ShowHelpMessage);

    menu_bar->addMenu(settings_menu);
    menu_bar->addMenu(help_menu);
    main_layout->setMenuBar(menu_bar);

    auto* info_layout = new QHBoxLayout;

    auto* exercise_label = new QLabel(QString("Режим:"));
    exercise_combo_box_->addItem("Чилл");
    exercise_combo_box_->addItem("Грамматика");
    exercise_combo_box_->addItem("Перевод");
    connect(
        exercise_combo_box_, &QComboBox::currentIndexChanged, this, &MainWindow::ExerciseChanged);
    info_layout->addWidget(exercise_label);
    info_layout->addWidget(exercise_combo_box_);
    info_layout->addStretch(1);

    difficulty_label_->setText(
        QString("Сложность: %1").arg(AsRussianLiteral(settings_->GetTaskDifficulty())));
    connect(
        settings_.get(), &Settings::DifficultyChanged, this,
        [this](const TaskDifficulty difficulty) {
            difficulty_label_->setText(QString("Сложность: %1").arg(AsRussianLiteral(difficulty)));
            ExerciseChanged(exercise_combo_box_->currentIndex());
        });
    info_layout->addWidget(difficulty_label_);

    info_layout->addStretch(1);

    score_label_->setText(QString("Очков: %1").arg(settings_->GetScore()));
    connect(settings_.get(), &Settings::ScoreChanged, this, [this](const int score) {
        score_label_->setText(QString("Очков: %1").arg(score));
    });
    info_layout->addWidget(score_label_);

    main_layout->addLayout(info_layout);

    main_layout->addWidget(stacked_widget_);

    connect(task_view_, &TaskView::ExerciseChanged, this, &MainWindow::ExerciseChanged);
    connect(task_view_, &TaskView::WA, this, [this] { audio_player_->PlaySound(Sound::Fail); });
    connect(task_view_, &TaskView::AC, this, [this] { audio_player_->PlaySound(Sound::Success); });

    auto* clear_stats_shortcut = new QShortcut(QKeySequence("Meta+R"), this);
    connect(clear_stats_shortcut, &QShortcut::activated, this, [this] {
        settings_->ClearDoneTasks();
        settings_->SetScore(0);
        ExerciseChanged(exercise_combo_box_->currentIndex());
    });

    central->setLayout(main_layout);
}

void MainWindow::ShowHelpMessage() {
    QMessageBox::information(
        this, QString("Помощь"),
        [this] -> QString {
            ExerciseType exercise = static_cast<ExerciseType>(exercise_combo_box_->currentIndex());
            switch (exercise) {
                case ExerciseType::Chill:
                    return {"Отдохните под lofi"};
                case ExerciseType::Grammar:
                    return QString(
                               "Нужно выбрать правильно 1 из 4 вариантов, чтобы подставить его на "
                               "пустое место. \n"
                               "Максимум ошибок: %1")
                        .arg(kMistakes);
                case ExerciseType::Translation:
                    return QString(
                               "Надо перевести текст с английского на русский. На легком уровне - "
                               "одно слово, на среднем - предложение, на сложном - пословицу. \n"
                               "Максимум ошибок: %1")
                        .arg(kMistakes);
                default:;
            }
            return {};
        }(),
        QMessageBox::Ok);
}

void MainWindow::ExerciseChanged(const int& new_exercise_id) const {
    const auto exercise = static_cast<ExerciseType>(new_exercise_id);
    exercise_combo_box_->setCurrentIndex(new_exercise_id);
    if (exercise == ExerciseType::Chill) {
        SetChill();
    } else {
        SetTask(exercise);
    }
}

void MainWindow::SetChill() const {
    stacked_widget_->setCurrentWidget(image_view_);
    audio_player_->PlaySound(Sound::Chill);
}

void MainWindow::SetTask(const ExerciseType exercise) const {
    stacked_widget_->setCurrentWidget(task_view_);
    task_view_->SetExercise(exercise);
    audio_player_->Shutdown();
}
