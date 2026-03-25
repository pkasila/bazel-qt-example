#include "main_window.h"

#include <QListWidgetItem>
#include <QRandomGenerator>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
    countSpinBox_ = new QSpinBox(this);
    countSpinBox_->setRange(0, 1000);
    countSpinBox_->setValue(0);

    listView_ = new QListWidget(this);

    questionView_ = new QGroupBox("Текущий билет", this);
    numberLabel_ = new QLabel("Номер:", this);
    nameLabel_ = new QLabel("Имя:", this);
    nameEdit_ = new QLineEdit(this);
    nameEdit_->setPlaceholderText("Новое имя билета");

    statusCombo_ = new QComboBox(this);
    statusCombo_->addItems({"Дефолтный", "Желтый", "Зеленый"});

    nextQuestionBtn_ = new QPushButton("Следующий", this);
    prevQuestionBtn_ = new QPushButton("Предыдущий", this);
    prevQuestionBtn_->setEnabled(false);

    totalProgress_ = new QProgressBar(this);
    totalProgress_->setFormat("Общий прогресс: %p%");

    greenProgress_ = new QProgressBar(this);
    greenProgress_->setFormat("Готово: %p%");

    QVBoxLayout* mainLayout = new QVBoxLayout(this);

    // Top layout (SpinBox)
    QHBoxLayout* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Количество билетов:"));
    topLayout->addWidget(countSpinBox_);
    mainLayout->addLayout(topLayout);

    // List and Question view
    QHBoxLayout* middleLayout = new QHBoxLayout();
    middleLayout->addWidget(listView_, 1);

    QVBoxLayout* questionLayout = new QVBoxLayout();
    questionLayout->addWidget(numberLabel_);
    questionLayout->addWidget(nameLabel_);
    questionLayout->addWidget(nameEdit_);
    questionLayout->addWidget(new QLabel("Статус:"));
    questionLayout->addWidget(statusCombo_);
    questionLayout->addStretch();
    questionView_->setLayout(questionLayout);

    middleLayout->addWidget(questionView_, 1);
    mainLayout->addLayout(middleLayout);

    // Buttons
    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addWidget(prevQuestionBtn_);
    btnLayout->addWidget(nextQuestionBtn_);
    mainLayout->addLayout(btnLayout);

    // Progress
    mainLayout->addWidget(totalProgress_);
    mainLayout->addWidget(greenProgress_);

    connect(
        countSpinBox_, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &MainWindow::onCountChanged);
    connect(listView_, &QListWidget::itemClicked, this, &MainWindow::onItemClicked);
    connect(listView_, &QListWidget::itemDoubleClicked, this, &MainWindow::onItemDoubleClicked);
    connect(nameEdit_, &QLineEdit::returnPressed, this, &MainWindow::onNameEditReturnPressed);
    connect(
        statusCombo_, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &MainWindow::onStatusChanged);
    connect(nextQuestionBtn_, &QPushButton::clicked, this, &MainWindow::onNextClicked);
    connect(prevQuestionBtn_, &QPushButton::clicked, this, &MainWindow::onPrevClicked);

    // Initialize state
    questionView_->setEnabled(false);
}

void MainWindow::onCountChanged(int count) {
    tickets_.resize(count);
    for (int i = 0; i < count; ++i) {
        tickets_[i].name = QString("Билет %1").arg(i + 1);
        tickets_[i].status = 0;
    }

    listView_->clear();
    for (int i = 0; i < count; ++i) {
        QListWidgetItem* item = new QListWidgetItem(tickets_[i].name);
        listView_->addItem(item);
        updateItemColor(i);
    }

    currentIndex_ = -1;
    history_.clear();
    prevQuestionBtn_->setEnabled(false);
    questionView_->setEnabled(false);
    updateProgress();
}

void MainWindow::updateItemColor(int index) {
    if (index < 0 || index >= static_cast<int>(tickets_.size())) {
        return;
    }
    QListWidgetItem* item = listView_->item(index);
    if (!item) {
        return;
    }

    if (tickets_[index].status == 0) {
        item->setBackground(Qt::white);
    } else if (tickets_[index].status == 1) {
        item->setBackground(Qt::yellow);
    } else if (tickets_[index].status == 2) {
        item->setBackground(Qt::green);
    }
}

void MainWindow::onItemClicked(QListWidgetItem* item) {
    int index = listView_->row(item);
    showQuestion(index);
}

void MainWindow::showQuestion(int index) {
    if (index < 0 || index >= static_cast<int>(tickets_.size())) {
        questionView_->setEnabled(false);
        return;
    }

    currentIndex_ = index;
    questionView_->setEnabled(true);
    numberLabel_->setText(QString("Номер: %1").arg(index + 1));
    nameLabel_->setText(QString("Имя: %1").arg(tickets_[index].name));
    nameEdit_->clear();
    statusCombo_->setCurrentIndex(tickets_[index].status);

    listView_->setCurrentRow(index);
}

void MainWindow::onItemDoubleClicked(QListWidgetItem* item) {
    int index = listView_->row(item);
    if (index < 0 || index >= static_cast<int>(tickets_.size())) {
        return;
    }

    if (tickets_[index].status == 0 || tickets_[index].status == 1) {
        tickets_[index].status = 2;
    } else if (tickets_[index].status == 2) {
        tickets_[index].status = 1;
    }

    updateItemColor(index);
    updateProgress();
    if (currentIndex_ == index) {
        statusCombo_->setCurrentIndex(tickets_[index].status);
    }
}

void MainWindow::onNameEditReturnPressed() {
    if (currentIndex_ < 0 || currentIndex_ >= static_cast<int>(tickets_.size())) {
        return;
    }
    QString newName = nameEdit_->text().trimmed();
    if (!newName.isEmpty()) {
        tickets_[currentIndex_].name = newName;
        nameLabel_->setText(QString("Имя: %1").arg(newName));
        listView_->item(currentIndex_)->setText(newName);
    }
    nameEdit_->clear();
}

void MainWindow::onStatusChanged(int index) {
    if (currentIndex_ < 0 || currentIndex_ >= static_cast<int>(tickets_.size())) {
        return;
    }
    if (tickets_[currentIndex_].status != index) {
        tickets_[currentIndex_].status = index;
        updateItemColor(currentIndex_);
        updateProgress();
    }
}

void MainWindow::onNextClicked() {
    std::vector<int> available;
    for (size_t i = 0; i < tickets_.size(); ++i) {
        if (tickets_[i].status == 0 || tickets_[i].status == 1) {
            available.push_back(i);
        }
    }

    if (available.empty()) {
        return;
    }

    int r = QRandomGenerator::global()->bounded(static_cast<int>(available.size()));
    int nextId = available[r];

    if (currentIndex_ != -1) {
        history_.push_back(currentIndex_);
        prevQuestionBtn_->setEnabled(true);
    }

    showQuestion(nextId);
}

void MainWindow::onPrevClicked() {
    if (history_.empty()) {
        return;
    }
    int prevId = history_.back();
    history_.pop_back();

    if (history_.empty()) {
        prevQuestionBtn_->setEnabled(false);
    }

    showQuestion(prevId);
}

void MainWindow::updateProgress() {
    int totalCount = tickets_.size();
    if (totalCount == 0) {
        totalProgress_->setValue(0);
        totalProgress_->setMaximum(100);
        greenProgress_->setValue(0);
        greenProgress_->setMaximum(100);
        return;
    }

    int totalDone = 0;  // Yellow + Green
    int greenDone = 0;  // Green only

    for (const auto& t : tickets_) {
        if (t.status == 1 || t.status == 2) {
            totalDone++;
        }
        if (t.status == 2) {
            greenDone++;
        }
    }

    totalProgress_->setMaximum(totalCount);
    totalProgress_->setValue(totalDone);

    greenProgress_->setMaximum(totalCount);
    greenProgress_->setValue(greenDone);
}
