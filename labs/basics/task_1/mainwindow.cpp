#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QTableWidgetItem>
#include <QRandomGenerator>
#include <QFileDialog>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    setWindowTitle("Билеты");

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QGridLayout *mainLayout = new QGridLayout(centralWidget);

    QVBoxLayout *contentLayout = new QVBoxLayout();

    ui->number->setWordWrap(true);
    contentLayout->addWidget(ui->number);

    QHBoxLayout *buttonStatusLayout = new QHBoxLayout();
    buttonStatusLayout->addWidget(ui->previous);
    buttonStatusLayout->addWidget(ui->next);
    buttonStatusLayout->addWidget(ui->random);
    buttonStatusLayout->addWidget(ui->status);
    contentLayout->addLayout(buttonStatusLayout);

    QHBoxLayout *progressLayout = new QHBoxLayout();
    progressLayout->addWidget(ui->total);
    progressLayout->addWidget(ui->green);
    contentLayout->addLayout(progressLayout);

    mainLayout->addLayout(contentLayout, 0, 0);

    ui->View->setMaximumWidth(300);
    mainLayout->addWidget(ui->View, 0, 1);

    ui->Count->setToolTip("Количество билетов");
    mainLayout->addWidget(ui->Count, 1, 1, Qt::AlignRight | Qt::AlignBottom);

    connect(ui->Count, QOverload<int>::of(&QSpinBox::valueChanged), this, &MainWindow::on_CountvalueChanged);
    connect(ui->View, &QTableWidget::cellClicked, this, &MainWindow::on_ViewcellClicked);
    connect(ui->View, &QTableWidget::cellDoubleClicked, this, &MainWindow::on_ViewcellDoubleClicked);
    connect(ui->next, &QPushButton::clicked, this, &MainWindow::on_nextclicked);
    connect(ui->random, &QPushButton::clicked, this, &MainWindow::on_randomclicked);
    connect(ui->previous, &QPushButton::clicked, this, &MainWindow::on_previousclicked);
    connect(ui->name_edit, &QLineEdit::returnPressed, this, &MainWindow::on_name_edit_returnPressed);
    connect(ui->status, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::on_statuscurrentIndexChanged);

    QPushButton *resetButton = new QPushButton("Сброс", this);
    resetButton->setToolTip("Сбросить все билеты в начальное состояние");
    ui->statusbar->addPermanentWidget(resetButton);
    connect(resetButton, &QPushButton::clicked, this, &MainWindow::on_resetclicked);

    QAction *saveAction = new QAction("Сохранить", this);
    QAction *loadAction = new QAction("Загрузить", this);
    connect(saveAction, &QAction::triggered, this, &MainWindow::on_savetriggered);
    connect(loadAction, &QAction::triggered, this, &MainWindow::on_loadtriggered);

    ui->menubar->addAction(saveAction);
    ui->menubar->addAction(loadAction);

    ui->number->setWordWrap(true);
    ui->number->setMaximumWidth(400);
    ui->status->addItems({"Default", "Yellow", "Green"});
}

void MainWindow::on_CountvalueChanged(int value)
{
    if (value <= 0) {
        QMessageBox::warning(this, "Ошибка", "Количество билетов должно быть больше 0.");
        return;
    }

    ui->View->setRowCount(value);
    ticketStatus.resize(value);
    ticketNames.resize(value);
    ticketContents.resize(value);

    for (int i = 0; i < value; ++i) {
        QTableWidgetItem *item = new QTableWidgetItem("Билет " + QString::number(i + 1));
        item->setBackground(Qt::gray);
        ui->View->setItem(i, 0, item);
        ticketStatus[i] = 0;
        ticketNames[i] = "Билет " + QString::number(i + 1);
        ticketContents = {"1. Критерий различия действительных чисел.",
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
                          "25. Теорема Вейерштрасса."                                       };
    }

    updateProgress();
}

void MainWindow::on_ViewcellClicked(int row, int column)
{
    Q_UNUSED(column);
    displayTicket(row);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_ViewcellDoubleClicked(int row, int column)
{
    Q_UNUSED(column);
    QTableWidgetItem *item = ui->View->item(row, 0);
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

void MainWindow::on_nextclicked()
{
    int nextRow = currentTicket +1;
    while (nextRow < ticketStatus.size()) {
        if (ticketStatus[nextRow] == 0 || ticketStatus[nextRow] == 1) {
            break;
        }
        ++nextRow;
    }
    if (nextRow < ticketStatus.size()) {
        ui->View->setCurrentCell(nextRow, 0);
        displayTicket(nextRow);
    } else {
        QMessageBox::information(this, "Информация", "Все билеты выше уже изучены.");
    }
}

void MainWindow::on_previousclicked()
{
    int prevRow = currentTicket;
    --prevRow;
    while (prevRow >= 0) {
        if (ticketStatus[prevRow] == 0 || ticketStatus[prevRow] == 1) {
            break;
        }
        --prevRow;
    }
    if (prevRow >= 0) {
        ui->View->setCurrentCell(prevRow, 0);
        displayTicket(prevRow);
    } else {
        QMessageBox::information(this, "Информация", "Все билеты ниже уже изучены.");
    }
}

void MainWindow::on_randomclicked()
{
    QVector<int> availableTickets;
    for (int i = 0; i < ticketStatus.size(); ++i) {
        if (ticketStatus[i] == 0 || ticketStatus[i] == 1) {
            availableTickets.append(i);
        }
    }
    if (!availableTickets.isEmpty()) {
        int row = availableTickets[QRandomGenerator::global()->bounded(availableTickets.size())];
        ui->View->setCurrentCell(row, 0);
        displayTicket(row);
    } else {
        QMessageBox::information(this, "Информация", "Все билеты уже изучены.");
    }
}

void MainWindow::on_name_edit_returnPressed()
{
    if (!ui->name_edit->text().isEmpty()) {
        ticketNames[currentTicket] = ui->name_edit->text();
        ui->name->setText(ticketNames[currentTicket]);
        ui->View->item(currentTicket, 0)->setText(ticketNames[currentTicket]);
    } else {
        QMessageBox::warning(this, "Ошибка", "Имя не может быть пустым.");
    }
}

void MainWindow::on_statuscurrentIndexChanged(int index)
{
    if (currentTicket != -1) {
        ticketStatus[currentTicket] = index;
        QTableWidgetItem *item = ui->View->item(currentTicket, 0);
        if (index == 0) item->setBackground(Qt::gray);
        else if (index == 1) item->setBackground(Qt::yellow);
        else if (index == 2) item->setBackground(Qt::green);
        updateProgress();
    }
}

void MainWindow::displayTicket(int row)
{
    if (row < 0 || row >= ticketStatus.size()) {
        QMessageBox::critical(this, "Ошибка", "Некорректный номер билета.");
        return;
    }

    if (ticketNames.isEmpty() || ticketContents.isEmpty()) {
        QMessageBox::critical(this, "Ошибка", "Массивы билетов не инициализированы.");
        return;
    }

    currentTicket = row;

    QString ticketInfo = QString("Билет %1<br><b>Содержание:</b><br>%2")
                             .arg(row + 1)
                             .arg(ticketContents[row]);

    if (!ui->number || !ui->name || !ui->name_edit || !ui->status) {
        QMessageBox::critical(this, "Ошибка", "UI-элементы не инициализированы.");
        return;
    }

    ui->number->setText(ticketInfo);
    ui->status->setCurrentIndex(ticketStatus[row]);

    qDebug() << "Displayed ticket:" << row << "CurrentTicket:" << currentTicket;
}

void MainWindow::updateProgress()
{
    if (ticketStatus.isEmpty()) {
        qDebug() << "Error: ticketStatus is empty.";
        return;
    }

    int total = ticketStatus.size();
    if (total == 0) {
        qDebug() << "Error: Total tickets count is zero.";
        return;
    }

    int green = 0;
    int yellow = 0;

    for (int status : ticketStatus) {
        if (status == 1) yellow++;
        else if (status == 2) green++;
    }

    if (!ui->total || !ui->green) {
        qDebug() << "Error: Progress bars are not initialized.";
        return;
    }

    ui->total->setValue((yellow + green) * 100 / total);
    ui->green->setValue(green * 100 / total);

    qDebug() << "Progress updated: Total =" << (yellow + green) * 100 / total
             << ", Green =" << green * 100 / total;
}

void MainWindow::on_resetclicked()
{
    on_CountvalueChanged(ui->Count->value());
    QMessageBox::information(this, "Сброс", "Все билеты сброшены в начальное состояние.");
}

void MainWindow::on_savetriggered()
{
    QString fileName = QFileDialog::getSaveFileName(this, "Сохранить состояние", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для записи.");
        return;
    }
    QTextStream out(&file);
    out << ui->Count->value() << "\n";
    for (int i = 0; i < ticketStatus.size(); ++i) {
        out << ticketNames[i] << "," << ticketStatus[i] << "\n";
    }
    file.close();
    QMessageBox::information(this, "Сохранено", "Состояние успешно сохранено.");
}

void MainWindow::on_loadtriggered()
{
    QString fileName = QFileDialog::getOpenFileName(this, "Загрузить состояние", "", "Text Files (*.txt)");
    if (fileName.isEmpty()) return;
    QFile file(fileName);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(this, "Ошибка", "Не удалось открыть файл для чтения.");
        return;
    }
    QTextStream in(&file);
    int count = in.readLine().toInt();
    ui->Count->setValue(count);
    ticketStatus.clear();
    ticketNames.clear();
    ui->View->setRowCount(count);
    for (int i = 0; i < count; ++i) {
        QStringList data = in.readLine().split(",");
        ticketNames.append(data[0]);
        ticketStatus.append(data[1].toInt());
        QTableWidgetItem *item = new QTableWidgetItem(ticketNames[i]);
        if (ticketStatus[i] == 0) item->setBackground(Qt::gray);
        else if (ticketStatus[i] == 1) item->setBackground(Qt::yellow);
        else if (ticketStatus[i] == 2) item->setBackground(Qt::green);
        ui->View->setItem(i, 0, item);
    }
    file.close();
    updateProgress();
    QMessageBox::information(this, "Загружено", "Состояние успешно загружено.");
}
