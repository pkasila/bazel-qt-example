#include "ticket_reviewer.h"

#include <QFocusEvent>
#include <QRandomGenerator>

TicketReviewer::TicketReviewer(QWidget* parent)
    : QMainWindow(parent), currentTicketIndex(-1), previousTicketIndex(-1) {
    createWidgets();
    createLayouts();
    createConnections();

    setWindowTitle(tr("Повторение билетов"));
    resize(800, 600);

    onTicketCountChanged(10);
}

void TicketReviewer::createWidgets() {
    countSpinBox = new QSpinBox(this);
    countSpinBox->setRange(1, 1000);
    countSpinBox->setValue(10);
    countSpinBox->setPrefix(tr("Количество билетов: "));

    viewWidget = new QListWidget(this);
    viewWidget->setViewMode(QListView::ListMode);
    viewWidget->setMovement(QListView::Static);
    viewWidget->setSelectionMode(QAbstractItemView::SingleSelection);

    numberLabel = new QLabel(this);
    numberLabel->setFont(QFont("Arial", 14, QFont::Bold));

    nameLabel = new QLabel(this);
    nameLabel->setFont(QFont("Arial", 12));

    nameEdit = new QLineEdit(this);
    nameEdit->setPlaceholderText(tr("Введите имя билета"));

    statusComboBox = new QComboBox(this);
    statusComboBox->addItem(tr("Не начат"), Ticket::Default);
    statusComboBox->addItem(tr("Повторить"), Ticket::Yellow);
    statusComboBox->addItem(tr("Выучен"), Ticket::Green);

    nextButton = new QPushButton(tr("Следующий"), this);
    previousButton = new QPushButton(tr("Предыдущий"), this);

    totalProgressBar = new QProgressBar(this);
    totalProgressBar->setRange(0, 100);
    totalProgressBar->setFormat(tr("Общий прогресс: %p%"));

    greenProgressBar = new QProgressBar(this);
    greenProgressBar->setRange(0, 100);
    greenProgressBar->setFormat(tr("Выучено: %p%"));

    QGroupBox* questionGroup = new QGroupBox(tr("Текущий билет"), this);
    QVBoxLayout* questionLayout = new QVBoxLayout(questionGroup);
    questionLayout->addWidget(numberLabel);
    questionLayout->addWidget(nameLabel);
    questionLayout->addWidget(new QLabel(tr("Изменить имя:"), this));
    questionLayout->addWidget(nameEdit);
    questionLayout->addWidget(new QLabel(tr("Статус:"), this));
    questionLayout->addWidget(statusComboBox);
    questionLayout->addWidget(nextButton);
    questionLayout->addWidget(previousButton);

    QWidget* central = new QWidget(this);
    setCentralWidget(central);

    QHBoxLayout* mainLayout = new QHBoxLayout(central);
    mainLayout->addWidget(viewWidget, 1);
    mainLayout->addWidget(questionGroup, 1);
}

void TicketReviewer::createLayouts() {
}

void TicketReviewer::createConnections() {
    connect(
        countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &TicketReviewer::onTicketCountChanged);
    connect(viewWidget, &QListWidget::itemClicked, this, &TicketReviewer::onTicketClicked);
    connect(
        viewWidget, &QListWidget::itemDoubleClicked, this, &TicketReviewer::onTicketDoubleClicked);
    connect(nameEdit, &QLineEdit::returnPressed, this, &TicketReviewer::onNameEditReturnPressed);
    connect(
        statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &TicketReviewer::onStatusChanged);
    connect(nextButton, &QPushButton::clicked, this, &TicketReviewer::onNextQuestion);
    connect(previousButton, &QPushButton::clicked, this, &TicketReviewer::onPreviousQuestion);
}

void TicketReviewer::onTicketCountChanged(int count) {
    QVector<Ticket::Status> oldStatuses;
    QVector<QString> oldNames;
    for (const auto& ticket : tickets) {
        oldStatuses.append(ticket.status);
        oldNames.append(ticket.name);
    }

    tickets.resize(count);
    for (int i = 0; i < count; ++i) {
        tickets[i].number = i + 1;
        if (i >= oldStatuses.size()) {
            tickets[i].name = "Билет " + QString::number(i + 1);
            tickets[i].status = Ticket::Default;
        } else {
            if (i < oldNames.size()) {
                tickets[i].name = oldNames[i];
            }
        }
    }

    viewWidget->clear();
    for (int i = 0; i < count; ++i) {
        auto* item = new QListWidgetItem(QString("Билет %1").arg(i + 1));
        item->setBackground(Qt::gray);
        viewWidget->addItem(item);
    }

    updateTicketColors();
    updateProgress();

    currentTicketIndex = -1;
    previousTicketIndex = -1;
    history.clear();
    numberLabel->clear();
    nameLabel->clear();
    nameEdit->clear();
    statusComboBox->setCurrentIndex(0);
}

void TicketReviewer::onTicketClicked(QListWidgetItem* item) {
    int index = viewWidget->row(item);
    if (index >= 0 && index < tickets.size()) {
        if (currentTicketIndex != index) {
            previousTicketIndex = currentTicketIndex;
            if (currentTicketIndex >= 0) {
                history.append(currentTicketIndex);
            }
        }
        updateTicketView(index);
    }
}

void TicketReviewer::onTicketDoubleClicked(QListWidgetItem* item) {
    int index = viewWidget->row(item);
    if (index >= 0 && index < tickets.size()) {
        Ticket& ticket = tickets[index];
        if (ticket.status == Ticket::Default || ticket.status == Ticket::Yellow) {
            ticket.status = Ticket::Green;
        } else if (ticket.status == Ticket::Green) {
            ticket.status = Ticket::Yellow;
        }
        updateTicketColors();
        updateProgress();
        updateTicketView(index);
    }
}

void TicketReviewer::onNameEditReturnPressed() {
    if (isFocusInNameEdit() && currentTicketIndex >= 0) {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty()) {
            tickets[currentTicketIndex].name = newName;
            nameLabel->setText(newName);

            // Update in view
            QListWidgetItem* item = viewWidget->item(currentTicketIndex);
            if (item) {
                item->setText(newName);
            }
        }
        nameEdit->clearFocus();
    }
}

void TicketReviewer::onNameEditFocusChanged(bool hasFocus) {
    Q_UNUSED(hasFocus);
}

void TicketReviewer::onStatusChanged(int index) {
    if (currentTicketIndex >= 0) {
        Ticket::Status newStatus =
            static_cast<Ticket::Status>(statusComboBox->itemData(index).toInt());
        tickets[currentTicketIndex].status = newStatus;
        updateTicketColors();
        updateProgress();
    }
}

void TicketReviewer::onNextQuestion() {
    QVector<int> available;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == Ticket::Default || tickets[i].status == Ticket::Yellow) {
            available.append(i);
        }
    }

    if (available.isEmpty()) {
        QMessageBox::information(this, tr("Информация"), tr("Все билеты выучены!"));
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(available.size());
    int ticketIndex = available[randomIndex];

    if (currentTicketIndex != ticketIndex) {
        previousTicketIndex = currentTicketIndex;
        if (currentTicketIndex >= 0) {
            history.append(currentTicketIndex);
        }
    }

    selectRandomTicket();
}

void TicketReviewer::onPreviousQuestion() {
    if (!history.isEmpty()) {
        int prevIndex = history.takeLast();
        if (prevIndex >= 0 && prevIndex < tickets.size()) {
            previousTicketIndex = currentTicketIndex;
            updateTicketView(prevIndex);
        }
    }
}

void TicketReviewer::updateProgress() {
    if (tickets.isEmpty()) {
        totalProgressBar->setValue(0);
        greenProgressBar->setValue(0);
        return;
    }

    int totalNonDefault = 0;
    int greenCount = 0;

    for (const auto& ticket : tickets) {
        if (ticket.status != Ticket::Default) {
            totalNonDefault++;
        }
        if (ticket.status == Ticket::Green) {
            greenCount++;
        }
    }

    totalProgressBar->setValue(100 * totalNonDefault / tickets.size());
    greenProgressBar->setValue(100 * greenCount / tickets.size());
}

void TicketReviewer::updateTicketView(int index) {
    currentTicketIndex = index;
    Ticket& ticket = tickets[index];

    numberLabel->setText(tr("Билет №%1").arg(ticket.number));
    nameLabel->setText(ticket.name);
    nameEdit->setText(ticket.name);

    for (int i = 0; i < statusComboBox->count(); ++i) {
        if (statusComboBox->itemData(i).toInt() == ticket.status) {
            statusComboBox->setCurrentIndex(i);
            break;
        }
    }

    viewWidget->setCurrentRow(index);
}

void TicketReviewer::selectRandomTicket() {
    QVector<int> available;
    for (int i = 0; i < tickets.size(); ++i) {
        if (tickets[i].status == Ticket::Default || tickets[i].status == Ticket::Yellow) {
            available.append(i);
        }
    }

    if (available.isEmpty()) {
        return;
    }

    int randomIndex = QRandomGenerator::global()->bounded(available.size());
    int ticketIndex = available[randomIndex];

    updateTicketView(ticketIndex);
}

void TicketReviewer::updateTicketColors() {
    for (int i = 0; i < tickets.size() && i < viewWidget->count(); ++i) {
        QListWidgetItem* item = viewWidget->item(i);
        if (item) {
            switch (tickets[i].status) {
                case Ticket::Default:
                    item->setBackground(Qt::gray);
                    break;
                case Ticket::Yellow:
                    item->setBackground(QColor(255, 255, 0));
                    break;
                case Ticket::Green:
                    item->setBackground(QColor(0, 255, 0));
                    break;
            }
        }
    }
}

bool TicketReviewer::isFocusInNameEdit() const {
    return nameEdit->hasFocus();
}
