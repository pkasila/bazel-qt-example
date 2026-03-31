#include "mainwindow.h"
//#include "ui_mainwindow.h"

#include <QFile>
#include <QFileDialog>
#include <QGridLayout>
#include <QMessageBox>
#include <QRandomGenerator>
#include <QTableWidgetItem>
#include <QTextStream>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    setupUi(this); // Use setupUi(this) if you still use auto-generated ui setup

    setWindowTitle("Билеты");

    QWidget* centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout* mainLayout = new QGridLayout(centralWidget);

    QVBoxLayout* contentLayout = new QVBoxLayout();

    //number->setWordWrap(true);
    contentLayout->addWidget(number);

    QHBoxLayout* buttonStatusLayout = new QHBoxLayout();
    buttonStatusLayout->addWidget(previous);
    buttonStatusLayout->addWidget(next);
    buttonStatusLayout->addWidget(random);
    buttonStatusLayout->addWidget(status);
    contentLayout->addLayout(buttonStatusLayout);

    QHBoxLayout* progressLayout = new QHBoxLayout();
    progressLayout->addWidget(total);
    progressLayout->addWidget(green);
    contentLayout->addLayout(progressLayout);

    mainLayout->addLayout(contentLayout, 0, 0);

    View->setMaximumWidth(300);
    mainLayout->addWidget(View, 0, 1);

    Count->setToolTip("Количество билетов");
    mainLayout->addWidget(Count, 1, 1, Qt::AlignRight | Qt::AlignBottom);

    connect(Count, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_CountvalueChanged);
    connect(View, &QTableWidget::cellClicked, this, &MainWindow::on_ViewcellClicked);
    connect(View, &QTableWidget::cellDoubleClicked, this, &MainWindow::on_ViewcellDoubleClicked);
    connect(next, &QPushButton::clicked, this, &MainWindow::on_nextclicked);
    connect(random, &QPushButton::clicked, this, &MainWindow::on_randomclicked);
    connect(previous, &QPushButton::clicked, this, &MainWindow::on_previousclicked);
    connect(name_edit, &QLineEdit::returnPressed, this, &MainWindow::on_name_edit_returnPressed);
    connect(status, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_statuscurrentIndexChanged);

    QPushButton* resetButton = new QPushButton("Сброс", this);
    resetButton->setToolTip("Сбросить все билеты в начальное состояние");
    statusbar->addPermanentWidget(resetButton);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::on_resetclicked);

    QAction* saveAction = new QAction("Сохранить", this);
    QAction* loadAction = new QAction("Загрузить", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::on_savetriggered);
    connect(loadAction, &QAction::triggered, this, &MainWindow::on_loadtriggered);

    menubar->addAction(saveAction);
    menubar->addAction(loadAction);

    number->setWordWrap(true);
    number->setMaximumWidth(400);
    status->addItems({"Default", "Yellow", "Green"});
}

void MainWindow::on_CountvalueChanged(int value) {
    if (value <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество билетов должно быть больше 0.");
        return;
    }

    View->setRowCount(value);
    ticketStatus.resize(value);
    ticketNames.resize(value);
    ticketContents.resize(value);

    for (int i = 0; i < value; ++i) {
        QTableWidgetItem* item = new QTableWidgetItem("Билет " + QString::number(i + 1));
        item->setBackground(Qt::gray);
        View->setItem(i, 0, item);
        ticketStatus[i] = 0;
        ticketNames[i] = "Билет " + QString::number(i + 1);
        ticketContents = {
            "1. Критерий различия действительных чисел.",
            "2. Теорема о плотности множества действительных чисел.",
            "3. Несчетность множества действительных чисел.",
            "4. Теорема о гранях.",
            "5. Определение предела последовательности и M-лемма.",
            "6. Единственность предела и ограниченность сходящейся последовательности.",
            "7. Сумма, произведение и частное пределов.",
            "8. Лемма о сжатой последовательности.",
            "9. Сходимость монотонной ограниченной последовательности.",
            "10. Существование монотонной подпоследовательности и принцип выбора.",
            "11. Число e.",
            "12. Критерий Коши для предела последовательности.",
            "13. Определение предела функции и M-лемма.",
            "14. Критерий Гейне для предела функции.",
            "15. Критерий Коши для предела функции.",
            "16. Определение непрерывности функции и типы точек разрыва.",
            "17. Критерий Гейне непрерывности.",
            "18. Критерий непрерывности монотонной функции.",
            "19. Непрерывность обратной функции.",
            "20. Непрерывность сложной функции.",
            "21. Теорема о стабилизации знака.",
            "22. Теорема о локальной ограниченности непрерывной функции.",
            "23. Непрерывность суммы, произведения, частного функций.",
            "24. Теорема о промежуточных значениях непрерывной функции.",
            "25. Теорема Вейерштрасса."};
    }

    updateProgress();
}

void MainWindow::on_ViewcellClicked(int row, int column) {
    Q_UNUSED(column);
    displayTicket(row);
}

MainWindow::~MainWindow() {}

void MainWindow::on_ViewcellDoubleClicked(int row, int column) {
    Q_UNUSED(column);
    QTableWidgetItem* item = View->item(row, 0);
    if (ticketStatus[row] == 0) {
        item->setBackground(Qt::yellow);
        ticketStatus[row] = 1;
    } else if (ticketStatus[row] == 1) {
        item->setBackground(Qt::green);
        ticketStatus[row] = 2;
    } else if (ticketStatus[row] == 2) {
        item->setBackground(Qt::yellow);
        ticketStatus[row] = 1;
    }

    updateProgress();
}

void MainWindow::on_nextclicked() {
    int nextRow = currentTicket + 1;
    while (nextRow < ticketStatus.size()) {
        if (ticketStatus[nextRow] == 0 || ticketStatus[nextRow] == 1) break;
        ++nextRow;
    }
    if (nextRow < ticketStatus.size()) {
        View->setCurrentCell(nextRow, 0);
        displayTicket(nextRow);
    } else {
        QMessageBox::information(this, "Информация", "Все билеты выше уже изучены.");
    }
}

void MainWindow::on_previousclicked() {
    int prevRow = currentTicket - 1;
    while (prevRow >= 0) {
        if (ticketStatus[prevRow] == 0 || ticketStatus[prevRow] == 1) break;
        --prevRow;
    }
    if (prevRow >= 0) {
        View->setCurrentCell(prevRow, 0);
        displayTicket(prevRow);
    } else {
        QMessageBox::information(this, "Информация", "Все билеты ниже уже изучены.");
    }
}

void MainWindow::on_randomclicked() {
    QVector<int> availableTickets;
    for (int i = 0; i < ticketStatus.size(); ++i) {
        if (ticketStatus[i] == 0 || ticketStatus[i] == 1) {
            availableTickets.append(i);
        }
    }
    if (!availableTickets.isEmpty()) {
        int row = availableTickets[QRandomGenerator::global()->bounded(availableTickets.size())];
        View->setCurrentCell(row, 0);
        displayTicket(row);
    } else {
        QMessageBox::information(this, "Информация", "Все билеты уже изучены.");
    }
}

void MainWindow::on_name_edit_returnPressed() {
    if (!name_edit->text().isEmpty()) {
        ticketNames[currentTicket] = name_edit->text();
        name->setText(ticketNames[currentTicket]);
        View->item(currentTicket, 0)->setText(ticketNames[currentTicket]);
    } else {
        QMessageBox::warning(this, "Ошибка", "Имя не может быть пустым.");
    }
}

void MainWindow::on_statuscurrentIndexChanged(int index) {
    if (currentTicket != -1) {
        ticketStatus[currentTicket] = index;
        QTableWidgetItem* item = View->item(currentTicket, 0);
        if (index == 0)
            item->setBackground(Qt::gray);
        else if (index == 1)
            item->setBackground(Qt::yellow);
        else if (index == 2)
            item->setBackground(Qt::green);
        updateProgress();
    }
}

void MainWindow::displayTicket(int row) {
    if (row < 0 || row >= ticketStatus.size()) {
        QMessageBox::critical(this, "Ошибка", "Некорректный номер билета.");
        return;
    }

    if (ticketNames.isEmpty() || ticketContents.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Массивы билетов не инициализированы.");
        return;
    }

    currentTicket = row;

    QString ticketInfo =
    QString("Билет %1<br><b>Содержание:</b><br>%2").arg(row + 1).arg(ticketContents[row]);

    if (!number || !name || !name_edit || !status) {
        QMessageBox::critical(this, "Ошибка", "UI-элементы не инициализированы.");
        return;
    }

    number->setText(ticketInfo);
    status->setCurrentIndex(ticketStatus[row]);
}

void MainWindow::updateProgress() {
    if (ticketStatus.isEmpty()) return;

    int totalCount = ticketStatus.size();
    int greenCount = 0, yellowCount = 0;

    for (int s : ticketStatus) {
        if (s == 1) yellowCount++;
        else if (s == 2) greenCount++;
    }

    if (!total || !green) return;

    total->setValue((yellowCount + greenCount) * 100 / totalCount);
    green->setValue(greenCount * 100 / totalCount);
}

void MainWindow::on_resetclicked() {
    on_CountvalueChanged(Count->value());
    QMessageBox::information(this, "Сброс", "Все билеты сброшены в начальное состояние.");
}

void MainWindow::on_savetriggered() {
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить состояние", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }

    QTextStream out(&file);
    out << Count->value() << "\n";
    for (int i = 0; i < ticketStatus.size(); ++i)
        out << ticketNames[i] << "," << ticketStatus[i] << "\n";

    file.close();
    QMessageBox::information(this, "Сохранено", "Состояние успешно сохранено.");
}

void MainWindow::on_loadtriggered() {
    QString fileName = QFileDialog::getOpenFileName(this, "Загрузить состояние", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;

    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения.");
        return;
    }

    QTextStream in(&file);
    int count = in.readLine().toInt();
    Count->setValue(count);
    ticketStatus.clear();
    ticketNames.clear();
    View->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        QStringList data = in.readLine().split(",");
        ticketNames.append(data[0]);
        ticketStatus.append(data[1].toInt());
        QTableWidgetItem* item = new QTableWidgetItem(ticketNames[i]);
        if (ticketStatus[i] == 0) item->setBackground(Qt::gray);
        else if (ticketStatus[i] == 1) item->setBackground(Qt::yellow);
        else if (ticketStatus[i] == 2) item->setBackground(Qt::green);
        View->setItem(i, 0, item);
    }

    file.close();
    updateProgress();
    QMessageBox::information(this, "Загружено", "Состояние успешно загружено.");
}

    void MainWindow::setupUi(QMainWindow *MainWindow)
    {
        if (MainWindow->objectName().isEmpty())
            MainWindow->setObjectName("MainWindow");
        MainWindow->resize(800, 600);
        centralwidget = new QWidget(MainWindow);
        centralwidget->setObjectName("centralwidget");
        questioview = new QGroupBox(centralwidget);
        questioview->setObjectName("questioview");
        questioview->setGeometry(QRect(10, 30, 461, 381));
        number = new QLabel(questioview);
        number->setObjectName("number");
        number->setGeometry(QRect(30, 80, 411, 231));
        name = new QLabel(questioview);
        name->setObjectName("name");
        name->setGeometry(QRect(20, 40, 67, 17));
        name_edit = new QLineEdit(questioview);
        name_edit->setObjectName("name_edit");
        name_edit->setGeometry(QRect(330, 30, 113, 25));
        status = new QComboBox(questioview);
        status->setObjectName("status");
        status->setGeometry(QRect(340, 330, 101, 31));
        previous = new QPushButton(centralwidget);
        previous->setObjectName("previous");
        previous->setGeometry(QRect(20, 440, 151, 81));
        total = new QProgressBar(centralwidget);
        total->setObjectName("total");
        total->setGeometry(QRect(530, 350, 241, 23));
        total->setValue(24);
        green = new QProgressBar(centralwidget);
        green->setObjectName("green");
        green->setGeometry(QRect(530, 390, 241, 23));
        next = new QPushButton(centralwidget);
        next->setObjectName("next");
        next->setGeometry(QRect(190, 440, 151, 81));
        Count = new QSpinBox(centralwidget);
        Count->setObjectName("Count");
        Count->setGeometry(QRect(660, 440, 81, 81));
        Count->setMouseTracking(false);
        Count->setMaximum(25);
        random = new QPushButton(centralwidget);
        random->setObjectName("random");
        random->setGeometry(QRect(370, 440, 151, 81));
        View = new QTableWidget(centralwidget);
        if (View->columnCount() < 1)
            View->setColumnCount(1);
        QTableWidgetItem *__qtablewidgetitem = new QTableWidgetItem();
        View->setHorizontalHeaderItem(0, __qtablewidgetitem);
        View->setObjectName("View");
        View->setGeometry(QRect(510, 80, 256, 192));
        MainWindow->setCentralWidget(centralwidget);
        menubar = new QMenuBar(MainWindow);
        menubar->setObjectName("menubar");
        menubar->setGeometry(QRect(0, 0, 800, 22));
        MainWindow->setMenuBar(menubar);
        statusbar = new QStatusBar(MainWindow);
        statusbar->setObjectName("statusbar");
        MainWindow->setStatusBar(statusbar);

        retranslateUi(MainWindow);

        QMetaObject::connectSlotsByName(MainWindow);
    } // setupUi

    void MainWindow::retranslateUi(QMainWindow *MainWindow)
    {
        MainWindow->setWindowTitle(QCoreApplication::translate("MainWindow", "MainWindow", nullptr));
        questioview->setTitle(QCoreApplication::translate("MainWindow", "\320\221\320\270\320\273\320\265\321\202\321\213", nullptr));
        number->setText(QString());
        name->setText(QString());
        previous->setText(QCoreApplication::translate("MainWindow", "\320\237\321\200\320\265\320\264\321\213\320\264\321\203\321\211\320\270\320\271", nullptr));
        next->setText(QCoreApplication::translate("MainWindow", "C\320\273\320\265\320\264\321\203\321\216\321\211\320\270\320\271", nullptr));
        random->setText(QCoreApplication::translate("MainWindow", "\320\241\320\273\321\203\321\207\320\260\320\271\320\275\321\213\320\271", nullptr));
        QTableWidgetItem *___qtablewidgetitem = View->horizontalHeaderItem(0);
        ___qtablewidgetitem->setText(QCoreApplication::translate("MainWindow", "\320\221\320\270\320\273\320\265\321\202\321\213", nullptr));
    } // retranslateUi

