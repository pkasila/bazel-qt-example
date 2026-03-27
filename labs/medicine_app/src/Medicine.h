#ifndef MEDICINE_H
#define MEDICINE_H

#include <QString>
#include <QTime>

class Medicine {
public:
    QString name;
    QString dosage;
    QTime time;
    QString notes;

    bool operator == (const Medicine& anotherMed) const {
        if (name == anotherMed.name &&
            dosage == anotherMed.dosage &&
            time == anotherMed.time &&
            notes == anotherMed.notes) {
            return true;
        } else {
            return false;
        }
    }
};

#endif // MEDICINE_H
