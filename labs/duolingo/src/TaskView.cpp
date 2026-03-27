#include "TaskView.h"

#include "ExerciseTypes.h"
#include "GrammarView.h"
#include "Settings.h"
#include "TaskDifficulty.h"
#include "TranslationView.h"
#include "data_base.h"

#include <QLabel>
#include <QMessageBox>
#include <QProgressBar>
#include <QStackedWidget>
#include <QString>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <memory>

TaskView::TaskView(const std::shared_ptr<Settings>& settings, QWidget* parent)
    : QWidget(parent)
    , stacked_widget_(new QStackedWidget(this))
    , grammar_view_(new GrammarView(this))
    , translation_view_(new TranslationView(this))
    , progress_bar_(new QProgressBar(this))
    , exercise_timer_(new QTimer(this))
    , refresh_timer_(new QTimer(this))
    , timer_label_(new QLabel(QString("Осталось времени:...с"), this))
    , settings_(settings) {
    stacked_widget_->addWidget(grammar_view_);
    stacked_widget_->addWidget(translation_view_);

    auto* main_layout = new QVBoxLayout;

    auto* info_layout = new QHBoxLayout;
    info_layout->addWidget(progress_bar_);
    info_layout->addWidget(timer_label_);
    exercise_timer_->setTimerType(Qt::PreciseTimer);
    exercise_timer_->setSingleShot(true);
    connect(exercise_timer_, &QTimer::timeout, this, &TaskView::HandleTimeout);
    refresh_timer_->setTimerType(Qt::PreciseTimer);
    exercise_timer_->setInterval(kExerciseDuration);
    refresh_timer_->setInterval(10);
    connect(refresh_timer_, &QTimer::timeout, this, [this] {
        if (!exercise_timer_->isActive()) {
            timer_label_->setText(QString("Осталось времени:...с"));
            return;
        }
        const int time = exercise_timer_->remainingTime();
        const int sec = time / 1000;
        const int msec = (time - (sec * 1000)) / 10;
        timer_label_->setText(QString("Осталось времени:%1,%2с").arg(sec).arg(msec));
    });
    refresh_timer_->start();

    RefreshStats();

    main_layout->addLayout(info_layout);
    main_layout->addWidget(stacked_widget_);
    setLayout(main_layout);

    connect(grammar_view_, &GrammarView::Accepted, this, &TaskView::HandleAccepted);
    connect(grammar_view_, &GrammarView::WrongAnswer, this, &TaskView::HandleWrongAnswer);

    connect(translation_view_, &TranslationView::Accepted, this, &TaskView::HandleAccepted);
    connect(translation_view_, &TranslationView::WrongAnswer, this, &TaskView::HandleWrongAnswer);
}

void TaskView::HandleAccepted() {
    emit AC();
    progress_bar_->setValue(progress_bar_->value() + 1);
    LoadNext();
}

void TaskView::HandleWrongAnswer() {
    emit WA();
    QMessageBox::warning(
        this, QString("Неправильный ответ"), QString("Неправильный вариант ответа"));
    ++mistakes_;
    if (mistakes_ == kMistakes) {
        exercise_timer_->stop();
        refresh_timer_->stop();
        QMessageBox::warning(
            this, QString("Попытки закончились"),
            QString("Слишком много ошибок. Отдохни и возвращайся"));
        emit ExerciseChanged(static_cast<int>(ExerciseType::Chill));
    }
}

void TaskView::HandleTimeout() {
    emit WA();
    QMessageBox::warning(this, tr("Время вышло"), tr("Время на выполнение упражнения истекло"));
    exercise_timer_->stop();
    refresh_timer_->stop();
    emit ExerciseChanged(static_cast<int>(ExerciseType::Chill));
}

void TaskView::SetExercise(const ExerciseType& exercise) {
    FindTasks();
    if (exercise == ExerciseType::Grammar) {
        stacked_widget_->setCurrentWidget(grammar_view_);
        grammar_view_->SetTask(settings_->GetTaskDifficulty(), task_ids_[task_id_ind_]);
    } else {
        stacked_widget_->setCurrentWidget(translation_view_);
        translation_view_->SetTask(settings_->GetTaskDifficulty(), task_ids_[task_id_ind_]);
    }
    RefreshStats();
    exercise_timer_->start();
    exercise_type_ = exercise;
}

void TaskView::FindTasks() {
    task_ids_ = settings_->GetUnusedTasks(
        static_cast<int>(exercise_type_), static_cast<int>(settings_->GetTaskDifficulty()));
    task_id_ind_ = 0;
}

void TaskView::RefreshStats() {
    progress_bar_->setValue(0);
    progress_bar_->setMaximum(kTasksInSet);

    mistakes_ = 0;
}

void TaskView::LoadNext() {
    ++task_id_ind_;
    if (task_id_ind_ == kTasksInSet) {
        QMessageBox::information(this, QString("Победа"), QString("Задание выполнено!"));
        exercise_timer_->stop();
        refresh_timer_->stop();
        switch (settings_->GetTaskDifficulty()) {
            case TaskDifficulty::Easy:
                settings_->AddScore(100);
                break;
            case TaskDifficulty::Medium:
                settings_->AddScore(200);
                break;
            case TaskDifficulty::Hard:
                settings_->AddScore(300);
                break;
            default:;
        }
        for (const auto& id : task_ids_) {
            settings_->MarkTaskDone(
                static_cast<int>(exercise_type_), static_cast<int>(settings_->GetTaskDifficulty()),
                QString::number(id));
        }
        emit ExerciseChanged(static_cast<int>(ExerciseType::Chill));
        return;
    }
    if (exercise_type_ == ExerciseType::Grammar) {
        grammar_view_->SetTask(settings_->GetTaskDifficulty(), task_ids_[task_id_ind_]);
    } else {
        translation_view_->SetTask(settings_->GetTaskDifficulty(), task_ids_[task_id_ind_]);
    }
}