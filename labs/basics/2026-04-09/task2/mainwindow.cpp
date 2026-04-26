#include "mainwindow.h"

#include <ranges>

#include <QCheckBox>
#include <QComboBox>
#include <QDate>
#include <QDateEdit>
#include <QFont>
#include <QFormLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QPlainTextEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSignalBlocker>
#include <QSlider>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>

namespace {
QString TrimmedOrDefault(const QString& text, const QString& fallback) {
    QString trimmed = text.trimmed();
    if (trimmed.isEmpty()) {
        return fallback;
    }

    return trimmed;
}
}  // namespace

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    SetupUi();
    SetupConnections();
    UpdateStatistics();
    ClearEditor();
}

void MainWindow::SetupUi() {
    auto* central_widget = new QWidget(this);
    setCentralWidget(central_widget);
    setWindowTitle("Ежедневник по принципу Парето");
    resize(1100, 700);

    task_list_widget_ = new QListWidget(this);

    title_line_edit_ = new QLineEdit(this);
    description_plain_text_edit_ = new QPlainTextEdit(this);
    deadline_date_edit_ = new QDateEdit(this);
    priority_combo_box_ = new QComboBox(this);
    effort_spin_box_ = new QSpinBox(this);
    focus_check_box_ = new QCheckBox("Задача входит в фокус 20%", this);
    done_check_box_ = new QCheckBox("Задача выполнена", this);

    add_task_button_ = new QPushButton("Добавить задачу", this);
    remove_task_button_ = new QPushButton("Удалить задачу", this);
    save_task_button_ = new QPushButton("Сохранить изменения", this);
    sort_important_button_ = new QPushButton("Показать важные сначала", this);
    clear_done_button_ = new QPushButton("Удалить выполненные", this);

    filter_combo_box_ = new QComboBox(this);
    pareto_slider_ = new QSlider(Qt::Horizontal, this);
    pareto_value_label_ = new QLabel(this);
    summary_label_ = new QLabel(this);
    completion_progress_bar_ = new QProgressBar(this);

    priority_combo_box_->addItem("Низкий приоритет");
    priority_combo_box_->addItem("Средний приоритет");
    priority_combo_box_->addItem("Высокий приоритет");

    filter_combo_box_->addItem("Все задачи");
    filter_combo_box_->addItem("Только фокус 20%");
    filter_combo_box_->addItem("Только невыполненные");
    filter_combo_box_->addItem("Только выполненные");

    deadline_date_edit_->setCalendarPopup(true);
    deadline_date_edit_->setDate(QDate::currentDate());

    effort_spin_box_->setMinimum(1);
    effort_spin_box_->setMaximum(10);
    effort_spin_box_->setValue(1);

    pareto_slider_->setMinimum(10);
    pareto_slider_->setMaximum(50);
    pareto_slider_->setValue(20);
    pareto_value_label_->setText("Порог Парето: 20%");

    completion_progress_bar_->setMinimum(0);
    completion_progress_bar_->setMaximum(100);
    completion_progress_bar_->setValue(0);
    completion_progress_bar_->setFormat("Выполнено: 0%");

    summary_label_->setWordWrap(true);
    summary_label_->setText("Добавьте первую задачу и отметьте самые результативные 20%.");

    QFont list_font = task_list_widget_->font();
    list_font.setPointSize(list_font.pointSize() + 1);
    task_list_widget_->setFont(list_font);

    auto* controls_group_box = new QGroupBox("Редактор задачи", this);
    auto* controls_layout = new QFormLayout(controls_group_box);
    controls_layout->addRow("Название:", title_line_edit_);
    controls_layout->addRow("Описание:", description_plain_text_edit_);
    controls_layout->addRow("Дедлайн:", deadline_date_edit_);
    controls_layout->addRow("Приоритет:", priority_combo_box_);
    controls_layout->addRow("Усилие (1-10):", effort_spin_box_);
    controls_layout->addRow("", focus_check_box_);
    controls_layout->addRow("", done_check_box_);

    auto* editor_buttons_layout = new QHBoxLayout();
    editor_buttons_layout->addWidget(add_task_button_);
    editor_buttons_layout->addWidget(remove_task_button_);
    editor_buttons_layout->addWidget(save_task_button_);

    auto* filter_layout = new QHBoxLayout();
    filter_layout->addWidget(new QLabel("Фильтр:", this));
    filter_layout->addWidget(filter_combo_box_);
    filter_layout->addWidget(sort_important_button_);
    filter_layout->addWidget(clear_done_button_);

    auto* pareto_layout = new QHBoxLayout();
    pareto_layout->addWidget(new QLabel("Процент фокуса:", this));
    pareto_layout->addWidget(pareto_slider_);
    pareto_layout->addWidget(pareto_value_label_);

    auto* right_layout = new QVBoxLayout();
    right_layout->addLayout(filter_layout);
    right_layout->addWidget(task_list_widget_, 1);
    right_layout->addWidget(controls_group_box);
    right_layout->addLayout(editor_buttons_layout);
    right_layout->addLayout(pareto_layout);
    right_layout->addWidget(completion_progress_bar_);
    right_layout->addWidget(summary_label_);

    auto* root_layout = new QVBoxLayout(central_widget);
    root_layout->addLayout(right_layout);
}

void MainWindow::SetupConnections() {
    connect(add_task_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnAddTaskClicked);

    connect(remove_task_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnRemoveTaskClicked);

    connect(save_task_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnSaveTaskClicked);

    connect(task_list_widget_,
            &QListWidget::currentRowChanged,
            this,
            [this](const int /*row*/) { OnTaskSelectionChanged(); });

    connect(task_list_widget_,
            &QListWidget::itemChanged,
            this,
            &MainWindow::OnTaskItemChanged);

    connect(deadline_date_edit_,
            &QDateEdit::dateChanged,
            this,
            [this](const QDate& /*date*/) { OnDeadlineChanged(); });

    connect(priority_combo_box_,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::OnPriorityChanged);

    connect(effort_spin_box_,
            &QSpinBox::valueChanged,
            this,
            &MainWindow::OnEffortChanged);

    connect(focus_check_box_,
            &QCheckBox::checkStateChanged,
            this,
            &MainWindow::OnFocusModeChanged);

    connect(done_check_box_,
            &QCheckBox::checkStateChanged,
            this,
            &MainWindow::OnCompletionChanged);

    connect(filter_combo_box_,
            &QComboBox::currentIndexChanged,
            this,
            &MainWindow::OnFilterChanged);

    connect(sort_important_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnSortImportantClicked);

    connect(clear_done_button_,
            &QPushButton::clicked,
            this,
            &MainWindow::OnClearDoneClicked);

    connect(pareto_slider_,
            &QSlider::valueChanged,
            this,
            [this](const int value) {
                pareto_value_label_->setText(QString("Порог Парето: %1%").arg(value));
                UpdateStatistics();
            });
}

void MainWindow::OnAddTaskClicked() {
    Task task;
    task.title = QString("Новая задача %1").arg(tasks_.size() + 1);
    task.description = "";
    task.deadline = QDate::currentDate();
    task.priority = TaskPriority::kMedium;
    task.effort = 1;
    task.is_done = false;
    task.is_focus = false;

    tasks_.push_back(task);
    RebuildTaskList();
    SelectTask(tasks_.size() - 1);
    UpdateStatistics();
}

void MainWindow::OnRemoveTaskClicked() {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    tasks_.removeAt(index);
    current_task_index_ = -1;

    RebuildTaskList();
    if (!tasks_.isEmpty()) {
        SelectTask(0);
    } else {
        ClearEditor();
    }

    UpdateStatistics();
}

void MainWindow::OnTaskSelectionChanged() {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        current_task_index_ = -1;
        ClearEditor();
        return;
    }

    current_task_index_ = index;
    UpdateEditorFromCurrentTask();
}

void MainWindow::OnTaskItemChanged(QListWidgetItem* item) {
    if (item == nullptr) {
        return;
    }

    const int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    tasks_[index].is_done = item->checkState() == Qt::Checked;

    if (index == current_task_index_) {
        const QSignalBlocker blocker(done_check_box_);
        done_check_box_->setChecked(tasks_[index].is_done);
    }

    UpdateTaskListItem(index);
    UpdateStatistics();
}

void MainWindow::OnSaveTaskClicked() {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    Task& task = tasks_[index];
    task.title = TrimmedOrDefault(
        title_line_edit_->text(),
        QString("Задача %1").arg(index + 1));
    task.description = description_plain_text_edit_->toPlainText().trimmed();
    task.deadline = deadline_date_edit_->date();

    switch (priority_combo_box_->currentIndex()) {
        case 0:
            task.priority = TaskPriority::kLow;
            break;
        case 2:
            task.priority = TaskPriority::kHigh;
            break;
        case 1:
        default:
            task.priority = TaskPriority::kMedium;
            break;
    }

    task.effort = effort_spin_box_->value();
    task.is_focus = focus_check_box_->isChecked();
    task.is_done = done_check_box_->isChecked();

    RebuildTaskList();
    SelectTask(index);
    UpdateStatistics();
}

void MainWindow::OnDeadlineChanged() {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    tasks_[index].deadline = deadline_date_edit_->date();
    UpdateTaskListItem(index);
    UpdateStatistics();
}

void MainWindow::OnPriorityChanged(const int index) {
    const int task_index = CurrentTaskIndex();
    if (task_index < 0 || task_index >= tasks_.size()) {
        return;
    }

    switch (index) {
        case 0:
            tasks_[task_index].priority = TaskPriority::kLow;
            break;
        case 2:
            tasks_[task_index].priority = TaskPriority::kHigh;
            break;
        case 1:
        default:
            tasks_[task_index].priority = TaskPriority::kMedium;
            break;
    }

    UpdateTaskListItem(task_index);
    UpdateStatistics();
}

void MainWindow::OnEffortChanged(const int value) {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    tasks_[index].effort = value;
    UpdateTaskListItem(index);
    UpdateStatistics();
}

void MainWindow::OnFocusModeChanged(const int state) {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    tasks_[index].is_focus = state == Qt::Checked;
    UpdateTaskListItem(index);
    UpdateStatistics();
}

void MainWindow::OnCompletionChanged(const int state) {
    const int index = CurrentTaskIndex();
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    tasks_[index].is_done = state == Qt::Checked;
    UpdateTaskListItem(index);
    UpdateStatistics();
}

void MainWindow::OnFilterChanged(const int /*index*/) {
    RebuildTaskList();
    UpdateStatistics();
}

void MainWindow::OnSortImportantClicked() {
    std::ranges::stable_sort(tasks_.begin(), tasks_.end(), [](const Task& left, const Task& right) {
        if (left.is_focus != right.is_focus) {
            return left.is_focus && !right.is_focus;
        }

        if (left.priority != right.priority) {
            return static_cast<int>(left.priority) > static_cast<int>(right.priority);
        }

        if (left.is_done != right.is_done) {
            return !left.is_done && right.is_done;
        }

        return left.deadline < right.deadline;
    });

    RebuildTaskList();
    if (!tasks_.isEmpty()) {
        SelectTask(0);
    }

    UpdateStatistics();
}

void MainWindow::OnClearDoneClicked() {
    QVector<Task> remaining_tasks;
    remaining_tasks.reserve(tasks_.size());

    for (const Task& task : tasks_) {
        if (!task.is_done) {
            remaining_tasks.push_back(task);
        }
    }

    tasks_ = remaining_tasks;
    current_task_index_ = -1;

    RebuildTaskList();
    if (!tasks_.isEmpty()) {
        SelectTask(0);
    } else {
        ClearEditor();
    }

    UpdateStatistics();
}

void MainWindow::RebuildTaskList() {
    task_list_widget_->clear();

    for (int index = 0; index < tasks_.size(); ++index) {
        if (!IsTaskVisible(tasks_[index])) {
            continue;
        }

        auto* item = new QListWidgetItem(BuildTaskTitle(tasks_[index], index));
        item->setData(Qt::UserRole, index);
        item->setFlags(item->flags() | Qt::ItemIsUserCheckable | Qt::ItemIsSelectable |
                       Qt::ItemIsEnabled);
        item->setCheckState(tasks_[index].is_done ? Qt::Checked : Qt::Unchecked);
        task_list_widget_->addItem(item);
    }
}

void MainWindow::UpdateTaskListItem(const int /*index*/) {
    RebuildTaskList();

    if (current_task_index_ >= 0 && current_task_index_ < tasks_.size()) {
        SelectTask(current_task_index_);
    }
}

void MainWindow::UpdateEditorFromCurrentTask() {
    if (current_task_index_ < 0 || current_task_index_ >= tasks_.size()) {
        ClearEditor();
        return;
    }

    const Task& task = tasks_[current_task_index_];

    title_line_edit_->setText(task.title);
    description_plain_text_edit_->setPlainText(task.description);
    deadline_date_edit_->setDate(task.deadline);

    {
        const QSignalBlocker blocker(priority_combo_box_);
        switch (task.priority) {
            case TaskPriority::kLow:
                priority_combo_box_->setCurrentIndex(0);
                break;
            case TaskPriority::kMedium:
                priority_combo_box_->setCurrentIndex(1);
                break;
            case TaskPriority::kHigh:
                priority_combo_box_->setCurrentIndex(2);
                break;
        }
    }

    {
        const QSignalBlocker blocker(effort_spin_box_);
        effort_spin_box_->setValue(task.effort);
    }

    {
        const QSignalBlocker blocker(focus_check_box_);
        focus_check_box_->setChecked(task.is_focus);
    }

    {
        const QSignalBlocker blocker(done_check_box_);
        done_check_box_->setChecked(task.is_done);
    }
}

void MainWindow::ClearEditor() {
    title_line_edit_->clear();
    description_plain_text_edit_->clear();
    deadline_date_edit_->setDate(QDate::currentDate());

    {
        const QSignalBlocker blocker(priority_combo_box_);
        priority_combo_box_->setCurrentIndex(1);
    }

    {
        const QSignalBlocker blocker(effort_spin_box_);
        effort_spin_box_->setValue(1);
    }

    {
        const QSignalBlocker blocker(focus_check_box_);
        focus_check_box_->setChecked(false);
    }

    {
        const QSignalBlocker blocker(done_check_box_);
        done_check_box_->setChecked(false);
    }
}

void MainWindow::UpdateStatistics() {
    const int total_count = tasks_.size();
    const int completed_count = CountCompletedTasks();
    const int important_count = CountImportantTasks();

    int percent = 0;
    if (total_count > 0) {
        percent = (completed_count * 100) / total_count;
    }

    completion_progress_bar_->setValue(percent);
    completion_progress_bar_->setFormat(
        QString("Выполнено: %1 из %2 (%3%)")
            .arg(completed_count)
            .arg(total_count)
            .arg(percent));

    const int pareto_percent = pareto_slider_->value();
    const int recommended_focus_count = (total_count * pareto_percent + 99) / 100;

    summary_label_->setText(
        QString("Всего задач: %1. В фокусе: %2. По правилу Парето при пороге %3% "
                "рекомендуется держать в фокусе около %4 задач.")
            .arg(total_count)
            .arg(important_count)
            .arg(pareto_percent)
            .arg(recommended_focus_count));
}

void MainWindow::SelectTask(const int index) {
    if (index < 0 || index >= tasks_.size()) {
        return;
    }

    current_task_index_ = index;

    for (int row = 0; row < task_list_widget_->count(); ++row) {
        QListWidgetItem* item = task_list_widget_->item(row);
        if (item == nullptr) {
            continue;
        }

        if (item->data(Qt::UserRole).toInt() == index) {
            task_list_widget_->setCurrentRow(row);
            UpdateEditorFromCurrentTask();
            return;
        }
    }

    UpdateEditorFromCurrentTask();
}

int MainWindow::CurrentTaskIndex() const {
    QListWidgetItem* item = task_list_widget_->currentItem();
    if (item == nullptr) {
        return -1;
    }

    return item->data(Qt::UserRole).toInt();
}

int MainWindow::CountImportantTasks() const {
    int count = 0;

    for (const Task& task : tasks_) {
        if (task.is_focus) {
            ++count;
        }
    }

    return count;
}

int MainWindow::CountCompletedTasks() const {
    int count = 0;

    for (const Task& task : tasks_) {
        if (task.is_done) {
            ++count;
        }
    }

    return count;
}

bool MainWindow::IsTaskVisible(const Task& task) const {
    switch (filter_combo_box_->currentIndex()) {
        case 1:
            return task.is_focus;
        case 2:
            return !task.is_done;
        case 3:
            return task.is_done;
        case 0:
        default:
            return true;
    }
}

QString MainWindow::BuildTaskTitle(const Task& task, const int index) const {
    const QString focus_mark = task.is_focus ? "[20%] " : "";
    const QString done_mark = task.is_done ? "✓ " : "";
    return QString("%1%2%3 | %4 | до %5 | усилие %6")
        .arg(done_mark)
        .arg(focus_mark)
        .arg(TrimmedOrDefault(task.title, QString("Задача %1").arg(index + 1)))
        .arg(PriorityToText(task.priority))
        .arg(task.deadline.toString("dd.MM.yyyy"))
        .arg(task.effort);
}

QString MainWindow::PriorityToText(const TaskPriority priority) const {
    switch (priority) {
        case TaskPriority::kLow:
            return "низкий";
        case TaskPriority::kMedium:
            return "средний";
        case TaskPriority::kHigh:
            return "высокий";
    }

    return "средний";
}