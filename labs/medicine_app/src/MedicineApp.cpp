#include "MedicineApp.h"


MedicineApp::MedicineApp(QWidget *parent)
    : QMainWindow(parent) {
    setupUI();
    setupConnections();
}

void MedicineApp::setupUI() {
    setWindowTitle("План приёма лекарств");
    resize(700, 300);
    setMinimumSize(600, 250);

    format.setFontItalic(true);
    format.setFontWeight(900);
    format.setForeground(Qt::magenta);

    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    calendar = new QCalendarWidget(this);
    calendar->setMinimumDate(QDate::currentDate());
    calendar->setMaximumDate(QDate::currentDate().addYears(1));
    calendar->setVerticalHeaderFormat(QCalendarWidget::NoVerticalHeader);

    mainLayout->addWidget(calendar);

    QVBoxLayout *medicineTableLayout = new QVBoxLayout();

    medicineTable = new QTableWidget(this);
    medicineTable->setColumnCount(4);
    medicineTable->setColumnWidth(3, 100);
    medicineTable->setHorizontalHeaderLabels({"Название", "Дозировка", "Время", "Заметки"});
    medicineTable->setContextMenuPolicy(Qt::CustomContextMenu);
    medicineTable->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    medicineTable->setSelectionMode(QAbstractItemView::NoSelection);
    medicineTable->horizontalHeader()->setSectionsClickable(false);
    medicineTable->horizontalHeader()->setSectionResizeMode(QHeaderView::ResizeToContents);
    medicineTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    medicineTable->setEditTriggers(QAbstractItemView::NoEditTriggers);
    medicineTable->setStyleSheet("QTableView::item:hover { background: none; }");

    medicineTableLayout->addWidget(medicineTable);

    addButton = new QPushButton("Добавить лекарство...", this);
    medicineTableLayout->addWidget(addButton);

    mainLayout->addLayout(medicineTableLayout);

    setCentralWidget(centralWidget);
}

void MedicineApp::setupConnections() {
    connect(calendar, &QCalendarWidget::selectionChanged, this, &MedicineApp::updateTable);
    connect(addButton, &QPushButton::clicked, this, &MedicineApp::addMedicine);
    connect(medicineTable, &QTableWidget::customContextMenuRequested, this, &MedicineApp::showContextMenu);
}

void MedicineApp::showContextMenu(const QPoint& position) {
    QModelIndex index = medicineTable->indexAt(position);
    if (!index.isValid()) {
        return;
    }
    QMenu *contextMenu = new QMenu(this);
    QAction *editAction = contextMenu->addAction("Редактировать");
    QAction *deleteAction = contextMenu->addAction("Удалить");

    QAction *selectedAction = contextMenu->exec(medicineTable->viewport()->mapToGlobal(position));

    if (selectedAction == editAction) {
        //add function
    } else if (selectedAction == deleteAction) {
        deleteMedicineRow(index.row());
    }
}

void MedicineApp::deleteMedicineRow(int row) {
    medicineTable->removeRow(row);
    QDate selectedDate = calendar->selectedDate();
    medicineData[selectedDate].removeAt(row);
    if (medicineData[selectedDate].size() == 0) {
        QTextCharFormat format; //Default format
        calendar->setDateTextFormat(selectedDate, format);
    }
}

void MedicineApp::addMedicineToMedicineTable(const Medicine& medicine, int newRow) {
    medicineTable->insertRow(newRow);
    medicineTable->setItem(newRow, 0, new QTableWidgetItem(medicine.name));
    medicineTable->setItem(newRow, 1, new QTableWidgetItem(medicine.dosage));
    medicineTable->setItem(newRow, 2, new QTableWidgetItem(medicine.time.toString("hh:mm")));
    medicineTable->setItem(newRow, 3, new QTableWidgetItem(medicine.notes));
}

void MedicineApp::updateTable() {
    medicineTable->setRowCount(0);
    QDate selectedDate = calendar->selectedDate();
    if (medicineData.contains(selectedDate)) {
        QVector<Medicine> meds = medicineData[selectedDate];
        for (const auto &med : meds) {
            addMedicineToMedicineTable(med, medicineTable->rowCount());
        }
    }
}

void MedicineApp::addMedicine() {
    AddMedicineDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        Medicine medicine = dialog.getMedicine();
        QDate selectedDate = calendar->selectedDate();
        calendar->setDateTextFormat(selectedDate, format);

        medicineData[selectedDate].append(medicine);
        std::sort(medicineData[selectedDate].begin(), medicineData[selectedDate].end(),
                  [](const Medicine& med_1, const Medicine& med_2) {
            return med_1.time < med_2.time;
        });
        int newRow = medicineData[selectedDate].indexOf(medicine);
        addMedicineToMedicineTable(medicine, newRow);
    }
}
