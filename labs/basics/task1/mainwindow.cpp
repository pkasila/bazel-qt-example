#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QRandomGenerator>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include "tickdialog.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    tot_pr_bar_ = new QProgressBar(this);
    learn_pr_bar_ = new QProgressBar(this);
    tot_pr_label_ = new QLabel("Общий прогресс по билетам", this);
    learn_pr_label_ = new QLabel("Билетов выучено", this);
    tot_pr_bar_->setRange(0, 100);
    tot_pr_bar_->setValue(0);
    tot_pr_bar_->setStyleSheet("QProgressBar { background-color: lightgray; text-align:center; }" "QProgressBar::chunk { background-color: yellow; }");
    learn_pr_bar_->setRange(0, 100);
    learn_pr_bar_->setValue(0);
    learn_pr_bar_->setStyleSheet("QProgressBar { background-color: lightgray; text-align:center;}" "QProgressBar::chunk { background-color: green; }");
    QHBoxLayout* progress_layout1 = new QHBoxLayout();
    progress_layout1->addWidget(tot_pr_bar_, 2);
    progress_layout1->addWidget(tot_pr_label_, 1);
    QHBoxLayout* progress_layout2 = new QHBoxLayout();
    progress_layout2->addWidget(learn_pr_bar_, 2);
    progress_layout2->addWidget(learn_pr_label_, 1);
    next_tick_ = new QPushButton("Следующий билет", this);
    prev_tick_ = new QPushButton("Предыдущий билет", this);
    list_ = new QListWidget(this);
    main_layout_ = new QVBoxLayout();
    QHBoxLayout* spinbox_layout = new QHBoxLayout();
    cnt_ = new QSpinBox(this);
    cnt_->setRange(1, 100);
    cnt_->setValue(1);
    QLabel* spinbox_text = new QLabel("Количество билетов:", this);
    spinbox_layout->addWidget(spinbox_text, 2);
    spinbox_layout->addWidget(cnt_, 2);
    QHBoxLayout* button_layout = new QHBoxLayout();
    button_layout->addWidget(prev_tick_);
    button_layout->addWidget(next_tick_);
    main_layout_->addLayout(progress_layout1);
    main_layout_->addLayout(progress_layout2);
    main_layout_->addLayout(spinbox_layout);
    main_layout_->addWidget(list_);
    main_layout_->addLayout(button_layout);
    QPushButton* save_button = new QPushButton("Сохранить", this);
    QPushButton* load_button = new QPushButton("Загрузить", this);
    QHBoxLayout* save_load_layout = new QHBoxLayout();
    save_load_layout->addSpacerItem(new QSpacerItem(40, 20, QSizePolicy::Expanding, QSizePolicy::Minimum));
    save_load_layout->addWidget(save_button);
    save_load_layout->addWidget(load_button);
    main_layout_->addLayout(save_load_layout);
    QWidget* central_widget = new QWidget(this);
    central_widget->setLayout(main_layout_);
    setCentralWidget(central_widget);
    connect(next_tick_, &QPushButton::clicked, this, &MainWindow::NextTicketButton);
    connect(prev_tick_, &QPushButton::clicked, this, &MainWindow::PrevTicketButton);
    connect(list_, &QListWidget::itemDoubleClicked, this, &MainWindow::DoubleClickOnTicket);
    connect(cnt_, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::UpdateTickets);
    connect(list_, &QListWidget::itemClicked, this, &MainWindow::SingleClickOnTicket);
    connect(save_button, &QPushButton::clicked, this, [this]() {
        SaveTickets("progress.txt");
    });
    connect(load_button, &QPushButton::clicked, this, [this]() {
        LoadTickets("progress.txt");
    });
    UpdateTickets(cnt_->value());
}

void MainWindow::onHintChanged(int index, const QString& newHint) {
    if (index >= 0 && index < tickets_.size()) {
        tickets_[index].SetHint(newHint);
    }
}

void MainWindow::UpdateProgressBars() {
    int total_tickets = tickets_.size();
    if (total_tickets == 0) {
        tot_pr_bar_->setValue(0);
        learn_pr_bar_->setValue(0);
        return;
    }
    int tot_pr = 0;
    int learn_pr = 0;
    for (const Ticket& ticket : tickets_) {
        if (ticket.GetStat() == 1 || ticket.GetStat() == 2) {
            tot_pr++;
        }
        if (ticket.GetStat() == 2) {
            learn_pr++;
        }
    }
    tot_pr_bar_->setValue((tot_pr * 100) / total_tickets);
    learn_pr_bar_->setValue((learn_pr * 100) / total_tickets);
}

void MainWindow::NextTicketButton() {
    GetNextTicket();
}

void MainWindow::PrevTicketButton() {
    int prev_tick = GetPref();
    if (prev_tick >= 0 && prev_tick < tickets_.size()) {
        list_->setCurrentRow(prev_tick);
    } else {
        QMessageBox::warning(this, "", "Нет предыдущих билетов.");
    }
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::UpdateTickets(int new_cnt) {
    while (!tick_stack_.empty()) {
        tick_stack_.pop();
    }
    list_->clear();
    tickets_.clear();

    for (int i = 0; i < new_cnt; ++i) {
        Ticket new_tick{i};
        new_tick.ChangeName(QString("Билет № %1").arg(i + 1));
        tickets_.push_back(new_tick);
        QListWidgetItem* item = new QListWidgetItem(new_tick.GetName());
        if (new_tick.GetStat() == 0) {
            item->setBackground(Qt::gray);
        } else if (new_tick.GetStat() == 1) {
            item->setBackground(Qt::yellow);
        } else if (new_tick.GetStat() == 2) {
            item->setBackground(Qt::green);
        }
        list_->addItem(item);
    }
    UpdateProgressBars();
}

void MainWindow::DoubleClickOnTicket(QListWidgetItem* item) {
    int tick_ind = list_->row(item);
    if (tick_ind >= 0 && tick_ind < tickets_.size()) {
        Ticket& ticket = tickets_[tick_ind];
        TickDialog* dialog = new TickDialog{ticket, this};
        connect(dialog, &TickDialog::nameChanged, this, &MainWindow::onNameChanged);
        connect(dialog, &TickDialog::statusChanged, this, &MainWindow::onStatusChanged);
        connect(dialog, &TickDialog::hintChanged, this, &MainWindow::onHintChanged);
        dialog->exec();
        delete dialog;
    }
}

void MainWindow::SingleClickOnTicket(QListWidgetItem* item) {
    int tick_ind = list_->row(item);
    tickets_[tick_ind].ChangeStat();
    if (tickets_[tick_ind].GetStat() == 0) {
        item->setBackground(Qt::gray);
    } else if (tickets_[tick_ind].GetStat() == 1) {
        item->setBackground(Qt::yellow);
    } else if (tickets_[tick_ind].GetStat() == 2) {
        item->setBackground(Qt::green);
    }
    UpdateProgressBars();
}

void MainWindow::GetNextTicket() {
    QList<int> available;
    for (int i = 0; i < tickets_.size(); ++i) {
        if (tickets_[i].GetStat() == 0 || tickets_[i].GetStat() == 1) {
            available.append(i);
        }
    }
    if (!available.isEmpty()) {
        int cur_ind = list_->currentRow();
        if (cur_ind != -1) {
            tick_stack_.push(cur_ind);
        }
        int rnd_tick = QRandomGenerator::global()->bounded(available.size());
        int tick = available[rnd_tick];
        list_->setCurrentRow(tick);
    } else {
        QMessageBox::warning(this, "Ура!", "Все билеты выучены!");
    }
}

int MainWindow::GetPref() {
    if (tick_stack_.empty()) {
        return -1;
    }
    auto top = tick_stack_.top();
    tick_stack_.pop();
    return top;
}

void MainWindow::UpdateTicketInList(int index) {
    if (index >= 0 && index < tickets_.size()) {
        QListWidgetItem* item = list_->item(index);
        item->setText(tickets_[index].GetName());
        if (tickets_[index].GetStat() == 0) {
            item->setBackground(Qt::gray);
        } else if (tickets_[index].GetStat() == 1) {
            item->setBackground(Qt::yellow);
        } else if (tickets_[index].GetStat() == 2) {
            item->setBackground(Qt::green);
        }
        UpdateProgressBars();
    }
}

void MainWindow::onNameChanged(int index, const QString& new_name) {
    if (index >= 0 && index < tickets_.size()) {
        QListWidgetItem* item = list_->item(index);
        item->setText(new_name);
    }
}

void MainWindow::onStatusChanged(int index, int new_status) {
    if (index >= 0 && index < tickets_.size()) {
        QListWidgetItem* item = list_->item(index);
        if (new_status == 0) {
            item->setBackground(Qt::gray);
        } else if (new_status == 1) {
            item->setBackground(Qt::yellow);
        } else if (new_status == 2) {
            item->setBackground(Qt::green);
        }
        UpdateProgressBars();
    }
}

void MainWindow::SaveTickets(const QString& filename) {
    QFile file(filename);
    if (file.open(QIODevice::WriteOnly)) {
        QTextStream out(&file);
        for (const Ticket& ticket : tickets_) {
            out << ticket.GetName() << "\n";
            out << ticket.GetStat() << "\n";
            out << ticket.GetHint() << "\n";
        }
        file.close();
        QMessageBox::warning(this, "Успех!", "Прогресс сохранен в файл:" + filename);
    } else {
        QMessageBox::warning(this, "Ошибка!", "Ошибка при сохранении прогресса в файл:" + filename);
    }
}

void MainWindow::LoadTickets(const QString& filename) {
    QFile file(filename);
    if (file.open(QIODevice::ReadOnly)) {
        QTextStream in(&file);
        tickets_.clear();
        list_->clear();
        while (!in.atEnd()) {
            QString name = in.readLine();
            int status = in.readLine().toInt();
            QString hint = in.readLine();
            int sz = tickets_.size();
            Ticket new_ticket{sz};
            new_ticket.ChangeName(name);
            new_ticket.SetStatus(status);
            new_ticket.SetHint(hint);
            tickets_.push_back(new_ticket);

            QListWidgetItem* item = new QListWidgetItem(new_ticket.GetName());
            if (new_ticket.GetStat() == 0) {
                item->setBackground(Qt::gray);
            } else if (new_ticket.GetStat() == 1) {
                item->setBackground(Qt::yellow);
            } else if (new_ticket.GetStat() == 2) {
                item->setBackground(Qt::green);
            }
            list_->addItem(item);
        }
        cnt_->setValue(tickets_.size());
        UpdateProgressBars();
        file.close();
        QMessageBox::warning(this, "Успех!", "Прогресс загружен из файла:" + filename);
    } else {
        QMessageBox::warning(this, "Ошибка!", "Ошибка при загрузке прогресса из файла:" + filename);
    }
}
