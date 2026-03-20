//
// Created by blalex on 3/14/25.
//

#ifndef MAIN_WINDOW_H
#define MAIN_WINDOW_H
#include "calculator_controller.h"

#include <QMainWindow>
#include <QPushButton>

QT_BEGIN_NAMESPACE
class QVBoxLayout;
class QMainWindow;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT
   public:
    MainWindow(QWidget* parent = nullptr);

   private:
    QVBoxLayout* main_layout_;
    CalculatorController* controller_;
    QPushButton* button_;
    QVBoxLayout* main_layout1_;
    QMenuBar* menu_bar_;

    std::vector<QString> units_ = {
      // Metrical
      "m", "cm", "km", "dm", "mm",

      // British/American
      "mi", "yd", "ft", "in",

      // Chinese
      "li", "zhang", "chi", "cun",

      // Astronomicalc
      "au", "ly", "pc",

      // Sea
      "nmi"};

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
    QString filepath_ = "labs/basics_1/settings.txt";

    void AddField();
    void SaveSettings();
    void LoadSettings();
    void СlearAllFields();
};

#endif  // MAIN_WINDOW_H
