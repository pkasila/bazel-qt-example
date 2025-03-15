#include "mainwindow.h"
#include "./ui_mainwindow.h"

#include <QSpinBox>
#include <QMap>
#include <QRandomGenerator>
#include <QMessageBox>
#include <QTextEdit>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Помощник для студента");

    setLightTheme();
    this->resize(1000,1000);

    ui->comboTicketStatus->addItem("Не выполнено");
    ui->comboTicketStatus->addItem("Выполнено");
    ui->comboTicketStatus->addItem("Повторить");

    ui->tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableWidget->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    connect(ui->DarkTheme, &QAction::triggered, this, &MainWindow::setDarkTheme);
    connect(ui->WhiteTheme, &QAction::triggered, this, &MainWindow::setLightTheme);

    connect(ui->ticketName, &QTextEdit::textChanged, this, &MainWindow::on_ticketName_textChanged);
    connect(ui->ticketTXT, &QTextEdit::textChanged, this, &MainWindow::on_TicketTXT_textChanged);
}

MainWindow::~MainWindow()
{
    delete ui;
}
void MainWindow::setDarkTheme() {
    this->setStyleSheet(
        "QWidget { background-color: #2D2D2D; color: white; }"
        "QPushButton { background-color: #444; color: white; border: 1px solid #777; padding: 5px; border-radius: 2px; }"
        "QPushButton:hover { background-color: #555; }"
        "QLineEdit, QTextEdit { background-color: #555; color: white; border: 1px solid #777; border-radius: 2px; }"
        "QComboBox { background-color: #444; color: white; border: 1px solid #777; border-radius: 2px; padding: 3px; }"
        "QProgressBar { background-color: #444; border: 1px solid #777; text-align: center; border-radius: 2px; }"
        "QProgressBar::chunk { background-color: #1E90FF; border-radius: 2px; }"
        "QTableWidget { background-color: #333; color: white; border: 1px solid #777; gridline-color: #777; border-radius: 2px; }"
        "QHeaderView::section { background-color: #444; color: white; padding: 5px; border: 1px solid #777; border-radius: 2px; }"
        "QTableWidget QTableCornerButton::section { background-color: #444; border: 1px solid #777; border-radius: 2px; }"

        "QTableWidget::item:column(0) { padding: 5px; border-radius: 2px; }"
        "QTableWidget::item:selected:column(0) { background-color: #555; border-radius: 2px; }"
        );
}

void MainWindow::setLightTheme() {
    this->setStyleSheet(
        "QWidget { background-color: #FFFFFF; color: black; }"
        "QPushButton { background-color: #E0E0E0; color: black; border: 1px solid #777; padding: 5px; border-radius: 2px; }"
        "QPushButton:hover { background-color: #D0D0D0; }"
        "QLineEdit, QTextEdit { background-color: #F5F5F5; color: black; border: 1px solid #777; border-radius: 2px; }"
        "QComboBox { background-color: #E0E0E0; color: black; border: 1px solid #777; border-radius: 2px; padding: 3px; }"
        "QProgressBar { background-color: #E0E0E0; border: 1px solid #777; text-align: center; border-radius: 2px; }"
        "QProgressBar::chunk { background-color: #1E90FF; border-radius: 2px; }"
        "QTableWidget { background-color: #FFFFFF; color: black; border: 1px solid #777; gridline-color: #777; border-radius: 2px; }"
        "QHeaderView::section { background-color: #F0F0F0; color: black; padding: 5px; border: 1px solid #777; border-radius: 2px; }"
        "QTableWidget QTableCornerButton::section { background-color: #F0F0F0; border: 1px solid #777; border-radius: 2px; }"

        "QTableWidget::item:column(0) { padding: 5px; border-radius: 2px; }"
        "QTableWidget::item:selected:column(0) { background-color: #E0E0E0; border-radius: 2px; }"
        );
}

void MainWindow::on_ticketName_textChanged() {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        return;
    }

    int ticketNumber = currentRow + 1;
    Ticket* selectedTicket = tickets_[ticketNumber];

    QString newName = ui->ticketName->toPlainText().trimmed();

    if (newName.isEmpty()) {
        ui->ticketName->setText(selectedTicket->getName());
        QMessageBox::warning(this, "Предупреждение",
                             "Название билета не может быть пустым!");
        return;
    }

    selectedTicket->setName(newName);
    QTableWidgetItem* nameItem = ui->tableWidget->item(currentRow, 0);
    nameItem->setText(newName);
}


void MainWindow::on_TicketTXT_textChanged() {
    int currentRow = ui->tableWidget->currentRow();
    if (currentRow == -1) {
        return;
    }

    int ticketNumber = currentRow + 1;
    Ticket* selectedTicket = tickets_[ticketNumber];

    QString newText = ui->ticketTXT->toPlainText();
    selectedTicket->setText(newText);
}

void MainWindow::updateProgressBar() {
    int totalTickets = tickets_.size();
    int progress = (completedTickets_ * 100) / totalTickets;
    ui->progressBarTickets->setValue(progress);
}

void MainWindow::on_spinBoxTickets_valueChanged(int arg1) {
    ui->tableWidget->clearContents();
    ui->tableWidget->setRowCount(arg1);
    ui->tableWidget->setColumnCount(2);
    ui->tableWidget->setHorizontalHeaderLabels({"Имя билета", "Статус"});
    ui->tableWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

    int totalWidth = ui->tableWidget->width();
    int nameColumnWidth = totalWidth * 5 / 9;
    int statusColumnWidth = totalWidth * 4 / 9;
    ui->tableWidget->setColumnWidth(0, nameColumnWidth);
    ui->tableWidget->setColumnWidth(1, statusColumnWidth);

    tickets_.clear();
    completedTickets_ = 0;
    unexecutedTickets_.clear();
    repeatedTickets_.clear();

    for (int i = 0; i < arg1; ++i) {
        Ticket* newTicket = new Ticket(i + 1);
        newTicket->setStatus(TicketStatus::Default);
        tickets_[i + 1] = newTicket;

        unexecutedTickets_.push_back(newTicket);

        QTableWidgetItem* nameItem = new QTableWidgetItem(newTicket->getName());
        QTableWidgetItem* statusItem = new QTableWidgetItem("Не выполнено");
        statusItem->setBackground(QBrush(Qt::gray));

        ui->tableWidget->setItem(i, 0, nameItem);
        ui->tableWidget->setItem(i, 1, statusItem);
    }

    updateProgressBar();
}

void MainWindow::on_tableWidget_itemClicked(QTableWidgetItem *item) {
    int row = item->row();
    int ticketNumber = row + 1;

    if (!tickets_.contains(ticketNumber)) {
        return;
    }
    Ticket selectedTicket = *tickets_[ticketNumber];

    ui->ticketName->setText(selectedTicket.getName());

    ui->ticketTXT->setText(selectedTicket.getText());

    ui->comboTicketStatus->blockSignals(true);
    switch (selectedTicket.getStatus()) {
    case TicketStatus::Yellow:
        ui->comboTicketStatus->setCurrentIndex(2);
        break;
    case TicketStatus::Green:
        ui->comboTicketStatus->setCurrentIndex(1);
        break;
    case TicketStatus::Default:
    default:
        ui->comboTicketStatus->setCurrentIndex(0);
        break;
    }
    ui->comboTicketStatus->blockSignals(false);
}

void MainWindow::updateTicketInTable(int ticketNumber, const Ticket &ticket) {
    int row = ticketNumber - 1;
    QTableWidgetItem *statusItem = ui->tableWidget->item(row, 1);

    switch (ticket.getStatus()) {
    case TicketStatus::Yellow:
        statusItem->setBackground(QBrush(Qt::yellow));
        statusItem->setText("Повторить");
        break;
    case TicketStatus::Green:
        statusItem->setBackground(QBrush(Qt::green));
        statusItem->setText("Выполнено");
        break;
    case TicketStatus::Default:
    default:
        statusItem->setBackground(QBrush(Qt::gray));
        statusItem->setText("Не выполнено");
        break;
    }

    updateProgressBar();
}

void MainWindow::updateTicketStatus(Ticket* ticket, TicketStatus newStatus) {
    TicketStatus oldStatus = ticket->getStatus();

    if (oldStatus == TicketStatus::Default) {
        unexecutedTickets_.removeOne(ticket);
    } else if (oldStatus == TicketStatus::Yellow) {
        repeatedTickets_.removeOne(ticket);
    }

    ticket->setStatus(newStatus);

    if (newStatus == TicketStatus::Default) {
        unexecutedTickets_.append(ticket);
    } else if (newStatus == TicketStatus::Yellow) {
        repeatedTickets_.append(ticket);
    }

    if (oldStatus == TicketStatus::Green && newStatus != TicketStatus::Green) {
        completedTickets_--;
    } else if (oldStatus != TicketStatus::Green && newStatus == TicketStatus::Green) {
        completedTickets_++;
    }
}

void MainWindow::on_comboTicketStatus_currentIndexChanged(int index) {
    int row = ui->tableWidget->currentRow();
    if (row == -1) {
        return;
    }

    int ticketNumber = row + 1;
    Ticket* selectedTicket = tickets_[ticketNumber];

    TicketStatus newStatus;
    switch (index) {
    case 0:
        newStatus = TicketStatus::Default;
        break;
    case 1:
        newStatus = TicketStatus::Green;
        break;
    case 2:
        newStatus = TicketStatus::Yellow;
        break;
    }

    updateTicketStatus(selectedTicket, newStatus);
    updateTicketInTable(ticketNumber, *selectedTicket);

    if (completedTickets_ == tickets_.size()) {
        QMessageBox::StandardButton reply = QMessageBox::question(this, "Информация",
                                                                  "Поздравляю, вы успешно выполнили все билеты! Хотети продолжить повторять билеты?",
                                                                  QMessageBox::Yes | QMessageBox::No);
        if(reply == QMessageBox::Yes) {
            return;
        } else {
            QApplication::quit();
        }
    }

}

void MainWindow::on_btnNextTicket_clicked() {
    int currentRow = ui->tableWidget->currentRow();
    int rowCount = ui->tableWidget->rowCount();

    if (currentRow < rowCount - 1) {
        ui->tableWidget->setCurrentCell(currentRow + 1, 0);
        emit ui->tableWidget->itemClicked(ui->tableWidget->item(currentRow + 1, 0));
    }
}

void MainWindow::on_btnPrevTicket_clicked() {
    int currentRow = ui->tableWidget->currentRow();

    if (currentRow > 0) {
        ui->tableWidget->setCurrentCell(currentRow - 1, 0);
        on_tableWidget_itemClicked(ui->tableWidget->item(currentRow - 1, 0));
    }
}

void MainWindow::selectTicketInTableByName(Ticket* ticket) {
    for (int row = 0; row < ui->tableWidget->rowCount(); ++row) {
        if (ui->tableWidget->item(row, 0)->text() == ticket->getName()) {
            ui->tableWidget->selectRow(row);
            break;
        }
    }
}

void MainWindow::on_btnRandomTicket_clicked() {
    Ticket* selectedTicket = nullptr;

    if (!unexecutedTickets_.isEmpty()) {
        int randomIndex = QRandomGenerator::global()->bounded(unexecutedTickets_.size());
        selectedTicket = unexecutedTickets_[randomIndex];
    }
    else if (!repeatedTickets_.isEmpty()) {
        int randomIndex = QRandomGenerator::global()->bounded(repeatedTickets_.size());
        selectedTicket = repeatedTickets_[randomIndex];
    }

    if (selectedTicket != nullptr) {
        int row = selectedTicket->getNumber() - 1;

        ui->tableWidget->setCurrentCell(row, 0);

        on_tableWidget_itemClicked(ui->tableWidget->item(row, 0));
    }
}

void MainWindow::on_tableWidget_itemDoubleClicked(QTableWidgetItem *item)
{
    if(item->column() != 1) {
        return;
    }

    int row = item->row();
    int ticketNumber = row + 1;

    if (!tickets_.contains(ticketNumber)) {
        return;
    }

    Ticket* selectedTicket = tickets_[ticketNumber];
    TicketStatus currentStatus = selectedTicket->getStatus();

    if(currentStatus == TicketStatus::Default || currentStatus == TicketStatus::Yellow) {
        updateTicketStatus(selectedTicket, TicketStatus::Green);
        updateTicketInTable(ticketNumber, *selectedTicket);

        if (ui->tableWidget->currentRow() == row) {
            ui->comboTicketStatus->setCurrentIndex(1);
        }

        if (completedTickets_ == tickets_.size()) {
            QMessageBox::StandardButton reply = QMessageBox::question(this, "Информация",
                                                                      "Поздравляю, вы успешно выполнили все билеты! Хотите продолжить повторять билеты?",
                                                                      QMessageBox::Yes | QMessageBox::No);
            if(reply == QMessageBox::No) {
                QApplication::quit();
            }
        }
    }
}
