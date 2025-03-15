#include "Results.h"
#include <QVBoxLayout>
#include <QTableWidgetItem>
#include <QColor>

Results::Results(QWidget *parent) : QDialog(parent) {
    setupUI();
}

Results::~Results() {}

void Results::setupUI() {
    setWindowTitle("Your results!");
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    label = new QLabel(QString(nameApplicant + ", your results are :"), this);
    layout->addWidget(label);

    tableWidget = new QTableWidget(this);
    tableWidget->setColumnCount(2);
    tableWidget->setHorizontalHeaderLabels({"Major", "Status"});
    layout->addWidget(tableWidget);

    centralWidget->setLayout(layout);
    setLayout(layout);
}

void Results::setData(const QString &name, int facultyPicked, int langPoints,
                      int mathPoints, int physPoints, int gpaPoints, bool hasBenefits) {
    Results::nameApplicant = name;
    std::vector<std::vector<std::pair<QString, int>>> faculties(4);

    faculties[0] = { {"Applied CompScie", 394}, {"CompScie", 384}, {"Applied Math", 370}, {"Cybersecurity", 373} };
    faculties[1] = { {"Computer Physics", 362}, {"Nuclear Physics", 351}, {"Fundamental Physics", 349} };
    faculties[2] = { {"Mechanics", 340}, {"Mathematics", 333}, {"CMSA", 354} };
    faculties[3] = { {"Applied CompScie", 364}, {"Radioelectronics", 360}, {"Cosmology", 356} };

    int totalPoints = langPoints + mathPoints + physPoints + gpaPoints;

    tableWidget->setRowCount(faculties[facultyPicked].size());

    for (int i = 0; i < faculties[facultyPicked].size(); ++i) {
        const auto &major = faculties[facultyPicked][i];

        tableWidget->setItem(i, 0, new QTableWidgetItem(major.first));

        QTableWidgetItem *statusItem = new QTableWidgetItem();
        if (hasBenefits || totalPoints >= major.second) {
            statusItem->setText("Enough Points");
            statusItem->setBackground(Qt::green);
        } else {
            statusItem->setText("Not Enough Points");
            statusItem->setBackground(Qt::red);
        }
        tableWidget->setItem(i, 1, statusItem);
    }


    tableWidget->setEditTriggers(QAbstractItemView::NoEditTriggers);
    updateLabel();
    tableWidget->resizeColumnsToContents();

}

void Results::updateLabel() {
    label->setText(QString(nameApplicant + ", your results are :"));
}
