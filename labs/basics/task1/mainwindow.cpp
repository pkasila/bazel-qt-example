#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QRandomGenerator>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    setupUI();
    onCountChanged(m_countSpinBox->value());
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    QVBoxLayout *leftLayout = new QVBoxLayout();
    m_countSpinBox = new QSpinBox();
    m_countSpinBox->setRange(1, 100);
    m_countSpinBox->setValue(10);

    m_viewTable = new QTableWidget();
    m_viewTable->setColumnCount(1);
    m_viewTable->horizontalHeader()->setStretchLastSection(true);
    m_viewTable->horizontalHeader()->hide();
    m_viewTable->setEditTriggers(QAbstractItemView::NoEditTriggers);

    m_totalProgress = new QProgressBar();
    m_greenProgress = new QProgressBar();
    m_greenProgress->setStyleSheet("QProgressBar::chunk { background-color: #4B952A; }");

    leftLayout->addWidget(new QLabel("Tickets number:"));
    leftLayout->addWidget(m_countSpinBox);
    leftLayout->addWidget(m_viewTable);
    leftLayout->addWidget(new QLabel("Progress:"));
    leftLayout->addWidget(m_totalProgress);
    leftLayout->addWidget(new QLabel("Done only:"));
    leftLayout->addWidget(m_greenProgress);

    m_questionGroup = new QGroupBox("Current ticket");
    QVBoxLayout *rightLayout = new QVBoxLayout(m_questionGroup);

    m_numLabel = new QLabel("Number: -");
    m_nameLabel = new QLabel("Name: -");
    m_nameEdit = new QLineEdit();
    m_nameEdit->setPlaceholderText("Enter new name...");

    m_statusCombo = new QComboBox();
    m_statusCombo->addItems({"To do", "Revise", "Done"});

    QHBoxLayout *navLayout = new QHBoxLayout();
    m_prevBtn = new QPushButton("Previous ticket");
    m_nextBtn = new QPushButton("Random ticket");
    navLayout->addWidget(m_prevBtn);
    navLayout->addWidget(m_nextBtn);

    rightLayout->addWidget(m_numLabel);
    rightLayout->addWidget(m_nameLabel);
    rightLayout->addWidget(new QLabel("Edit name(Enter):"));
    rightLayout->addWidget(m_nameEdit);
    rightLayout->addWidget(new QLabel("Status:"));
    rightLayout->addWidget(m_statusCombo);
    rightLayout->addStretch();
    rightLayout->addLayout(navLayout);

    mainLayout->addLayout(leftLayout, 1);
    mainLayout->addWidget(m_questionGroup, 1);

connect(m_countSpinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, [this](int count){
    onCountChanged(count);
});
connect(m_viewTable, &QTableWidget::cellClicked, this, [this](int row, int column){
    onCellClicked(row, column);
});
connect(m_viewTable, &QTableWidget::cellDoubleClicked, this, [this](int row, int column){
    onCellDoubleClicked(row, column);
});
connect(m_nameEdit, &QLineEdit::returnPressed, this, [this](){
    onNameEdited();
});
connect(m_statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this, [this](int index){
    onStatusChanged(index);
});
connect(m_nextBtn, &QPushButton::clicked, this, [this](){
    nextRandomTicket();
});
connect(m_prevBtn, &QPushButton::clicked, this, [this](){
    previousTicket();
});
}

void MainWindow::onCountChanged(int count) {
    m_tickets.clear();
    m_history.clear();
    m_currentIndex = -1;
    m_viewTable->setRowCount(count);

    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.name = QString("Ticket %1").arg(i + 1);
        m_tickets.append(t);

        QTableWidgetItem *item = new QTableWidgetItem(t.name);
        m_viewTable->setItem(i, 0, item);
        applyStatusColor(i, TicketStatus::Default);
    }
    updateProgress();
    updateQuestionView(-1);
}

void MainWindow::onCellClicked(int row, int column) {
    if (row == m_currentIndex) return; 
    if (m_currentIndex != -1) {
        m_history.push(m_currentIndex);
    }
    updateQuestionView(row);
    m_prevBtn->setEnabled(!m_history.isEmpty());}

void MainWindow::onCellDoubleClicked(int row, int column) {
    if (m_tickets[row].status == TicketStatus::Green) {
        m_tickets[row].status = TicketStatus::Yellow;
    } else {
        m_tickets[row].status = TicketStatus::Green;
    }
    applyStatusColor(row, m_tickets[row].status);
    updateProgress();
    if (m_currentIndex == row) updateQuestionView(row);
}

void MainWindow::onNameEdited() {
    if (m_currentIndex != -1 && !m_nameEdit->text().isEmpty()) {
        m_tickets[m_currentIndex].name = m_nameEdit->text();
        m_nameLabel->setText(m_tickets[m_currentIndex].name);
        m_viewTable->item(m_currentIndex, 0)->setText(m_tickets[m_currentIndex].name);
        m_nameEdit->clear();
    }
}

void MainWindow::onStatusChanged(int index) {
    if (m_currentIndex == -1 || m_blockSignals) return;
    m_tickets[m_currentIndex].status = static_cast<TicketStatus>(index);
    applyStatusColor(m_currentIndex, m_tickets[m_currentIndex].status);
    updateProgress();
}

void MainWindow::nextRandomTicket() {
    QVector<int> available;
    for (int i = 0; i < m_tickets.size(); ++i) {
        if (m_tickets[i].status != TicketStatus::Green) {
            available.append(i);
        }
    }

    if (available.isEmpty()) {
        QMessageBox::information(this, "ALL DONE!", "GOOD LUCK ON THE EXAM!");
        return;
    }

    if (m_currentIndex != -1) {
        m_history.push(m_currentIndex);
    }

    int randomIndex = available[QRandomGenerator::global()->bounded(available.size())];
    m_viewTable->setCurrentCell(randomIndex, 0);
    updateQuestionView(randomIndex);    
    m_prevBtn->setEnabled(!m_history.isEmpty());
}
void MainWindow::previousTicket() {
    if (m_history.isEmpty()) return;
    int prevIndex = m_history.pop();
    m_viewTable->setCurrentCell(prevIndex, 0);
    updateQuestionView(prevIndex);
    m_prevBtn->setEnabled(!m_history.isEmpty());
}

void MainWindow::updateQuestionView(int index) {
    m_currentIndex = index;
    m_blockSignals = true;

    if (index == -1) {
        m_numLabel->setText("Number: -");
        m_nameLabel->setText("Name -");
        m_statusCombo->setCurrentIndex(0);
        m_questionGroup->setEnabled(false);
    } else {
        m_questionGroup->setEnabled(true);
        m_numLabel->setText(QString("Number: %1").arg(index + 1));
        m_nameLabel->setText(m_tickets[index].name);
        m_statusCombo->setCurrentIndex(static_cast<int>(m_tickets[index].status));
    }
    m_blockSignals = false;
}

void MainWindow::applyStatusColor(int row, TicketStatus status) {
    QColor color;
    switch (status) {
        case TicketStatus::Default: color = Qt::lightGray; break;
        case TicketStatus::Yellow:  color = QColor("#c7ad03"); break;
        case TicketStatus::Green:   color = QColor("#4B952A"); break;
    }
    m_viewTable->item(row, 0)->setBackground(color);
}

void MainWindow::updateProgress() {
    int total = m_tickets.size();
    int yellowOrGreen = 0;
    int greenOnly = 0;

    for (const auto &t : m_tickets) {
        if (t.status == TicketStatus::Yellow || t.status == TicketStatus::Green) yellowOrGreen++;
        if (t.status == TicketStatus::Green) greenOnly++;
    }

    m_totalProgress->setValue((yellowOrGreen * 100) / total);
    m_greenProgress->setValue((greenOnly * 100) / total);
}