#ifndef RESULTS_H
#define RESULTS_H

#include <QDialog>
#include <QLabel>
#include <QTableWidget>

namespace UI {
class Results;
}

class Results : public QDialog {
    Q_OBJECT

public:
    explicit Results(QWidget *parent = nullptr);
    ~Results();

    void setData(const QString &name, int facultyPicked, int langPoints,
                 int mathPoints, int physPoints, int gpaPoints, bool hasBenefits);
    void updateLabel();

private:
    QLabel *label;
    QTableWidget *tableWidget;
    void setupUI();
    QString nameApplicant;
};

#endif // RESULTS_H
