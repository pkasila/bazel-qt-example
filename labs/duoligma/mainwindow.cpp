#include "mainwindow.h"

#include <QBuffer>
#include <QIODevice>
#include <QKeyEvent>
#include <QTimer>
#include <QtMultimedia/QAudioOutput>
#include <QtMultimedia/QMediaPlayer>
#include <QtSql/QSql>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlDriver>
#include <QtSql/QSqlError>
#include <QtSql/QSqlQuery>
#include <QtWidgets/QComboBox>
#include <QtWidgets/QGroupBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMessageBox>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QSplitter>
#include <QtWidgets/QStackedWidget>
#include <QtWidgets/QVBoxLayout>

#include <cassert>
#include <cmath>
#include <functional>
#include <ranges>
#include <utility>
#include <vector>

class ImageWidget : public QLabel
{
public:
    explicit ImageWidget(QWidget *parent = nullptr) : QLabel(parent)
    {
        this->setAlignment(Qt::AlignCenter);
    }

    void setPixmap(const QPixmap &p)
    {
        pix = p;
        updatePixmap();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        updatePixmap();
        QLabel::resizeEvent(event);
    }

private:
    QPixmap pix;

    void updatePixmap()
    {
        if (pix.isNull()) {
            return;
        }
        auto dim = pix.scaled(this->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QLabel::setPixmap(dim);
    }
};

SearchTasks::SearchTasks(QWidget *parent, std::function<void(u32)> callback) : QWidget(parent)
{
    auto *vertical_layout = new QVBoxLayout();
    auto *horizontal_layout = new QHBoxLayout();

    line_edit = new QLineEdit(this);
    line_edit->setPlaceholderText("Search task");

    horizontal_layout->addWidget(line_edit);

    auto *spacer = new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Minimum);
    horizontal_layout->addSpacerItem(spacer);

    auto *label = new QLabel(this);
    label->setText("Difficulty");

    horizontal_layout->addWidget(label);

    auto *difficulty = new QComboBox(this);
    difficulty->addItem("Any");
    difficulty->addItem("Low");
    difficulty->addItem("High");
    difficulty->setCurrentIndex(0);

    horizontal_layout->addWidget(difficulty);

    list = new QListWidget(this);
    list->setSortingEnabled(false);

    connect(difficulty, &QComboBox::currentIndexChanged, this,
            [this]() { emit line_edit->textEdited(line_edit->text()); });

    connect(list, &QListWidget::itemDoubleClicked, this,
            [callback = std::move(callback), this](QListWidgetItem *item) {
                callback(list->row(item));
            });

    connect(line_edit, &QLineEdit::textEdited, this, [this, difficulty](const QString &pat) {
        bool found = false;
        auto level = static_cast<Difficulty>(difficulty->currentIndex());
        for (auto i : std::views::iota(0, list->count())) {
            auto &task = dynamic_cast<ListItem *>(list->item(i))->value;
            bool item_found = false;
            if (level == Difficulty::Any || level == task.level) {
                item_found = task.name.contains(pat, Qt::CaseInsensitive);
            }
            list->item(static_cast<int>(i))->setHidden(!item_found);
            found |= item_found;
        }
        auto p = QGuiApplication::palette();
        if (!found) {
            p.setColor(QPalette::ColorRole::Text, Qt::red);
        }
        this->line_edit->setPalette(p);
    });

    vertical_layout->addLayout(horizontal_layout);
    vertical_layout->addWidget(list);

    this->setLayout(vertical_layout);
}

void SearchTasks::reload(const std::vector<TaskSet> &tasks)
{
    list->clear();
    for (const auto &set : tasks) {
        auto text = QString("%1").arg(set.name);
        u32 completed = 0;
        for (const auto &task : set.tasks) {
            if (task.correct) {
                completed += 1;
            }
        }
        if (completed == 0) {
            // text.append(QString(" (%1)").arg(set.tasks.size()));
        } else {
            text.append(QString(" (%1/%2)").arg(completed).arg(set.tasks.size()));
        }
        list->addItem(new ListItem(set, text));
    }
    emit line_edit->textChanged(line_edit->text());
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_H || event->key() == Qt::Key_F1) {
        emit this->help_requested();
    } else if (event->key() == Qt::Key_Enter || event->key() == Qt::Key_Return) {
        emit this->enter_pressed();
    } else if (event->key() == Qt::Key_Escape) {
        emit this->esc_pressed();
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    this->setObjectName("Duoligma");
    this->resize(650, 700);
    this->setMouseTracking(false);

    fetch_tasks();

    auto *central = new QWidget(this);
    auto *verticalLayout = new QVBoxLayout(central);

    stack = new QStackedWidget(this);

    score_label = new QLabel(this);

    task_stack = new QStackedWidget();

    {
        auto *layer = new QWidget(this);
        auto *vertical = new QVBoxLayout(layer);

        auto *label = new QLabel(layer);
        label->setText("Select task");
        label->setAlignment(Qt::AlignCenter);

        vertical->addWidget(label);

        layer->setLayout(vertical);
        task_stack->addWidget(layer);
    }

    choose_layer = new QWidget(this);
    choose_prompt = new QLabel(choose_layer);
    choose_layout = new QVBoxLayout(choose_layer);

    auto pad_vertical = [](auto *widget) {
        auto *layer = new QWidget();
        auto *vertical = new QVBoxLayout(layer);

        vertical->addWidget(widget);
        vertical->addSpacerItem(
                new QSpacerItem(20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding));
        layer->setLayout(vertical);
        return layer;
    };

    {
        choose_layout->addWidget(choose_prompt);

        auto *box_layout = new QVBoxLayout(choose_layer);

        auto *box = new QGroupBox(choose_layer);
        box->setLayout(box_layout);
        choose_layout->addWidget(box);

        choose_layer->setLayout(choose_layout);

        choose_layout = box_layout;

        task_stack->addWidget(pad_vertical(choose_layer));
    }

    prompt_prompt = new QLabel();
    prompt_resp = new QLineEdit();
    prompt_resp->setPlaceholderText("Reponse");

    {
        auto *layer = new QWidget(this);
        auto *vertical = new QVBoxLayout(layer);

        vertical->addWidget(prompt_prompt);
        vertical->addWidget(prompt_resp);

        layer->setLayout(vertical);
        task_stack->addWidget(pad_vertical(layer));
    }

    audio_prompt = new QLabel();
    audio_toggle = new QPushButton();
    audio_resp = new QLineEdit();
    audio_resp->setPlaceholderText("Reponse");

    {
        auto *layer = new QWidget(this);
        auto *vertical = new QVBoxLayout(layer);

        auto *horizontal_layer = new QWidget(this);
        auto *horizontal = new QHBoxLayout(layer);

        horizontal->addWidget(audio_prompt);
        horizontal->addWidget(audio_toggle);
        horizontal->addSpacerItem(
                new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

        horizontal_layer->setLayout(horizontal);
        vertical->addWidget(horizontal_layer);

        vertical->addWidget(audio_resp);

        layer->setLayout(vertical);
        task_stack->addWidget(pad_vertical(layer));
    }

    player = new QMediaPlayer();
    audio_output = new QAudioOutput();
    player->setAudioOutput(audio_output);
    buffer = new QBuffer();

    connect(audio_toggle, &QPushButton::pressed, this, [this]() {
        if (player->isPlaying()) {
            player->pause();
        } else {
            player->play();
        }
    });

    connect(player, &QMediaPlayer::playbackStateChanged, this,
            [this](QMediaPlayer::PlaybackState state) {
                if (state == QMediaPlayer::PlayingState) {
                    audio_toggle->setIcon(style()->standardIcon(QStyle::SP_MediaPause));
                } else {
                    audio_toggle->setIcon(style()->standardIcon(QStyle::SP_MediaPlay));
                }
            });

    auto *splitter = new QSplitter(this);

    auto *menu = new QWidget(this);
    auto *vertical = new QVBoxLayout(menu);

    terminate = new QPushButton(menu);
    terminate->setText("Terminate");
    vertical->addWidget(terminate);

    list = new QListWidget(menu);
    vertical->addWidget(list);

    connect(list, &QListWidget::currentRowChanged, this, [this](int row) {
        if (row < 0) {
            return;
        }
        if (task_stack->currentIndex() != 0) {
            auto old_row = current_task_id;
            auto &task = task_sets[current_taskset_id].tasks[old_row];
            save_task(task);
        }

        this->current_task_id = row;
        const auto &task = task_sets[current_taskset_id].tasks[row];
        show_task(task);
    });

    progress = new QProgressBar(menu);
    progress->setValue(1);
    progress->setMinimum(0);
    progress->setMaximum(20);
    progress->setTextVisible(true);
    progress->setOrientation(Qt::Orientation::Horizontal);
    progress->setFormat("%v/%m");
    vertical->addWidget(progress);

    auto p = menu->sizePolicy();
    p.setVerticalPolicy(QSizePolicy::Expanding);
    p.setHorizontalPolicy(QSizePolicy::Expanding);

    p.setHorizontalStretch(1);
    menu->setSizePolicy(p);
    menu->setLayout(vertical);

    splitter->addWidget(menu);
    splitter->setCollapsible(0, false);

    auto *task_widget = new QWidget(splitter);
    auto *task_widget_layout = new QVBoxLayout(splitter);

    timer_label = new QLabel(splitter);

    auto *timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, [this]() {
        if (stack->currentIndex() != 1) {
            return;
        }
        update_timer_label();
        auto &set = task_sets[current_taskset_id];
        auto &time = set.time_wasted;
        time += 1;

        if (check_show_time_limit()) {
            terminate_taskset();
        }
    });
    // Consider pausing while hidden
    timer->setInterval(1000);
    timer->start();

    submit = new QPushButton(splitter);
    submit->setText("Submit");
    auto *task_buttons = new QWidget(splitter);

    {
        auto *task_buttons_layout = new QHBoxLayout(splitter);

        task_buttons_layout->addSpacerItem(
                new QSpacerItem(20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));

        task_buttons_layout->addWidget(timer_label);

        task_buttons_layout->addWidget(submit);
        task_buttons->setLayout(task_buttons_layout);
    }

    task_widget_layout->addWidget(task_buttons);
    task_widget_layout->addWidget(task_stack);

    task_widget->setLayout(task_widget_layout);

    p.setHorizontalStretch(4);
    task_widget->setSizePolicy(p);

    splitter->addWidget(task_widget);
    splitter->setCollapsible(1, false);

    splitter->refresh();
    auto split_factor = 4;
    int w = this->size().width();
    splitter->setSizes({ w / split_factor, w * (split_factor - 1) / split_factor });

    select = new SearchTasks(this, [this](u32 row) {
        this->current_taskset_id = row;
        this->current_task_id = 0;

        if (check_show_error_limit()) {
            return;
        }
        if (check_show_time_limit()) {
            return;
        }
        update_progress();
        list->clear();
        auto &set = task_sets[current_taskset_id];
        auto &tasks = set.tasks;
        for (const auto i : std::views::iota(0UL, tasks.size())) {
            list->addItem(QString("Task %1").arg(i + 1));
            QBrush brush = QGuiApplication::palette().text();
            if (tasks[i].correct) {
                brush.setColor(Qt::green);
            }
            list->item(static_cast<int>(i))->setForeground(brush);
        }
        task_stack->setCurrentIndex(0);
        stack->setCurrentIndex(1);
        update_timer_label();
    });

    connect(submit, &QPushButton::pressed, this, [this]() {
        if (stack->currentIndex() != 1) {
            assert(false);
        }
        auto &set = task_sets[current_taskset_id];
        auto &task = set.tasks[current_task_id];

        bool got_answer = false;
        bool correct = false;
        switch (task.type) {
        case TaskType::Choose: {
            auto active_id = 0;
            for (auto *button : choose_buttons) {
                if (button->isChecked()) {
                    break;
                }
                active_id += 1;
            }
            if (active_id >= choose_buttons.size()) {
                break;
            }
            got_answer = true;
            if (active_id == task.correct_variant) {
                correct = true;
            }
        } break;
        case TaskType::Prompt:
        case TaskType::Audio: {
            auto resp = QString();
            if (task.type == TaskType::Prompt) {
                resp = prompt_resp->text();
            } else if (task.type == TaskType::Audio) {
                resp = audio_resp->text();
            } else {
                assert(false);
            }
            if (resp == "") {
                break;
            }
            got_answer = true;
            auto mode = QString::NormalizationForm_D;
            auto left = resp;
            auto right = task.correct_resp;
            if (task.correct_resp_normalize) {
                left = left.normalized(mode).simplified();
                right = right.normalized(mode).simplified();
            }
            if (left.compare(right, Qt::CaseInsensitive) == 0) {
                correct = true;
            }
        } break;
        default:
            assert(false);
        }
        QBrush brush = QGuiApplication::palette().text();
        if (got_answer) {
            if (correct) {
                brush.setColor(Qt::green);
            } else {
                brush.setColor(Qt::red);
                set.error_counter += 1;
                if (check_show_error_limit()) {
                    terminate_taskset();
                }
            }
        }
        list->item(static_cast<int>(current_task_id))->setForeground(brush);
        task.correct = got_answer && correct;

        update_progress();

        auto cur_id = current_task_id;
        auto next_id = (cur_id + 1) % set.tasks.size();
        while (set.tasks[next_id].correct) {
            if (cur_id == next_id) {
                terminate_taskset();
                return;
            }
            next_id = (next_id + 1) % set.tasks.size();
        }
        list->setCurrentRow(static_cast<int>(next_id));
    });

    connect(this, &MainWindow::help_requested, this, [this]() {
        if (stack->currentIndex() != 1) {
            return;
        }
        QMessageBox msg;
        msg.setWindowTitle("Help");
        const auto &set = task_sets[current_taskset_id];
        auto help = set.help;
        if (help == "") {
            help = "No help message for this kind of tasks";
        }
        if (task_stack->currentIndex() != 0) {
            const auto &task = set.tasks[current_task_id];
            help.append("\n\nCorrect answer: ");
            switch (task.type) {
            case TaskType::Choose: {
                help.append(task.variants.at(task.correct_variant));
            } break;
            case TaskType::Prompt:
            case TaskType::Audio: {
                help.append(task.correct_resp);
            } break;
            }
        }
        msg.setText(help);
        msg.exec();
    });

    connect(this, &MainWindow::enter_pressed, this, [this]() {
        if (stack->currentIndex() != 1) {
            return;
        }
        emit submit->pressed();
    });

    connect(this, &MainWindow::esc_pressed, this, [this]() {
        if (stack->currentIndex() != 1) {
            return;
        }
        emit terminate->pressed();
    });

    connect(terminate, &QPushButton::pressed, this, [this]() {
        terminate_taskset(); //
    });

    update_total_score();
    select->reload(task_sets);

    {
        auto *layer = new QWidget(this);
        auto *vertical = new QVBoxLayout(layer);

        auto s = QSizePolicy();
        s.setVerticalPolicy(QSizePolicy::Expanding);
        s.setHorizontalPolicy(QSizePolicy::Expanding);
        // s.setVerticalStretch(1);
        s.setHorizontalStretch(10);

        auto *image = new ImageWidget(layer);
        image->setMinimumSize(400, 250);
        image->setPixmap(QPixmap(":/resources/images/logo.png"));
        image->setSizePolicy(s);
        image->setAlignment(Qt::AlignCenter);

        vertical->addWidget(image);

        auto *horizontal_layer = new QWidget(layer);
        auto *horizontal = new QHBoxLayout(layer);

        s.setVerticalPolicy(QSizePolicy::Minimum);
        s.setHorizontalPolicy(QSizePolicy::Expanding);
        // s.setVerticalStretch(10);
        s.setHorizontalStretch(4);

        horizontal->addStretch(1);

        select->setSizePolicy(s);
        horizontal->addWidget(select);

        auto l = QSizePolicy();
        l.setVerticalPolicy(QSizePolicy::Expanding);
        l.setHorizontalPolicy(QSizePolicy::Expanding);
        l.setVerticalStretch(0);
        l.setHorizontalStretch(1);

        score_label->setAlignment(Qt::AlignBottom | Qt::AlignRight);
        score_label->setSizePolicy(l);

        horizontal->addWidget(score_label);

        horizontal_layer->setLayout(horizontal);
        vertical->addWidget(horizontal_layer);

        layer->setLayout(vertical);
        stack->addWidget(layer);
    }
    stack->addWidget(splitter);
    stack->setCurrentIndex(0);

    verticalLayout->addWidget(stack);

    this->setLayout(verticalLayout);

    this->setCentralWidget(central);

    this->setWindowTitle("Duoligma");

    QMetaObject::connectSlotsByName(this);
}

void MainWindow::save_task(Task &task)
{
    switch (task.type) {
    case TaskType::Choose: {
        u32 i = 0;
        task.last_variant = 0;
        task.resp_provided = false;
        for (auto *button : choose_buttons) {
            if (button->isChecked()) {
                task.last_variant = i;
                task.resp_provided = true;
                break;
            }
            i += 1;
        }
    } break;
    case TaskType::Prompt: {
        task.last_resp = prompt_resp->text();
        task.resp_provided = task.last_resp != "";
    } break;
    case TaskType::Audio: {
        task.last_resp = audio_resp->text();
        task.resp_provided = task.last_resp != "";
    } break;
    default: {
        qDebug() << task.type;
        assert(false);
    }
    }
}

void MainWindow::show_task(const Task &task)
{
    switch (task.type) {
    case TaskType::Choose: {
        choose_prompt->setText(task.prompt);
        for (auto *button : choose_buttons) {
            choose_layout->removeWidget(button);
            delete button;
        }
        choose_buttons.clear();
        for (const auto &variant : task.variants) {
            auto *button = new QRadioButton(variant);
            choose_layout->addWidget(button);
            choose_buttons.push_back(button);
        }
        if (task.resp_provided) {
            choose_buttons.at(task.last_variant)->setChecked(true);
        }
        task_stack->setCurrentIndex(1);
        choose_buttons.at(0)->setFocus();
    } break;
    case TaskType::Prompt: {
        prompt_prompt->setText(task.prompt);
        auto resp = QString("");
        if (task.resp_provided) {
            resp = task.last_resp;
        }
        prompt_resp->setText(resp);
        task_stack->setCurrentIndex(2);
        prompt_resp->setFocus();
    } break;
    case TaskType::Audio: {
        player->stop();
        buffer->close();
        // NOLINTNEXTLINE(cppcoreguidelines-pro-type-const-cast)
        buffer->setBuffer(const_cast<QByteArray *>(&task.prompt_audio));
        buffer->open(QIODevice::ReadOnly);
        player->setSourceDevice(buffer);
        audio_toggle->setIcon(this->style()->standardIcon(QStyle::SP_MediaPlay));

        audio_prompt->setText(task.prompt);
        auto resp = QString("");
        if (task.resp_provided) {
            resp = task.last_resp;
        }
        audio_resp->setText(resp);
        task_stack->setCurrentIndex(3);
        audio_resp->setFocus();
    } break;
    default: {
        qDebug() << task.type;
        assert(false);
    }
    }
}

void MainWindow::update_timer_label()
{
    auto &time = task_sets[current_taskset_id].time_wasted;
    timer_label->setText(QString("Spent: %1:%2")
                                 .arg(static_cast<int>(time / 60), 2, 10, QChar('0'))
                                 .arg(static_cast<int>(time % 60), 2, 10, QChar('0')));
}

void MainWindow::terminate_taskset()
{
    if (task_stack->currentIndex() == 1) {
        auto old_row = current_task_id;
        auto &task = task_sets[current_taskset_id].tasks[old_row];
        save_task(task);
    }
    player->stop();
    update_total_score();
    select->reload(task_sets);
    stack->setCurrentIndex(0);
    task_stack->setCurrentIndex(0);
}

void MainWindow::update_total_score()
{
    auto score = 0;
    for (const auto &set : task_sets) {
        bool all_correct = true;
        for (const auto &task : set.tasks) {
            if (!task.correct) {
                all_correct = false;
                break;
            }
        }
        if (all_correct) {
            score += static_cast<int>(set.tasks.size());
        }
    }
    total_score = score;
    score_label->setText(QString("Score: %1").arg(total_score));
}

void MainWindow::update_progress()
{
    auto count_correct = 0;
    const auto &tasks = task_sets[current_taskset_id].tasks;
    for (const auto &task : tasks) {
        if (task.correct) {
            count_correct += 1;
        }
    }
    progress->setMaximum(static_cast<int>(tasks.size()));
    progress->setValue(count_correct);
}

bool MainWindow::check_show_time_limit()
{
    auto &set = task_sets[current_taskset_id];
    if (set.time_limit != 0 && set.time_wasted >= set.time_limit) {
        QMessageBox msg;
        msg.setText("Oops. Time limit reached");
        msg.exec();
        return true;
    }
    return false;
}

bool MainWindow::check_show_error_limit()
{
    auto &set = task_sets[current_taskset_id];
    if (set.error_limit != 0 && set.error_counter >= set.error_limit) {
        QMessageBox msg;
        msg.setText("Oops. Error limit reached");
        msg.exec();
        return true;
    }
    return false;
}

void MainWindow::fetch_tasks()
{
    auto db = QSqlDatabase::addDatabase("QSQLITE");
    db.setConnectOptions("QSQLITE_OPEN_READONLY;QSQLITE_USE_QT_VFS");
    db.setDatabaseName(":/resources/sqlite/data.db");
    if (!db.open()) {
        QMessageBox msg;
        msg.setText(QString("Failed to open db: %1").arg(db.lastError().text()));
        msg.exec();
        return;
    }

    auto sets = QSqlQuery("SELECT id, name, level, help, time_limit, error_limit "
                          "FROM TaskSets",
                          db);
    while (sets.next()) {
        auto set = TaskSet{};
        int set_id = sets.value(0).toInt();
        set.name = sets.value(1).toString();
        set.level = static_cast<Difficulty>(sets.value(2).toInt());
        set.help = sets.value(3).toString();
        set.time_limit = sets.value(4).toInt();
        set.error_limit = sets.value(5).toInt();

        auto tasks = QSqlQuery(db);
        tasks.prepare("SELECT id, type, prompt, prompt_audio, variants, "
                      "correct_variant, correct_resp, correct_resp_normalize "
                      "FROM Tasks WHERE task_set_id = (:set_id)");
        tasks.bindValue(":set_id", set_id);

        if (!tasks.exec()) {
            QMessageBox msg;
            msg.setText(QString("Failed to fetch tasks: %1").arg(tasks.lastError().text()));
            msg.exec();
            return;
        }

        while (tasks.next()) {
            Task task{};
            task.type = static_cast<TaskType>(tasks.value(1).toInt());
            task.prompt = tasks.value(2).toString();
            task.prompt_audio = tasks.value(3).toByteArray();
            auto variants = tasks.value(4).toString();
            for (const auto &v : variants.split(";;")) {
                task.variants.push_back(v);
            }
            task.correct_variant = tasks.value(5).toInt();
            task.correct_resp = tasks.value(6).toString();
            task.correct_resp_normalize = tasks.value(7).toBool();

            set.tasks.push_back(task);
        }

        task_sets.push_back(set);
    }
}
