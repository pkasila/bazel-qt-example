#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QListWidget>
#include <QMessageBox>
#include <QDialog>
#include <QCalendarWidget>
#include <QTimeEdit>
#include <QLineEdit>
#include <QTextEdit>
#include <QDialogButtonBox>
#include <QLabel>
#include <algorithm>
#include <QCheckBox>
#include <QTimer>
#include <QFileDialog>
#include <QFile>
#include <QDataStream>

//NOLINTBEGIN
MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    , ui_(new Ui::MainWindow)
{
    reminder_timer_ = new QTimer(this);
    connect(reminder_timer_, &QTimer::timeout, this, &MainWindow::checkReminders);
    reminder_timer_->start(60000);
    this->setWindowTitle("Органайзер");
    QWidget* central_widget = new QWidget(this);
    this->setCentralWidget(central_widget);
    QVBoxLayout* main_layout = new QVBoxLayout(central_widget);
    QHBoxLayout* top_layout = new QHBoxLayout();
    tasks_list_widget_ = new QListWidget();
    tasks_list_widget_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    top_layout->addWidget(tasks_list_widget_, 7);
    QVBoxLayout* control_button_layout = new QVBoxLayout();
    QPushButton* add_task_button = new QPushButton("Добавить задачу", this);
    QPushButton* remove_task_button = new QPushButton("Удалить задачу", this);
    control_button_layout->addWidget(add_task_button);
    control_button_layout->addWidget(remove_task_button);
    top_layout->addLayout(control_button_layout, 3);
    main_layout->addLayout(top_layout);
    QHBoxLayout* bottom_layout = new QHBoxLayout();
    QPushButton* save_button = new QPushButton("Сохранить", this);
    QPushButton* load_button = new QPushButton("Загрузить", this);
    QLabel* status_label = new QLabel("", this);
    bottom_layout->addWidget(save_button, 2);
    bottom_layout->addWidget(load_button, 2);
    bottom_layout->addWidget(status_label, 6);
    main_layout->addLayout(bottom_layout);
    central_widget->setLayout(main_layout);
    connect(tasks_list_widget_, &QListWidget::itemDoubleClicked, this, &MainWindow::onTaskDoubleClicked);
    connect(add_task_button, &QPushButton::clicked, this, &MainWindow::onAddTaskClicked);
    connect(remove_task_button, &QPushButton::clicked, this, &MainWindow::onRemoveTaskClicked);
    connect(save_button, &QPushButton::clicked, this, &MainWindow::onSaveClicked);
    connect(load_button, &QPushButton::clicked, this, &MainWindow::onLoadClicked);
    updateTaskList();
}

MainWindow::~MainWindow()
{
    delete ui_;
}

void MainWindow::onTaskDoubleClicked(QListWidgetItem* item) {
    int index = tasks_list_widget_->row(item);
    if (index < 0 || index >= tasks_.size()) return;

    const Task& task = tasks_[index];

    QDialog dialog(this);
    dialog.setWindowTitle("Информация о задаче");

    QLabel* name_label = new QLabel("Название задачи: " + task.getName(), &dialog);
    QLabel* description_label = new QLabel("Описание задачи: " + task.getDescription(), &dialog);
    QLabel* date_time_label = new QLabel("Дата и время: " + task.getDateTime().toString("dd.MM.yyyy hh:mm"), &dialog);
    QLabel* reminder_label = new QLabel("Напоминание: " + QString(task.isReminderEnabled() ? "Включено" : "Отключено"), &dialog);

    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok, &dialog);

    QVBoxLayout* layout = new QVBoxLayout(&dialog);
    layout->addWidget(name_label);
    layout->addWidget(description_label);
    layout->addWidget(date_time_label);
    layout->addWidget(reminder_label);
    layout->addWidget(button_box);

    dialog.setLayout(layout);

    connect(button_box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);

    dialog.exec();
}

void MainWindow::onAddTaskClicked()
{
    QDialog dialog(this);
    dialog.setWindowTitle("Добавить задачу");
    QLabel* name_label = new QLabel("Название задачи:", &dialog);
    QLineEdit* name_edit = new QLineEdit(&dialog);
    QLabel* time_label = new QLabel("Время задачи:", &dialog);
    QTimeEdit* time_edit = new QTimeEdit(&dialog);
    QLabel* date_label = new QLabel("Дата задачи:", &dialog);
    QLabel* description_label = new QLabel("Описание задачи:", &dialog);
    QCalendarWidget* calendar_widget = new QCalendarWidget(&dialog);
    QTextEdit* description_edit = new QTextEdit(&dialog);
    QCheckBox* reminder_check_box = new QCheckBox("Напомнить о задаче?", &dialog);
    reminder_check_box->setChecked(true);
    QDialogButtonBox* button_box = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, &dialog);
    QGridLayout* layout = new QGridLayout(&dialog);
    layout->addWidget(name_label, 0, 0);
    layout->addWidget(name_edit, 0, 1);
    layout->addWidget(time_label, 1, 0);
    layout->addWidget(time_edit, 1, 1);
    layout->addWidget(date_label, 2, 0);
    layout->addWidget(description_label, 2, 1);
    layout->addWidget(calendar_widget, 3, 0);
    layout->addWidget(description_edit, 3, 1);
    layout->addWidget(reminder_check_box, 4, 0, 1, 2);
    layout->addWidget(button_box, 5, 0, 1, 2);
    layout->setColumnStretch(0, 50);
    layout->setColumnStretch(1, 50);
    date_label->setAlignment(Qt::AlignCenter);
    description_label->setAlignment(Qt::AlignCenter);
    dialog.setLayout(layout);
    connect(button_box, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(button_box, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);
    if (dialog.exec() == QDialog::Accepted) {
        QDateTime date_time(calendar_widget->selectedDate(), time_edit->time());
        QString name = name_edit->text();
        if (name.size() == 0) {
            QMessageBox::warning(this, "Ошибка!", "Имя задачи не может быть пустым!");
            return;
        }
        QDateTime current_date_time = QDateTime::currentDateTime();
        if (date_time <= current_date_time) {
            QMessageBox::warning(this, "Ошибка!", "Дата и время задачи должны быть позже текущего времени!");
            return;
        }
        bool reminder_enabled = reminder_check_box->isChecked();
        QString description = description_edit->toPlainText();
        Task new_task(name, description, date_time, reminder_enabled);
        addTask(new_task);
        updateTaskList();
        if (reminder_enabled) {
            setupReminderTimer();
        }
    }
}

void MainWindow::onSaveClicked()
{
    QString file_name = QFileDialog::getSaveFileName(this, "Сохранить задачи", ".task", "Task Files (*.task)");
    if (file_name.isEmpty()) {
        return;
    }
    QFile file(file_name);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(this, "Ошибка!", "Не удалось открыть файл для записи!");
        return;
    }
    QDataStream out(&file);
    out << tasks_;
    file.close();
    QLabel* status_label = findChild<QLabel*>();
    if (status_label) {
        status_label->setText("Сохранено успешно!");
    }
}

void MainWindow::onLoadClicked()
{
    QString file_name = QFileDialog::getOpenFileName(this, "Загрузить задачи", "", "Task Files (*.task)");
    if (file_name.isEmpty()) {
        return;
    }
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly)) {
        QMessageBox::warning(this, "Ошибка!", "Не удалось открыть файл для чтения!");
        return;
    }
    QDataStream in(&file);
    QList<Task> loaded_tasks;
    in >> loaded_tasks;
    file.close();
    tasks_ = loaded_tasks;
    updateTaskList();
    QLabel* status_label = findChild<QLabel*>();
    if (status_label) {
        status_label->setText("Загружено успешно!");
    }
}

void MainWindow::setupReminderTimer()
{
    reminder_timer_->start(500);
}

void MainWindow::onRemoveTaskClicked()
{
    QListWidget* task_list_widget = findChild<QListWidget*>();
    if (!task_list_widget) return;
    QListWidgetItem* selected_item = task_list_widget->currentItem();
    if (!selected_item) {
        QMessageBox::warning(this, "Ошибка!", "Выберите задачу для удаления!");
        return;
    }
    int index = task_list_widget->row(selected_item);
    removeTask(index);
    updateTaskList();
}

void MainWindow::addTask(const Task& task)
{
    tasks_.append(task);
}

void MainWindow::removeTask(int index)
{
    if (index >= 0 && index < tasks_.size()) {
        tasks_.removeAt(index);
    }
}

void MainWindow::updateTaskList()
{
    QListWidget* task_list_widget = findChild<QListWidget*>();
    std::sort(tasks_.begin(), tasks_.end());
    if (!task_list_widget) return;
    task_list_widget->clear();
    for (const Task& task : tasks_) {
        QString task_text = task.getName() + " - " + task.getDateTime().toString("dd.MM.yyyy hh:mm");
        QListWidgetItem* item = new QListWidgetItem(task_text);
        task_list_widget->addItem(item);
    }
    setupReminderTimer();
}

void MainWindow::checkReminders()
{
    QDateTime now = QDateTime::currentDateTime();
    for (const Task& task : tasks_) {
        if (task.isReminderEnabled() && task.getDateTime() <= now) {
            QMessageBox::information(this, "Напоминание", "Задача: " + task.getName() + "\nВремя: " + task.getDateTime().toString("dd.MM.yyyy hh:mm"));
            const_cast<Task&>(task).setReminderEnabled(false);
        }
    }
    setupReminderTimer();
}
//NOLINTEND