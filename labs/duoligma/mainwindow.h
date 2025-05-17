#pragma once

#include <QBuffer>
#include <QKeyEvent>
#include <QMainWindow>
#include <QTimer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

#include <cstdint>

using u32 = uint32_t;
using u64 = uint64_t;

enum Difficulty {
    Any,
    Low,
    High,
};

enum TaskType {
    Choose,
    Prompt,
    Audio,
};

struct Task
{
    TaskType type;
    bool correct;

    bool resp_provided;

    // for TaskType::Choose | TaskType::Prompt | TaskType::Audio
    QString prompt;

    // for TaskType::Choose
    std::vector<QString> variants;
    u32 correct_variant;
    u32 last_variant;

    // for TaskType::Prompt | TaskType::Audio
    QString correct_resp;
    QString last_resp;
    bool correct_resp_normalize;

    // for TaskType::Audio
    QByteArray prompt_audio;
};

struct TaskSet
{
    QString name;
    Difficulty level;
    QString help;
    u64 error_limit;
    u64 error_counter;
    u64 time_limit;
    u64 time_wasted;
    std::vector<Task> tasks;
};

template <class Value>
class ListItemWithValue : public QListWidgetItem
{
public:
    explicit ListItemWithValue(Value value, const QString text)
        : QListWidgetItem(text), value(std::move(value))
    {
    }
    Value value;
};

using ListItemTask = ListItemWithValue<Task>;

class SearchTasks : public QWidget
{
public:
    using ListItem = ListItemWithValue<TaskSet>;

    SearchTasks(const SearchTasks &) = delete;
    SearchTasks(SearchTasks &&) = delete;
    SearchTasks &operator=(const SearchTasks &) = delete;
    SearchTasks &operator=(SearchTasks &&) = delete;
    ~SearchTasks() override = default;

    explicit SearchTasks(QWidget *parent, std::function<void(u32)> callback);

    void reload(const std::vector<TaskSet> &tasks);

private:
    QListWidget *list;
    QLineEdit *line_edit;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);

    void keyPressEvent(QKeyEvent *event) override;

private slots:

signals:
    void help_requested();
    void enter_pressed();
    void ctrl_enter_pressed();
    void esc_pressed();

private:
    void save_task(Task &task);
    void show_task(const Task &task);
    void update_timer_label();
    void terminate_taskset();
    void update_total_score();
    void update_progress();
    bool check_show_time_limit();
    bool check_show_error_limit();
    void fetch_tasks();

    QStackedWidget *stack;
    QStackedWidget *task_stack;
    SearchTasks *select;

    QLabel *score_label;
    uint32_t total_score = 0;

    QWidget *choose_layer;
    QLabel *choose_prompt;
    QVBoxLayout *choose_layout;
    std::vector<QRadioButton *> choose_buttons;

    QLineEdit *prompt_resp;
    QLabel *prompt_prompt;

    QLabel *audio_prompt;
    QPushButton *audio_toggle;
    QLineEdit *audio_resp;

    QMediaPlayer *player;
    QAudioOutput *audio_output;
    QBuffer *buffer;

    QListWidget *list;
    QPushButton *terminate;
    QProgressBar *progress;
    QLabel *timer_label;
    QPushButton *submit;

    u32 current_taskset_id = 0;
    u32 current_task_id = 0;

    std::vector<TaskSet> task_sets;
};
