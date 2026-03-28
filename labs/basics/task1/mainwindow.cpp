#include "mainwindow.h"

#include <QHeaderView>
#include <QRandomGenerator>
#include <QTableWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setWindowTitle("Calculus Exam");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    spinBox = new QSpinBox(this);
    spinBox->setValue(3);
    layout->addWidget(spinBox);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Name", "Status"});
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    layout->addWidget(tableWidget);

    centralWidget->setLayout(layout);

    initializeQuestions(spinBox->value());

    fillTable();

    tableWidget->horizontalHeader()->setStretchLastSection(true);
    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);

    connect(tableWidget, &QTableWidget::cellDoubleClicked, this,
            &MainWindow::onCellDoubleClicked);
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this,
            &MainWindow::onSpinBoxValueChanged);

    groupBox = new QGroupBox("Ticket info", this);
    QVBoxLayout *groupBoxLayout = new QVBoxLayout(groupBox);

    numberLabel1 = new QLabel("Ticket number : ", groupBox);
    groupBoxLayout->addWidget(numberLabel1);

    numberLabel2 = new QLabel("Ticket text : ", groupBox);
    groupBoxLayout->addWidget(numberLabel2);

    textEdit = new QLineEdit(groupBox);
    connect(textEdit, &QLineEdit::editingFinished, this,
            &MainWindow::onQuestionTextChanged);
    groupBoxLayout->addWidget(textEdit);

    statusComboBox = new QComboBox(groupBox);
    statusComboBox->addItem("Default", Ticket::Status::Default);
    statusComboBox->addItem("In Progress", Ticket::Status::InProgress);
    statusComboBox->addItem("Done", Ticket::Status::Done);
    connect(statusComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onQuestionStatusChanged);
    groupBoxLayout->addWidget(statusComboBox);

    layout->addWidget(groupBox);

    setDetailsGroupBoxEnabled(false);

    next = new QPushButton(this);
    next->setText("Gimme Next!");
    connect(next, &QPushButton::clicked, this, &MainWindow::goToNextTicket);
    layout->addWidget(next);

    last = new QPushButton(this);
    last->setText("Come back!");
    connect(last, &QPushButton::clicked, this, &MainWindow::goToLastTicket);
    layout->addWidget(last);

    progressBar = new QProgressBar(this);
    progressBar->setRange(0, 100);
    progressBar->setValue(0);
    progressBar->setFormat("Progress: %p%");
    layout->addWidget(new QLabel("Overall Progress:", this));
    layout->addWidget(progressBar);
    layout->addWidget(progressBar);

    progressBarDone = new QProgressBar(this);
    progressBarDone->setRange(0, 100);
    progressBarDone->setValue(0);
    progressBarDone->setFormat("Progress: %p%");
    layout->addWidget(new QLabel("Complete Progress:", this));
    layout->addWidget(progressBarDone);
    layout->addWidget(progressBarDone);






    centralWidget->setLayout(layout);





}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::initializeQuestions(int count) {
    tickets.clear();
    for (int i = 0; i < count; ++i) {
        tickets.append(Ticket(QString("Ticket %1").arg(i + 1)));
    }
}

void MainWindow::updateTableRow(int row) {
    const Ticket &ticket = tickets[row];

    QTableWidgetItem *questionItem = new QTableWidgetItem(ticket.getText());
    tableWidget->setItem(row, 0, questionItem);

    QTableWidgetItem *statusItem = new QTableWidgetItem(ticket.getStatusText());
    statusItem->setBackground(ticket.getStatusColor());
    statusItem->setTextAlignment(Qt::AlignCenter);
    tableWidget->setItem(row, 1, statusItem);
}

void MainWindow::fillTable() {
    tableWidget->setRowCount(tickets.size());

    for (int row = 0; row < tickets.size(); row++) {
        updateTableRow(row);
    }
}

void MainWindow::onCellDoubleClicked(int row, int column) {
    if (column == 0) {
        currentRow = row;
        rows.push(row);
        updateDetailsGroupBox();
        setDetailsGroupBoxEnabled(true);
    } else if (column == 1) {
        if (tickets[row].getStatus() == Ticket::Status::Default) {
            numberInProgress++;
        } else if (tickets[row].getStatus() == Ticket::Status::InProgress) {
            numberInProgress--;
            numberDone++;
        } else if (tickets[row].getStatus() == Ticket::Status::Done) {
            numberInProgress++;
            numberDone--;
        }
        refreshProgressBar1();
        refreshProgressBar2();
        tickets[row].cycleStatus();

        updateTableRow(row);
    }
}

void MainWindow::resizeQuestions(int newSize) {
    int currentSize = tickets.size();

    if (newSize > currentSize) {

        for (int i = currentSize; i < newSize; ++i) {
            tickets.append(*new Ticket(QString("Ticket %1").arg(i + 1),
                                       Ticket::Status::Default, i));
        }
        refreshProgressBar1();
        refreshProgressBar2();
    } else if (newSize < currentSize) {

        for (int r = newSize; r < currentSize; r++) {
            if (tickets[r].getStatus() == Ticket::Status::InProgress) {
                numberInProgress--;
            }
            if (tickets[r].getStatus() == Ticket::Status::Done) {
                numberDone--;
            }
        }

        tickets.resize(newSize);
        refreshProgressBar1();
        refreshProgressBar2();
    }
}

void MainWindow::onSpinBoxValueChanged(int value) {
    resizeQuestions(value);

    fillTable();
}

void MainWindow::setDetailsGroupBoxEnabled(bool enabled) {
    groupBox->setEnabled(enabled);

    if (!enabled) {
        numberLabel1->setText("Ticket Number: -");
        numberLabel2->setText("Ticket Text: -");
        textEdit->clear();
        statusComboBox->setCurrentIndex(-1);
    }
}

void MainWindow::onQuestionTextChanged() {
    if (currentRow >= 0 && currentRow < tickets.size()) {
        tickets[currentRow].setText(QString(textEdit->text()));
        updateTableRow(currentRow);
        numberLabel2->setText(QString("Ticket text - " + textEdit->text()));
    }
}

void MainWindow::onQuestionStatusChanged(int index) {
    if (currentRow >= 0 && currentRow < tickets.size()) {
        Ticket::Status newStatus =
            static_cast<Ticket::Status>(statusComboBox->itemData(index).toInt());

        Ticket::Status oldStatus = tickets[currentRow].getStatus();

        if (oldStatus == Ticket::Status::Default) {
            if (newStatus == Ticket::Status::InProgress) {
                numberInProgress++;
            }
            if (newStatus == Ticket::Status::Done) {
                numberDone++;
            }
            if (newStatus == Ticket::Status::Default) {
                //nichoha!
            }
        }

        if (oldStatus == Ticket::Status::InProgress) {
            if (newStatus == Ticket::Status::InProgress) {
                //
            }
            if (newStatus == Ticket::Status::Done) {
                numberDone++;
                numberInProgress--;
            }
            if (newStatus == Ticket::Status::Default) {
                numberInProgress--;
            }
        }

        if (oldStatus == Ticket::Status::Done) {
            if (newStatus == Ticket::Status::InProgress) {
                numberDone--;
                numberInProgress++;
            }
            if (newStatus == Ticket::Status::Done) {
                //
            }
            if (newStatus == Ticket::Status::Default) {
                numberDone--;
            }
        }

        refreshProgressBar1();
        refreshProgressBar2();
        tickets[currentRow].setStatus(newStatus);

        updateTableRow(currentRow);
    }
}

void MainWindow::updateDetailsGroupBox() {
    if (currentRow >= 0 && currentRow < tickets.size()) {
        const Ticket &tick = tickets[currentRow];
        numberLabel1->setText(QString("Ticket Number: %1").arg(currentRow + 1));

        textEdit->setText("Ticket Text: " + tick.getText());
        textEdit->setText(tick.getText());

        statusComboBox->setCurrentIndex(tick.getStatus());
    }
}

void MainWindow::goToNextTicket() {
    QRandomGenerator *random = new QRandomGenerator;
    int nextIndex = QRandomGenerator::global()->bounded(tickets.size());
    int i = 0;
    while ((tickets[nextIndex].getStatus() == Ticket::Status::Default ||
            nextIndex == currentRow) &&
           i <= tickets.size()) {
        nextIndex = random->bounded(tickets.size());
        i++;
    }
    if (tickets[nextIndex].getStatus() == Ticket::Status::Default) {
        return;
    } else {
        rows.push(nextIndex);
        currentRow = nextIndex;
        tableWidget->setCurrentCell(currentRow, 0);
        updateDetailsGroupBox();
        setDetailsGroupBoxEnabled(true);
    }
}

void MainWindow::goToLastTicket() {
    if (rows.size() > 1) {
        rows.pop();
        currentRow = rows.top();
        tableWidget->setCurrentCell(currentRow, 0);
        updateDetailsGroupBox();
        setDetailsGroupBoxEnabled(true);
    }
}

void MainWindow::refreshProgressBar1() {
    int totalQuestions = tickets.size();
    int progressPercentage = 0;
    if (totalQuestions > 0) {
        progressPercentage = ((numberInProgress + numberDone) * 100) / totalQuestions;
    }

    // Update the progress bar
    progressBar->setValue(progressPercentage);
    progressBar->setFormat(QString("Progress: %p%"));
}


void MainWindow::refreshProgressBar2() {
    int totalQuestions = tickets.size();
    int progressPercentage = 0;
    if (totalQuestions > 0) {
        progressPercentage = ((numberDone) * 100) / totalQuestions;
    }

    // Update the progress bar
    progressBarDone->setValue(progressPercentage);
    progressBarDone->setFormat(QString("Progress: %p%"));
}

