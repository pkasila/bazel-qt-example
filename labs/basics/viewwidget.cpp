#include "viewwidget.h"
#include <QHBoxLayout>

ViewWidget::ViewWidget(QWidget *parent) : QWidget(parent),
    currentTicket(nullptr), currentItem(nullptr)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);

    listWidget = new QListWidget(this);

    mainLayout->addWidget(listWidget);

    questionWidget = new QuestionWidget(this);
    connect(questionWidget, &QuestionWidget::ticketSelected,
            this, &ViewWidget::handleTicketSelection);
    mainLayout->addWidget(questionWidget);

    progressWidget = new ProgressWidget(this);
    mainLayout->addWidget(progressWidget);

    questionView = new QGroupBox("Информация о билете", this);
    QVBoxLayout *questionLayout = new QVBoxLayout(questionView);

    numberLabel = new QLabel("Номер: ", questionView);
    questionLayout->addWidget(numberLabel);

    nameLabel = new QLabel("Имя: ", questionView);
    questionLayout->addWidget(nameLabel);

    nameEdit = new QLineEdit(questionView);
    questionLayout->addWidget(nameEdit);
    connect(nameEdit, &QLineEdit::editingFinished, this, &ViewWidget::onNameEditFinished);

    statusCombo = new QComboBox(questionView);
    statusCombo->addItem("Default", Ticket::Default);
    statusCombo->addItem("Yellow", Ticket::Yellow);
    statusCombo->addItem("Green", Ticket::Green);
    questionLayout->addWidget(statusCombo);
    connect(statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &ViewWidget::onStatusChanged);

    questionView->setLayout(questionLayout);
    mainLayout->addWidget(questionView);

    setLayout(mainLayout);

    connect(listWidget, &QListWidget::itemClicked, this, &ViewWidget::onItemClicked);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &ViewWidget::onItemDoubleClicked);
}

void ViewWidget::setItemCount(int count)
{
    listWidget->clear();
    progressWidget->tickets_count = count;
    progressWidget->Clear();
    for (int i = 0; i < count; i++) {
        QListWidgetItem* item = new QListWidgetItem;
        Ticket* ticket = new Ticket(i + 1);
        allTickets.append(ticket);
        item->setData(Qt::UserRole, QVariant::fromValue(ticket));
        updateItemAppearance(item);
       item->setSizeHint(QSize(listWidget->viewport()->width(), 50));
        listWidget->addItem(item);
    }
    listWidget->setResizeMode(QListView::Adjust);
    listWidget->setUniformItemSizes(false);
    listWidget->updateGeometry();
    listWidget->repaint();

    questionWidget->setTickets(allTickets);
}

void ViewWidget::updateItemAppearance(QListWidgetItem* item)
{
    if (!item) return;

    Ticket* ticket = item->data(Qt::UserRole).value<Ticket*>();
    item->setText(ticket->name);

    QColor color;
    switch (ticket->status) {
    case Ticket::Default: color = Qt::gray; break;
    case Ticket::Yellow: color = Qt::yellow; break;
    case Ticket::Green: color = Qt::green; break;
    }
    item->setBackground(color);
}
void ViewWidget::onItemClicked(QListWidgetItem* item) {
    if (!item || questionWidget->isPrev) return;

    currentItem = item;
    currentTicket = item->data(Qt::UserRole).value<Ticket*>();

    questionWidget->addToHistory(currentTicket);

    if (currentTicket) {
        numberLabel->setText("Номер: " + QString::number(currentTicket->number));
        nameLabel->setText("Имя: " + currentTicket->name);
        nameEdit->setText(currentTicket->name);
        statusCombo->setCurrentIndex(currentTicket->status);
    }
}

void ViewWidget::onItemDoubleClicked(QListWidgetItem* item)
{
    if (!item) return;

    Ticket* ticket = item->data(Qt::UserRole).value<Ticket*>();
    if (!ticket) return;

    if (ticket->status == Ticket::Green) {
        ticket->status = Ticket::Yellow;
        progressWidget->updateBars(-1);
    } else {
        if (ticket->status == Ticket::Yellow) {
            progressWidget->updateBars(1);
        }
        else {
            progressWidget->updateBars(2);
        }
        ticket->status = Ticket::Green;
    }

    updateItemAppearance(item);

    if (item == currentItem) {
        statusCombo->setCurrentIndex(ticket->status);
    }
}

void ViewWidget::onNameEditFinished()
{
    if (!currentTicket || !currentItem) return;

    QString newName = nameEdit->text().trimmed();
    if (!newName.isEmpty()) {
        currentTicket->name = newName;
        updateItemAppearance(currentItem);
    }
}

void ViewWidget::handleTicketSelection(Ticket* ticket) {
    if (!ticket) return;

    currentTicket = ticket;
    numberLabel->setText("Номер: " + QString::number(ticket->number));
    nameLabel->setText("Имя: " + ticket->name);
    nameEdit->setText(ticket->name);
    statusCombo->setCurrentIndex(ticket->status);

    for (int i = 0; i < listWidget->count(); ++i) {
        QListWidgetItem* item = listWidget->item(i);
        if (item->data(Qt::UserRole).value<Ticket*>() == ticket) {
            listWidget->setCurrentItem(item);
            currentItem = item;
            break;
        }
    }
}

void ViewWidget::onStatusChanged(int index)
{
    if (!currentTicket || !currentItem) return;
    int new_status = statusCombo->itemData(index).toInt();
    if (new_status != currentTicket->status) {
        if (currentTicket->status == Ticket::Default) {
            if (new_status == 2) {
                progressWidget->updateBars(2);
            } else {
                progressWidget->updateBars(1);
            }
        } else if (currentTicket->status == Ticket::Green) {
            progressWidget->updateBars(-1);
            if (new_status == 0) {
                progressWidget->updateBars(3);
            }
        } else {
            if (new_status == 2) {
                progressWidget->updateBars(1);
            } else {
                progressWidget->updateBars(3);
            }
        }
    }
    currentTicket->status = static_cast<Ticket::Status>(statusCombo->itemData(index).toInt());
    updateItemAppearance(currentItem);
}
