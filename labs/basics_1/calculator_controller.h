//
// Created by blalex on 3/14/25.
//

#ifndef CALCULATOR_CONTROLLER_H
#define CALCULATOR_CONTROLLER_H
#include <QComboBox>
#include <QLineEdit>
#include <qdialog.h>
#include <unordered_set>

class CalculatorController : public QObject {
    Q_OBJECT
   public:
    explicit CalculatorController(QObject* parent = nullptr);

   private:
    void UpdateValues(QLineEdit* edited);
    void UpdateUnit(QLineEdit* edited, QString unit);

    float ToBase(const QString& unit, double number);
    float FromBase(const QString& unit, double number);

    struct Field {
        QLineEdit* value_;
        QComboBox* unit_;
        QString prev_value_;
    };

    std::vector<Field> fields_{};
    bool is_work_ = false;
    friend class MainWindow;

    std::unordered_map<QString, double> to_base_{
      {"m", 1},
      {"cm", 0.01},
      {"km", 1000},
      {"dm", 0.1},
      {"mm", 0.001},

      {"mi", 1609.34},
      {"yd", 0.9144},
      {"ft", 0.3048},
      {"in", 0.0254},

      {"li", 500},
      {"zhang", 3.33333},
      {"chi", 0.333333},
      {"cun", 0.0333333},

      {"au", 149'597'870'700},
      {"ly", 9.4607304725808e15},
      {"pc", 3.0856775814913672789e16},

      {"nmi", 1852}};

    std::unordered_map<QString, double> from_base_{
      {"m", 1},
      {"cm", 100},
      {"km", 0.001},
      {"dm", 10},
      {"mm", 1000},

      {"mi", 0.000621371},
      {"yd", 1.09361},
      {"ft", 3.28084},
      {"in", 39.3701},

      {"li", 0.002},
      {"zhang", 0.3},
      {"chi", 3},
      {"cun", 30},

      {"au", 6.684587122e-12},
      {"ly", 1.057000834e-16},
      {"pc", 3.240779289e-17},

      {"nmi", 0.000539957}};

    std::pmr::unordered_set<QString> units_{
      "Metrical:", "British/American:", "Chinese:", "Astronomical:", "Sea:"};
};

#endif  // CALCULATOR_CONTROLLER_H
