#include "main_window.h"

#include "ticket.h"

#include <QtWidgets>

//NOLINTBEGIN(cppcoreguidelines-owning-memory, *-unused-return-value)
MainWindow::MainWindow()
    : count_(new QSpinBox())
    , view_(new QListWidget())
    , question_view_(new QGroupBox())
    , number_(new QLabel())
    , name_(new QLabel())
    , name_edit_(new QLineEdit())
    , status_(new QComboBox())
    , next_question_(new QPushButton())
    , previous_question_(new QPushButton())
    , total_progress_(new QProgressBar())
    , green_progress_(new QProgressBar()) {
    auto* widget = new QWidget();
    setCentralWidget(widget);
    setWindowTitle("BASICS");
    setStyleSheet(
        R"(
    QListWidget {
        font-size: 16px;
        padding: 5px;
    }
    QSpinBox, QComboBox, QLineEdit {
        font-size: 16px;
        padding: 5px;
    }
    QPushButton {
        background-color: #0078D7;
        color: white;
        border-radius: 5px;
        padding: 7px;
    }
    QPushButton:hover {
        background-color: #005A9E;
    }
    QLabel {
        font-size: 16px;
        font-weight: bold;
    }
    )");

    auto* grid_layout = new QGridLayout();
    auto* left_v_layout = new QVBoxLayout();
    auto* right_v_layout = new QVBoxLayout();
    auto* h_layout = new QHBoxLayout();

    auto* separator = new QFrame();

    Connect();

    count_->setMinimum(1);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);

    status_->addItem("Не повторён");
    status_->addItem("Нужно вернуться");
    status_->addItem("Повторён");

    previous_question_->setText("Предыдущий вопрос");
    next_question_->setText("Следующий вопрос");
    previous_question_->setIcon(style()->standardIcon(QStyle::SP_ArrowLeft));
    next_question_->setIcon(style()->standardIcon(QStyle::SP_ArrowRight));
    next_question_->setLayoutDirection(Qt::RightToLeft);

    grid_layout->setContentsMargins(10, 10, 10, 10);
    h_layout->setSpacing(15);

    left_v_layout->addWidget(view_);
    left_v_layout->addWidget(count_);
    right_v_layout->addWidget(number_);
    right_v_layout->addWidget(name_);
    right_v_layout->addWidget(name_edit_);
    right_v_layout->addWidget(status_);
    right_v_layout->addStretch();
    question_view_->setLayout(right_v_layout);

    separator->setFrameShape(QFrame::VLine);

    h_layout->addItem(left_v_layout);
    h_layout->addWidget(separator);
    h_layout->addWidget(question_view_);
    grid_layout->addItem(h_layout, 0, 0, 1, 2);
    grid_layout->addWidget(new QLabel("Полный прогресс:"), 1, 0, Qt::AlignLeading);
    grid_layout->addWidget(new QLabel("Зелёный прогресс:"), 2, 0, Qt::AlignLeading);
    grid_layout->addWidget(total_progress_, 1, 1, Qt::AlignTrailing);
    grid_layout->addWidget(green_progress_, 2, 1, Qt::AlignTrailing);
    grid_layout->addWidget(previous_question_, 3, 0, Qt::AlignLeading);
    grid_layout->addWidget(next_question_, 3, 1, Qt::AlignLeading);

    count_->setValue(30);

    widget->setLayout(grid_layout);

    setMinimumSize(kDefaultWidth / 2, kDefaultHeight / 2);
    resize(kDefaultWidth, kDefaultHeight);
}

void MainWindow::CountChanged(int count) {
    AllocateTickets(count);
}

void MainWindow::ViewItemClicked(const QListWidgetItem* item) {
    SelectTicket(view_->row(item));
}

void MainWindow::ViewItemDoubleClicked(const QListWidgetItem* item) const {
    switch (const auto ticket = tickets_[view_->row(item)]; ticket->GetStatus()) {
        case Ticket::GREEN: {
            ticket->SetStatus(Ticket::YELLOW);
            break;
        }
        default: {
            ticket->SetStatus(Ticket::GREEN);
        }
    }
}

void MainWindow::ViewCurrentItemChanged(
    QListWidgetItem* /*current*/, const QListWidgetItem* previous) {
    const auto tmp = previous_button_click_;
    previous_button_click_ = false;
    if (previous == nullptr || tickets_.empty() || tmp) {
        return;
    }
    previous_tickets_.push(tickets_[view_->row(previous)]);
}

void MainWindow::TicketNameChanged(int index, const QString& name) const {
    view_->item(index)->setText(name);
    if (selected_ticket_->GetIndex() == index) {
        name_->setText(name);
    }
}

void MainWindow::TicketStatusChanged(int index, Ticket::Status status, Ticket::Status prev_status) {
    Qt::GlobalColor color; //NOLINT(cppcoreguidelines-init-variables)
    switch (status) {
        case Ticket::YELLOW:
            color = Qt::yellow;
            break;
        case Ticket::GREEN:
            color = Qt::green;
            break;
        default:
            color = Qt::transparent;
    }
    view_->item(index)->setBackground(color);
    view_->item(index)->setForeground(status == Ticket::DEFAULT ? QBrush() : Qt::black);
    if (status == Ticket::GREEN) {
        non_green_tickets_.erase(tickets_[index]);
    } else if (prev_status == Ticket::GREEN) {
        non_green_tickets_.insert(tickets_[index]);
    }
    if (selected_ticket_->GetIndex() == index) {
        status_->setCurrentIndex(selected_ticket_->GetStatus());
    }
    total_progress_->setValue(total_progress_->value() - prev_status);
    total_progress_->setValue(total_progress_->value() + status);
    green_progress_->setValue(green_progress_->value() - static_cast<int>(prev_status == Ticket::GREEN));
    green_progress_->setValue(green_progress_->value() + static_cast<int>(status == Ticket::GREEN));
}

void MainWindow::StatusChanged(int index) const {
    if (selected_ticket_ != nullptr) {
        selected_ticket_->SetStatus(static_cast<Ticket::Status>(index));
    }
}

void MainWindow::NameEdited() const {
    const auto text = name_edit_->text();
    if (text.isEmpty()) {
        return;
    }
    selected_ticket_->SetName(text);
}

void MainWindow::PreviousClicked() {
    if (!previous_tickets_.empty()) {
        previous_button_click_ = true;
        SelectTicket(previous_tickets_.top()->GetIndex());
        previous_tickets_.pop();
    }
}

void MainWindow::NextClicked() {
    SelectTicket(GetRandomTicket()->GetIndex());
}


void MainWindow::AllocateTickets(int count) {
    tickets_count_ = count;
    tickets_.clear();
    view_->clear();
    non_green_tickets_.clear();
    previous_tickets_ = std::stack<std::shared_ptr<Ticket> >();
    tickets_.reserve(tickets_count_);
    QStringList items;
    items.reserve(tickets_count_);
    for (int i = 0; i < tickets_count_; i++) {
        auto ticket = std::make_shared<Ticket>(i + 1);
        tickets_.push_back(ticket);
        items.append(ticket->GetName());
        connect(
            &*ticket, &Ticket::NameChanged, [i, this](const QString& name) {
                TicketNameChanged(i, name);
            });
        connect(
            &*ticket, &Ticket::StatusChanged, [i, this](
            Ticket::Status status, Ticket::Status prev_status) {
                TicketStatusChanged(i, status, prev_status);
            });
    }
    view_->addItems(items);
    non_green_tickets_.insert(tickets_.begin(), tickets_.end());
    total_progress_->setRange(0, tickets_count_ * 2);
    green_progress_->setRange(0, tickets_count_);
    total_progress_->setValue(0);
    green_progress_->setValue(0);
    SelectTicket(0);
}

void MainWindow::SelectTicket(int index) {
    view_->setCurrentRow(index);
    selected_ticket_ = tickets_.at(index);
    number_->setText(QString("Билет №%1").arg(selected_ticket_->GetNumber()));
    name_->setText(selected_ticket_->GetName());
    name_edit_->setText(selected_ticket_->GetName());
    status_->setCurrentIndex(selected_ticket_->GetStatus());
}

std::shared_ptr<Ticket> MainWindow::GetRandomTicket() const {
    static std::random_device rd;
    static std::mt19937 rng{rd()};
    if (non_green_tickets_.empty()) {
        return selected_ticket_;
    }
    std::uniform_int_distribution distribution(0, static_cast<int>(non_green_tickets_.size()) - 1);
    const auto index = distribution(rng);
    auto iterator = std::next(non_green_tickets_.begin(), index);
    if (*iterator == selected_ticket_) {
        iterator = std::next(
            non_green_tickets_.begin(), (index + 1) % static_cast<int>(non_green_tickets_.size()));
    }
    return *iterator;
}

void MainWindow::Connect() {
    connect(count_, &QSpinBox::valueChanged, this, &MainWindow::CountChanged);
    connect(view_, &QListWidget::itemClicked, this, &MainWindow::ViewItemClicked);
    connect(view_, &QListWidget::itemDoubleClicked, this, &MainWindow::ViewItemDoubleClicked);
    connect(view_, &QListWidget::currentItemChanged, this, &MainWindow::ViewCurrentItemChanged);
    connect(status_, &QComboBox::currentIndexChanged, this, &MainWindow::StatusChanged);
    connect(name_edit_, &QLineEdit::editingFinished, this, &MainWindow::NameEdited);
    connect(next_question_, &QPushButton::clicked, this, &MainWindow::NextClicked);
    connect(previous_question_, &QPushButton::clicked, this, &MainWindow::PreviousClicked);
}



//NOLINTEND(cppcoreguidelines-owning-memory, *-unused-return-value)