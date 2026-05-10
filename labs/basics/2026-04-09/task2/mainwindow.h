#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QDate>
#include <QMainWindow>
#include <QString>
#include <QVector>

class QCheckBox;
class QComboBox;
class QDateEdit;
class QLabel;
class QLineEdit;
class QListWidget;
class QListWidgetItem;
class QPlainTextEdit;
class QProgressBar;
class QPushButton;
class QSlider;
class QSpinBox;

class MainWindow : public QMainWindow {
   public:
    explicit MainWindow(QWidget* parent = nullptr);
    MainWindow(const MainWindow& other) = delete;
    MainWindow(MainWindow&& other) = delete;
    MainWindow& operator=(const MainWindow& other) = delete;
    MainWindow& operator=(MainWindow&& other) = delete;
    ~MainWindow() override = default;

   private:
    enum class TaskPriority : uint8_t {
        kLow = 0,
        kMedium = 1,
        kHigh = 2,
    };

    struct Task {
        QString title;
        QString description;
        QDate deadline;
        TaskPriority priority = TaskPriority::kMedium;
        int effort = 1;
        bool is_done = false;
        bool is_focus = false;
    };

    void OnAddTaskClicked();
    void OnRemoveTaskClicked();
    void OnTaskSelectionChanged();
    void OnTaskItemChanged(QListWidgetItem* item);
    void OnSaveTaskClicked();
    void OnDeadlineChanged();
    void OnPriorityChanged(int index);
    void OnEffortChanged(int value);
    void OnFocusModeChanged(int state);
    void OnCompletionChanged(int state);
    void OnFilterChanged(int index);
    void OnSortImportantClicked();
    void OnClearDoneClicked();

    void SetupUi();
    void SetupConnections();
    void RebuildTaskList();
    void UpdateTaskListItem(int index);
    void UpdateEditorFromCurrentTask();
    void ClearEditor();
    void UpdateStatistics();
    void SelectTask(int index);
    [[nodiscard]] int CurrentTaskIndex() const;
    [[nodiscard]] int CountImportantTasks() const;
    [[nodiscard]] int CountCompletedTasks() const;
    [[nodiscard]] bool IsTaskVisible(const Task& task) const;
    [[nodiscard]] QString BuildTaskTitle(const Task& task, int index) const;
    [[nodiscard]] QString PriorityToText(TaskPriority priority) const;

    QLineEdit* title_line_edit_ = nullptr;
    QPlainTextEdit* description_plain_text_edit_ = nullptr;
    QDateEdit* deadline_date_edit_ = nullptr;
    QComboBox* priority_combo_box_ = nullptr;
    QSpinBox* effort_spin_box_ = nullptr;
    QCheckBox* focus_check_box_ = nullptr;
    QCheckBox* done_check_box_ = nullptr;

    QListWidget* task_list_widget_ = nullptr;

    QPushButton* add_task_button_ = nullptr;
    QPushButton* remove_task_button_ = nullptr;
    QPushButton* save_task_button_ = nullptr;
    QPushButton* sort_important_button_ = nullptr;
    QPushButton* clear_done_button_ = nullptr;

    QComboBox* filter_combo_box_ = nullptr;
    QSlider* pareto_slider_ = nullptr;
    QLabel* pareto_value_label_ = nullptr;
    QLabel* summary_label_ = nullptr;
    QProgressBar* completion_progress_bar_ = nullptr;

    QVector<Task> tasks_;
    int current_task_index_ = -1;
};

#endif  // MAINWINDOW_H