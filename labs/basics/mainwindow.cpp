 #include "mainwindow.h"

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {

    //количество билетов
    spinBox = new QSpinBox(this);
    spinBox->setRange(0, 100);
    spinBox->setValue(0);
    connect(spinBox, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::onSpinBoxValueChanged);

    //поля с билетами
    listWidget = new QListWidget(this);
    connect(listWidget, &QListWidget::itemClicked, this, &MainWindow::onListItemClicked);
    connect(listWidget, &QListWidget::itemDoubleClicked, this, &MainWindow::onListItemDoubleClicked);
    listWidget->setStyleSheet("QListWidget { border: 1px solid grey; border-radius: 5px; background-color: dark gray; }");
    listWidget->setSelectionMode(QAbstractItemView::NoSelection);

    //названия полей
    nameLabel = new QLabel("Имя билета: ", this);
    numberLabel = new QLabel("Номер билета: ", this);
    nameEdit = new QLineEdit(this);
    nameEdit->setStyleSheet("QLineEdit { border: 1px solid grey; border-radius: 5px; background-color: dark  gray; }");
    connect(nameEdit, &QLineEdit::returnPressed, this, &MainWindow::onNameEditEnterPressed);

    //статус изучения
    comboBox = new QComboBox(this);
    comboBox->addItems({"Дефолтный", "Желтый", "Зеленый"});
    comboBox->setStyleSheet("QComboBox { text-align: center; }"
        "QComboBox QAbstractItemView { text-align: center; }");
    connect(comboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::onStatusChanged);

    //переключение по билетам
    nextQuestionButton = new QPushButton("Следующий билет", this);
    connect(nextQuestionButton, &QPushButton::clicked, this, &MainWindow::onNextQuestionClicked);
    previousQuestionButton = new QPushButton("Предыдущий билет", this);
    connect(previousQuestionButton, &QPushButton::clicked, this, &MainWindow::onPreviousQuestionClicked);

    //плашка прогресса
    totalProgress = new QProgressBar(this);
    totalProgress->setRange(0, 100);
    totalProgress->setValue(0);
    greenProgress = new QProgressBar(this);
    greenProgress->setRange(0, 100);
    greenProgress->setValue(0);
    totalProgress->setStyleSheet(
        "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center;}"
        );

    greenProgress->setStyleSheet(
        "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center;}"
        );

    //доп кнопки
    showHintButton = new QPushButton("Показать подсказку", this);
    connect(showHintButton, &QPushButton::clicked, this, &MainWindow::onShowHintClicked);
    saveProgressButton = new QPushButton("Сохранить прогресс", this);
    connect(saveProgressButton, &QPushButton::clicked, this, &MainWindow::onSaveProgressClicked);
    loadProgressButton = new QPushButton("Загрузить прогресс", this);
    connect(loadProgressButton, &QPushButton::clicked, this, &MainWindow::onLoadProgressClicked);

    QWidget* window = new QWidget(this);
    setCentralWidget(window);

    //статус изучения и подсказка в одну строку
    QHBoxLayout* hlayout1 = new QHBoxLayout;
    hlayout1->addWidget(comboBox);
    hlayout1->addWidget(showHintButton);

    QHBoxLayout* hlayout2 = new QHBoxLayout;

    QVBoxLayout* wlayout1 = new QVBoxLayout;
    wlayout1->addWidget(nextQuestionButton);
    wlayout1->addWidget(previousQuestionButton);
    wlayout1->addStretch(1);
    wlayout1->addWidget(saveProgressButton);
    wlayout1->addWidget(loadProgressButton);

    QHBoxLayout* hlayout3 = new QHBoxLayout;
    hlayout3->addWidget(listWidget);
    hlayout3->addLayout(wlayout1);
    hlayout3->setStretch(0,3);
    hlayout3->setStretch(0,1);


    QVBoxLayout* layout = new QVBoxLayout(window);
    layout->addWidget(spinBox);
    layout->addLayout(hlayout3);
    layout->addWidget(nameLabel);
    layout->addWidget(numberLabel);
    layout->addWidget(nameEdit);
    layout->addLayout(hlayout1);
    layout->addWidget(totalProgress);
    layout->addWidget(greenProgress);
    layout->addLayout(hlayout2);
}

void MainWindow::clearTicketInfo() {
    if (!listWidget->selectedItems().isEmpty()) {
        return;
    }
    selectedItem = nullptr;
    nameLabel->setText("Имя билета: ");
    numberLabel->setText("Номер билета: ");
    nameEdit->clear();
    comboBox->setCurrentIndex(0);
}

void MainWindow::mousePressEvent(QMouseEvent *event) {
    if (!listWidget->geometry().contains(event->pos())) {
        clearTicketInfo();
    }
    QMainWindow::mousePressEvent(event);
}

void MainWindow::onSaveProgressClicked() {
    QFile file("progress.txt");
    if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QTextStream out(&file);
        for (auto item : ticketStatuses.keys()) {
            out << ticketNumbers[item] << ";" << ticketStatuses[item] << ";" << ticketHints[item] << "\n";
        }
        file.close();
        QMessageBox::information(this, "Сохранение", "Прогресс успешно сохранен.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось сохранить прогресс.");
    }
}

void MainWindow::onLoadProgressClicked() {
    QFile file("progress.txt");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QTextStream in(&file);
        ticketStatuses.clear();
        ticketNumbers.clear();
        ticketHints.clear();
        listWidget->clear();

        while (!in.atEnd()) {
            QString line = in.readLine();
            QStringList parts = line.split(";");
            if (parts.size() == 3) {
                int number = parts[0].toInt();
                int status = parts[1].toInt();
                QString hint = parts[2];

                QListWidgetItem *item = new QListWidgetItem("Билет " + QString::number(number), listWidget);
                ticketStatuses[item] = status;
                ticketNumbers[item] = number;
                ticketHints[item] = hint;
                updateItemColor(item);
            }
        }
        file.close();
        QMessageBox::information(this, "Загрузка", "Прогресс успешно загружен.");
    } else {
        QMessageBox::warning(this, "Ошибка", "Не удалось загрузить прогресс.");
    }
}

void MainWindow::onItemDoubleClicked(QListWidgetItem *item) {
    if (item) {
        bool ok;
        QString hint = QInputDialog::getText(this, "Добавить подсказку", "Введите подсказку:", QLineEdit::Normal, ticketHints[item], &ok);
        if (ok) {
            ticketHints[item] = hint;
        }
    }
}

void MainWindow::onSpinBoxValueChanged(int value) {
    selectedItem = nullptr;
    listWidget->clear();
    ticketStatuses.clear();
    ticketNumbers.clear();
    ticketHints.clear();
    totalCount = value;
    totalLearned = 0;
    greenLearned = 0;

    for (int i = 1; i <= value; ++i) {
        QListWidgetItem* item = new QListWidgetItem("Билет " + QString::number(i), listWidget);
        ticketStatuses[item] = 0;
        ticketNumbers[item] = i;
        updateItemColor(item);
    }

    updateProgressBars();
    clearTicketInfo();
}

void MainWindow::onNameEditEnterPressed() {
    if (selectedItem) {
        QString newName = nameEdit->text().trimmed();
        if (!newName.isEmpty()) {
            selectedItem->setText(newName);
            nameLabel->setText("Имя билета: " + newName);
            listWidget->setCurrentItem(selectedItem);
            listWidget->setFocus();
        }
    }
}

void MainWindow::onStatusChanged(int index) {
    if(selectedItem){
        int prevStatus = ticketStatuses[selectedItem];
        ticketStatuses[selectedItem] = index;
        updateItemColor(selectedItem);
        if (prevStatus == 0 && (index == 1 || index == 2)) {
            totalLearned++;
        } else if ((prevStatus == 1 || prevStatus == 2) && index == 0) {
            totalLearned--;
        }

        if (prevStatus != 2 && index == 2) {
            greenLearned++;
        } else if (prevStatus == 2 && index != 2) {
            greenLearned--;
        }

        updateProgressBars();
        listWidget->setCurrentItem(selectedItem);
        listWidget->setFocus();
    }
}

void MainWindow::onListItemDoubleClicked(QListWidgetItem* item) {
    if (item) {
        int currentStatus = ticketStatuses[item];
        int newStatus;
        if (currentStatus == 0 || currentStatus == 1) {
            newStatus = 2;
        } else {
            newStatus = 1;
        }
        if (currentStatus == 0 && newStatus == 2) {
            totalLearned++;
            greenLearned++;
        } else if (currentStatus == 1 && newStatus == 2) {
            greenLearned++;
        } else if (currentStatus == 2 && newStatus == 1) {
            greenLearned--;
        } else if (currentStatus == 2 && newStatus == 0) {
            totalLearned--;
            greenLearned--;
        }

        ticketStatuses[item] = newStatus;
        updateItemColor(item);
        updateProgressBars();
        comboBox->setCurrentIndex(newStatus);
    }
}

void MainWindow::onListItemClicked(QListWidgetItem* item) {
    if (item && item != selectedItem) {
        if (selectedItem) {
            historyStack.push(selectedItem);
        }
        selectedItem = item;
        nameLabel->setText("Имя билета: " + item->text());
        nameEdit->setText(item->text());
        int ticketNumber = ticketNumbers[item];
        numberLabel->setText("Номер билета: " + QString::number(ticketNumber));
        comboBox->setCurrentIndex(ticketStatuses[item]);
    }
}

void MainWindow::updateItemColor(QListWidgetItem* item) {
    if (!item) return;
    int status = ticketStatuses[item];
    switch (status) {
        case 0:
            item->setBackground(QBrush(Qt::gray));
            item->setForeground(QBrush(Qt::white));
            break;
        case 1:
            item->setBackground(QBrush(Qt::yellow));
            item->setForeground(QBrush(Qt::black));
            break;
        case 2:
            item->setBackground(QBrush(Qt::green));
            item->setForeground(QBrush(Qt::black));
            break;
    }

    listWidget->update();
}

void MainWindow::onShowHintClicked() {
    if (selectedItem) {
        if (ticketHints.contains(selectedItem)) {
            QMessageBox::information(this, "Подсказка", ticketHints[selectedItem]);
        } else {
            QMessageBox msgBox(this);
            msgBox.setWindowTitle("Подсказка отсутствует");
            msgBox.setText("Подсказка для этого билета отсутствует. Хотите добавить подсказку?");
            QPushButton *addButton = msgBox.addButton("Добавить подсказку", QMessageBox::ActionRole);
            //QPushButton *cancelButton = msgBox.addButton("Отмена", QMessageBox::RejectRole);

            msgBox.exec();

            if (msgBox.clickedButton() == addButton) {
                bool ok;
                QString hint = QInputDialog::getText(this, "Добавить подсказку", "Введите подсказку:", QLineEdit::Normal, "", &ok);

                if (ok && !hint.isEmpty()) {
                    ticketHints[selectedItem] = hint;
                    QMessageBox::information(this, "Подсказка", "Подсказка добавлена.");
                }
            }
        }
        listWidget->setCurrentItem(selectedItem);
        listWidget->setFocus();
    } else {
        QMessageBox::information(this, "Подсказка", "Выберите билет для добавления подсказки.");
    }
}

void MainWindow::onNextQuestionClicked() {
    QList<QListWidgetItem*> defaultOrYellowTickets;

    for (auto item : ticketStatuses.keys()) {
        int status = ticketStatuses[item];
        if (status == 0 || status == 1) {
            defaultOrYellowTickets.append(item);
        }
    }

    if (!defaultOrYellowTickets.isEmpty()) {
        if (selectedItem) {
            historyStack.push(selectedItem);
        }
        int randomIndex = QRandomGenerator::global()->bounded(defaultOrYellowTickets.size());
        selectedItem = defaultOrYellowTickets[randomIndex];

        listWidget->setCurrentItem(selectedItem);
        nameLabel->setText("Имя билета: " + selectedItem->text());
        nameEdit->setText(selectedItem->text());
        numberLabel->setText("Номер билета: " + QString::number(ticketNumbers[selectedItem]));
        comboBox->setCurrentIndex(ticketStatuses[selectedItem]);
        listWidget->setFocus();
    }
}

void MainWindow::onPreviousQuestionClicked() {
    if (!historyStack.isEmpty()) {
        QListWidgetItem* prevItem = historyStack.pop();
        if (prevItem) {
            listWidget->setCurrentItem(prevItem);
            if (prevItem && prevItem != selectedItem) {
                selectedItem = prevItem;
                nameLabel->setText("Имя билета: " + prevItem->text());
                nameEdit->setText(prevItem->text());
                int ticketNumber = ticketNumbers[prevItem];
                numberLabel->setText("Номер билета: " + QString::number(ticketNumber));
                comboBox->setCurrentIndex(ticketStatuses[prevItem]);
            }
        }
        listWidget->setFocus();
    }
}


void MainWindow::updateProgressBars() {
    if (totalCount > 0) {
        int totalValue = (totalLearned * 100) / totalCount;
        int greenValue = (greenLearned * 100) / totalCount;

        totalProgress->setValue(totalValue);
        greenProgress->setValue(greenValue);
    if (totalValue < 33) {
        totalProgress->setStyleSheet(
            "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center;}"
            "QProgressBar::chunk { background-color: red; }"
            );
    } else if (totalValue < 66) {
        totalProgress->setStyleSheet(
            "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center;}"
            "QProgressBar::chunk { background-color: orange; }"
            );
    } else {
        totalProgress->setStyleSheet(
            "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center; color: white;}"
            "QProgressBar::chunk { background-color: dark yellow; }"
            );
    }

    if (greenValue < 33) {
        greenProgress->setStyleSheet(
            "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center;}"
            "QProgressBar::chunk { background-color: orange; }"
            );
    } else if (greenValue < 66) {
        greenProgress->setStyleSheet(
            "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center;}"
            "QProgressBar::chunk { background-color: green; }"
            );
    } else {
        greenProgress->setStyleSheet(
            "QProgressBar { border: 2px solid grey; border-radius: 5px; text-align: center; color: white;}"
            "QProgressBar::chunk { background-color: darkgreen; }"
            );
    }
} else {
    totalProgress->setValue(0);
    greenProgress->setValue(0);
}
}
