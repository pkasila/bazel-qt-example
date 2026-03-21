#include "MainWindow.h"

#include "Ticket.h"
#include "TicketCardWidget.h"

#include <QApplication>
#include <QCheckBox>
#include <QComboBox>
#include <QFileDialog>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QLineEdit>
#include <QMessageBox>
#include <QProgressBar>
#include <QShortcut>
#include <QSpinBox>
#include <QSplitter>
#include <QVBoxLayout>

MainWindow::MainWindow(
    QWidget* parent)     // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    : QWidget(parent) {  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    SetupUi();
    count_spin_box_->setValue(10);
}

void MainWindow::SetupUi() {
    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(12, 12, 12, 12);
    main_layout->setSpacing(12);

    auto* top_layout = new QHBoxLayout();
    top_layout->setSpacing(15);

    // Блок: Билеты
    auto* gen_layout = new QHBoxLayout();
    gen_layout->setSpacing(8);
    count_spin_box_ = new QSpinBox();
    count_spin_box_->setRange(1, 9999);
    count_spin_box_->setFixedWidth(80);
    gen_layout->addWidget(new QLabel("Кол-во билетов:"));
    gen_layout->addWidget(count_spin_box_);

    auto* sep1 = new QFrame();
    sep1->setFrameShape(QFrame::VLine);
    sep1->setFrameShadow(QFrame::Sunken);

    // Блок: Поиск
    auto* view_layout = new QHBoxLayout();
    view_layout->setSpacing(8);
    filter_combo_ = new QComboBox();
    filter_combo_->addItems({"Все", "Дефолт", "Желтые", "Зеленые"});
    filter_combo_->setFixedWidth(90);
    search_edit_ = new QLineEdit();
    search_edit_->setPlaceholderText("Найти...");
    search_edit_->setFixedWidth(130);
    view_layout->addWidget(filter_combo_);
    view_layout->addWidget(search_edit_);

    auto* sep2 = new QFrame();
    sep2->setFrameShape(QFrame::VLine);
    sep2->setFrameShadow(QFrame::Sunken);

    // Блок: Действия
    auto* actions_layout = new QHBoxLayout();
    actions_layout->setSpacing(5);
    shuffle_button_ = new QPushButton("Перемешать");
    reset_order_button_ = new QPushButton("Сбросить порядок");
    reset_names_button_ = new QPushButton("Сбросить имена");
    reset_statuses_button_ = new QPushButton("Сбросить статусы");

    actions_layout->addWidget(shuffle_button_);
    actions_layout->addWidget(reset_order_button_);
    actions_layout->addWidget(reset_names_button_);
    actions_layout->addWidget(reset_statuses_button_);

    top_layout->addLayout(gen_layout);
    top_layout->addWidget(sep1);
    top_layout->addLayout(view_layout);
    top_layout->addWidget(sep2);
    top_layout->addLayout(actions_layout);
    top_layout->addStretch();
    main_layout->addLayout(top_layout);

    auto* splitter = new QSplitter(Qt::Horizontal);
    view_list_ = new QListWidget();
    view_list_->setViewMode(QListView::IconMode);
    view_list_->setResizeMode(QListView::Adjust);
    view_list_->setWordWrap(true);
    view_list_->setSpacing(12);
    view_list_->setUniformItemSizes(true);
    view_list_->setGridSize(QSize(130, 90));
    view_list_->setIconSize(QSize(110, 70));
    view_list_->setMinimumWidth(350);
    view_list_->setStyleSheet(
        "QListWidget::item { background: transparent; } QListWidget::item:selected { background: "
        "transparent; }");

    question_group_ = new QGroupBox("Детали билета");
    question_group_->setMinimumWidth(250);
    auto* details_layout = new QVBoxLayout(question_group_);
    auto* form_layout = new QFormLayout();
    number_label_ = new QLabel("-");
    name_label_ = new QLabel("-");
    name_label_->setWordWrap(true);
    name_edit_ = new QLineEdit();
    name_edit_->setPlaceholderText("Название...");
    status_combo_ = new QComboBox();
    status_combo_->addItems({"Дефолтный", "Желтый", "Зеленый"});

    form_layout->addRow("Номер:", number_label_);
    form_layout->addRow("Имя:", name_label_);
    form_layout->addRow("Правка:", name_edit_);
    form_layout->addRow("Статус:", status_combo_);
    details_layout->addLayout(form_layout);
    details_layout->addStretch();

    splitter->addWidget(view_list_);
    splitter->addWidget(question_group_);
    splitter->setStretchFactor(0, 4);
    splitter->setStretchFactor(1, 1);
    splitter->setCollapsible(0, false);
    splitter->setCollapsible(1, false);
    main_layout->addWidget(splitter);

    auto* bottom_container = new QWidget();
    auto* bottom_layout = new QVBoxLayout(bottom_container);
    bottom_layout->setContentsMargins(0, 0, 0, 0);
    bottom_layout->setSpacing(10);

    auto* progress_layout = new QGridLayout();
    total_progress_ = new QProgressBar();
    total_progress_->setFormat("%v/%m (%p%)");
    total_progress_->setFixedHeight(18);
    green_progress_ = new QProgressBar();
    green_progress_->setFormat("%v/%m (%p%)");
    green_progress_->setFixedHeight(18);
    progress_layout->addWidget(new QLabel("Общий прогресс:"), 0, 0);
    progress_layout->addWidget(total_progress_, 0, 1);
    progress_layout->addWidget(new QLabel("Выучено:"), 1, 0);
    progress_layout->addWidget(green_progress_, 1, 1);

    auto* nav_buttons_layout = new QHBoxLayout();
    prev_button_ = new QPushButton("Предыдущий");
    next_button_ = new QPushButton("Случайный билет");
    save_button_ = new QPushButton("Сохранить JSON");
    load_button_ = new QPushButton("Загрузить JSON");
    nav_buttons_layout->addWidget(prev_button_);
    nav_buttons_layout->addWidget(next_button_);
    nav_buttons_layout->addStretch();
    nav_buttons_layout->addWidget(save_button_);
    nav_buttons_layout->addWidget(load_button_);

    bottom_layout->addLayout(progress_layout);
    bottom_layout->addLayout(nav_buttons_layout);
    main_layout->addWidget(bottom_container);

    connect(
        count_spin_box_, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &MainWindow::OnTicketCountChanged);
    connect(view_list_, &QListWidget::itemDoubleClicked, this, &MainWindow::OnItemDoubleClicked);
    connect(view_list_, &QListWidget::itemClicked, this, &MainWindow::OnItemClicked);
    connect(next_button_, &QPushButton::clicked, this, &MainWindow::OnNextButtonClicked);
    connect(prev_button_, &QPushButton::clicked, this, &MainWindow::OnPreviousButtonClicked);
    connect(name_edit_, &QLineEdit::returnPressed, this, &MainWindow::OnNameEditFinished);
    connect(
        status_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::OnStatusChanged);
    connect(reset_names_button_, &QPushButton::clicked, this, &MainWindow::OnResetNamesClicked);
    connect(
        reset_statuses_button_, &QPushButton::clicked, this, &MainWindow::OnResetStatusesClicked);
    connect(reset_order_button_, &QPushButton::clicked, this, &MainWindow::OnResetOrderClicked);
    connect(save_button_, &QPushButton::clicked, this, &MainWindow::OnSaveClicked);
    connect(load_button_, &QPushButton::clicked, this, &MainWindow::OnLoadClicked);
    connect(search_edit_, &QLineEdit::textChanged, this, &MainWindow::OnSearchChanged);
    connect(
        filter_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::OnFilterChanged);
    connect(shuffle_button_, &QPushButton::clicked, this, &MainWindow::OnShuffleClicked);

    auto* next_shortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(next_shortcut, &QShortcut::activated, this, &MainWindow::OnNextButtonClicked);

    auto* prev_shortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(prev_shortcut, &QShortcut::activated, this, &MainWindow::OnPreviousButtonClicked);
}

void MainWindow::StyleItem(
    [[maybe_unused]] QListWidgetItem* item, [[maybe_unused]] const Ticket& ticket) {
}

void MainWindow::OnTicketCountChanged(int count) {
    manager_.GenerateTickets(count);
    total_progress_->setMaximum(count);
    green_progress_->setMaximum(count);
    current_index_ = -1;
    prev_button_->setEnabled(false);

    number_label_->setText("-");
    name_label_->setText("-");
    name_edit_->clear();
    status_combo_->setCurrentIndex(0);

    UpdateView();
    UpdateProgressBars();
}

void MainWindow::UpdateView() {
    view_list_->clear();
    QString search_text = search_edit_->text().toLower();
    int filter_index = filter_combo_->currentIndex();  // NOLINT(cppcoreguidelines-init-variables)

    for (int i = 0; i < manager_.GetTicketCount(); ++i) {
        const auto& ticket = manager_.GetTicket(i);

        const bool matches_search =
            (ticket.name.toLower().contains(
                 search_text) ||  // NOLINT(cppcoreguidelines-init-variables)
             QString::number(ticket.id).contains(search_text));
        const bool matches_filter =
            (filter_index == 0) || (filter_index == 1 && ticket.status == TicketStatus::Default) ||
            (filter_index == 2 && ticket.status == TicketStatus::Yellow) ||
            (filter_index == 3 &&
             ticket.status == TicketStatus::Green);  // NOLINT(cppcoreguidelines-init-variables)

        if (matches_search && matches_filter) {
            auto* item = new QListWidgetItem(view_list_);
            item->setSizeHint(QSize(110, 70));
            item->setData(Qt::UserRole, i);

            auto* widget = new TicketCardWidget(ticket, current_index_ == i, view_list_);
            view_list_->addItem(item);
            view_list_->setItemWidget(item, widget);

            connect(widget, &TicketCardWidget::ResetRequested, this, &MainWindow::OnTicketReset);

            if (current_index_ == i) {
                item->setSelected(true);
            }
        }
    }
}

void MainWindow::UpdateProgressBars() {
    total_progress_->setValue(manager_.GetTotalProgress());
    green_progress_->setValue(manager_.GetGreenProgress());
}

void MainWindow::SelectTicket(int index) {
    if (index < 0 || index >= manager_.GetTicketCount()) {
        return;
    }

    // Update old ticket selection
    for (int i = 0; i < view_list_->count(); ++i) {
        auto* item = view_list_->item(i);
        int item_idx = item->data(Qt::UserRole).toInt();
        auto* widget = qobject_cast<TicketCardWidget*>(view_list_->itemWidget(item));
        if (widget) {
            widget->UpdateSelection(item_idx == index);
        }
        if (item_idx == index) {
            view_list_->setCurrentItem(item);
        }
    }

    current_index_ = index;
    const auto& ticket = manager_.GetTicket(index);

    number_label_->setText(QString::number(ticket.id));
    name_label_->setText(ticket.name);
    name_edit_->setText(ticket.name);

    QSignalBlocker blocker(status_combo_);
    if (ticket.status == TicketStatus::Default) {  // NOLINT(bugprone-branch-clone)
        status_combo_->setCurrentIndex(0);
    } else if (ticket.status == TicketStatus::Yellow) {
        status_combo_->setCurrentIndex(1);
    } else if (ticket.status == TicketStatus::Green) {
        status_combo_->setCurrentIndex(2);
    }
}

void MainWindow::OnItemClicked(QListWidgetItem* item) {
    int index = item->data(Qt::UserRole).toInt();
    SelectTicket(index);
}

void MainWindow::OnItemDoubleClicked(QListWidgetItem* item) {
    int row = item->data(Qt::UserRole).toInt();
    auto& ticket = manager_.GetTicket(row);

    if (ticket.status == TicketStatus::Default ||
        ticket.status == TicketStatus::Yellow) {  // NOLINT(bugprone-branch-clone)
        manager_.SetTicketStatus(row, TicketStatus::Green);
    } else if (ticket.status == TicketStatus::Green) {
        manager_.SetTicketStatus(row, TicketStatus::Yellow);
    }

    auto* widget = qobject_cast<TicketCardWidget*>(view_list_->itemWidget(item));
    if (widget) {
        widget->UpdateStatus(ticket.status);
    }

    UpdateProgressBars();
    SelectTicket(row);
}

void MainWindow::OnNextButtonClicked() {
    const int next_index =
        manager_.GetNextRandomTicketIndex();  // NOLINT(cppcoreguidelines-init-variables)
    if (next_index != -1) {
        if (current_index_ != -1) {
            manager_.PushToHistory(current_index_);
            prev_button_->setEnabled(true);
        }
        SelectTicket(next_index);
    }
}

void MainWindow::OnPreviousButtonClicked() {
    if (manager_.HasHistory()) {
        int prev_index = manager_.PopFromHistory();  // NOLINT(cppcoreguidelines-init-variables)
        SelectTicket(prev_index);
        if (!manager_.HasHistory()) {
            prev_button_->setEnabled(false);
        }
    }
}

void MainWindow::OnNameEditFinished() {
    if (current_index_ != -1) {
        QString name = name_edit_->text();
        if (name.isEmpty()) {
            return;
        }

        manager_.SetTicketName(current_index_, name);
        name_label_->setText(name);

        for (int i = 0; i < view_list_->count(); ++i) {
            auto* item = view_list_->item(i);
            if (item->data(Qt::UserRole).toInt() == current_index_) {
                auto* widget = qobject_cast<TicketCardWidget*>(view_list_->itemWidget(item));
                if (widget) {
                    widget->UpdateName(name);
                }
                break;
            }
        }
    }
}

void MainWindow::OnStatusChanged(int index) {
    if (current_index_ != -1) {
        TicketStatus status = TicketStatus::Default;
        if (index == 1) {
            status = TicketStatus::Yellow;
        } else if (index == 2) {
            status = TicketStatus::Green;
        }

        manager_.SetTicketStatus(current_index_, status);

        // Find and update the widget
        for (int i = 0; i < view_list_->count(); ++i) {
            auto* item = view_list_->item(i);
            if (item->data(Qt::UserRole).toInt() == current_index_) {
                auto* widget = qobject_cast<TicketCardWidget*>(view_list_->itemWidget(item));
                if (widget) {
                    widget->UpdateStatus(status);
                }
                break;
            }
        }
        UpdateProgressBars();
    }
}

void MainWindow::OnResetNamesClicked() {  // NOLINT(readability-convert-member-functions-to-static)
    manager_.ResetNames();
    if (current_index_ != -1) {
        name_edit_->setText(manager_.GetTicket(current_index_).name);
        name_label_->setText(manager_.GetTicket(current_index_).name);
    }
    UpdateView();
}

void MainWindow::
    OnResetStatusesClicked() {  // NOLINT(readability-convert-member-functions-to-static)
    auto res = QMessageBox::question(this, "Сброс", "Сбросить прогресс по всем билетам?");
    if (res == QMessageBox::Yes) {
        manager_.ResetStatuses();
        UpdateView();
        UpdateProgressBars();
    }
}

void MainWindow::OnResetOrderClicked() {  // NOLINT(readability-convert-member-functions-to-static)
    manager_.SortTicketsById();
    current_index_ = -1;
    UpdateView();
}

void MainWindow::OnSearchChanged(const QString& /*unused*/) {
    UpdateView();
}

void MainWindow::OnFilterChanged(int /*unused*/) {
    UpdateView();
}

void MainWindow::OnSaveClicked() {  // NOLINT(readability-convert-member-functions-to-static)
    QString file_name =
        QFileDialog::getSaveFileName(this, "Сохранить билеты", "", "JSON Files (*.json)");
    if (!file_name.isEmpty()) {
        if (!manager_.SaveToJson(file_name)) {
            QMessageBox::warning(this, "Ошибка", "Не удалось сохранить файл.");
        }
    }
}

void MainWindow::OnLoadClicked() {
    QString file_name =
        QFileDialog::getOpenFileName(this, "Загрузить билеты", "", "JSON Files (*.json)");
    if (!file_name.isEmpty()) {
        if (manager_.LoadFromJson(file_name)) {
            QSignalBlocker blocker(count_spin_box_);
            count_spin_box_->setValue(manager_.GetTicketCount());
            current_index_ = -1;
            prev_button_->setEnabled(false);

            number_label_->setText("-");
            name_label_->setText("-");
            name_edit_->clear();
            status_combo_->setCurrentIndex(0);

            UpdateView();
            UpdateProgressBars();
        } else {
            QMessageBox::warning(this, "Ошибка", "Не удалось загрузить файл.");
        }
    }
}

void MainWindow::OnShuffleClicked() {
    manager_.ShuffleTickets();
    current_index_ = -1;
    UpdateView();
}

void MainWindow::OnTicketReset(int id) {
    for (int i = 0; i < manager_.GetTicketCount(); ++i) {
        if (manager_.GetTicket(i).id == id) {
            manager_.SetTicketStatus(i, TicketStatus::Default);

            for (int j = 0; j < view_list_->count(); ++j) {
                auto* item = view_list_->item(j);
                if (item->data(Qt::UserRole).toInt() == i) {
                    auto* widget = qobject_cast<TicketCardWidget*>(view_list_->itemWidget(item));
                    if (widget) {
                        widget->UpdateStatus(TicketStatus::Default);
                    }
                    break;
                }
            }

            if (current_index_ == i) {
                SelectTicket(i);
            }
            UpdateProgressBars();
            break;
        }
    }
}
