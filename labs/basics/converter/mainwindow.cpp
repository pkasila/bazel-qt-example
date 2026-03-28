#include "mainwindow.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QApplication>

const QMap<QString, double> conversionFactors = {
    {"Миллиметры", 0.001},
    {"Сантиметры", 0.01},
    {"Метры", 1.0},
    {"Километры", 1000.0},
    {"Дюймы", 0.0254},
    {"Футы", 0.3048},
    {"Мили", 1609.34}
};


void MainWindow::convertValue() {
    bool ok;
    double inputValue = textline->text().toDouble(&ok);

    if (!ok) {
        resultLabel->setText("Ошибка: Введите число!");
        return;
    }

    QString fromUnit = velichbox1->currentText();
    QString toUnit = velichbox2->currentText();

    if (!conversionFactors.contains(fromUnit) || !conversionFactors.contains(toUnit)) {
        resultLabel->setText("Ошибка: Некорректные единицы!");
        return;
    }

    double valueInMeters = inputValue * conversionFactors[fromUnit];
    double convertedValue = valueInMeters / conversionFactors[toUnit];
    resultLabel->setText(QString::number(convertedValue, 'f', 4));
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    resize(600, 400);
    QWidget *centralWidget = new QWidget(this);

    QVBoxLayout *mainVertical = new QVBoxLayout(centralWidget);
    mainVertical->setContentsMargins(30, 30, 30, 30);
    mainVertical->setSpacing(25);

    QLabel *mainLabel = new QLabel("🔄 Конвертер величин");
    themeToggle = new QCheckBox("🌙 Тёмная тема");
    themeToggle->setStyleSheet("color: white; font-size: 13px;");
    themeToggle->setChecked(true); // по умолчанию тёмная
    mainVertical->addWidget(themeToggle, 0, Qt::AlignRight);

    connect(themeToggle, &QCheckBox::toggled, this, &MainWindow::toggleTheme);

    mainLabel->setAlignment(Qt::AlignCenter);
    mainLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #2c3e50;");

    QHBoxLayout *unitLayout = new QHBoxLayout;
    QVBoxLayout *fromLayout = new QVBoxLayout;
    QVBoxLayout *toLayout = new QVBoxLayout;

    velichbox1 = new QComboBox;
    velichbox2 = new QComboBox;
    QStringList units = {"Миллиметры", "Сантиметры", "Метры", "Километры", "Дюймы", "Футы", "Мили"};
    velichbox1->addItems(units);
    velichbox2->addItems(units);

    QLabel *labelFrom = new QLabel("Из чего:");
    QLabel *labelTo = new QLabel("Во что:");

    fromLayout->addWidget(labelFrom);
    fromLayout->addWidget(velichbox1);
    toLayout->addWidget(labelTo);
    toLayout->addWidget(velichbox2);

    unitLayout->addLayout(fromLayout);
    unitLayout->addSpacing(40);
    unitLayout->addLayout(toLayout);

    QVBoxLayout *inputLayout = new QVBoxLayout;
    QLabel *inputLabel = new QLabel("Введите значение:");
    textline = new QLineEdit();
    textline->setPlaceholderText("например: 12.5");
    textline->setFixedHeight(35);
    textline->setStyleSheet("padding: 5px; font-size: 14px;");
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(textline);

    slide1 = new QSlider(Qt::Horizontal);
    slide1->setRange(0, 100);
    slide1->setTickInterval(1);
    slide1->setSingleStep(1);
    slide1->setValue(0);

    connect(slide1, &QSlider::valueChanged, this, [=](int value) {
        textline->setText(QString::number(value));
    });

    connect(textline, &QLineEdit::textChanged, this, [=](const QString &text) {
        bool ok;
        int val = text.toInt(&ok);
        if (ok && val >= slide1->minimum() && val <= slide1->maximum()) {
            slide1->setValue(val);
        }
    });

    QPushButton *convertBtn = new QPushButton("Преобразовать");
    convertBtn->setStyleSheet("background-color: #2980b9; color: white; padding: 10px 15px; font-size: 15px; border-radius: 5px;");
    connect(convertBtn, &QPushButton::clicked, this, &MainWindow::convertValue);

    QVBoxLayout *resultLayout = new QVBoxLayout;
    QLabel *resultText = new QLabel("Результат:");
    resultLabel = new QLabel("0.0");
    resultLabel->setStyleSheet("font-size: 20px; font-weight: bold; color: #16a085;");
    resultLayout->addWidget(resultText);
    resultLayout->addWidget(resultLabel);

    mainVertical->addWidget(mainLabel);
    mainVertical->addLayout(unitLayout);
    mainVertical->addLayout(inputLayout);
    mainVertical->addWidget(slide1);
    mainVertical->addWidget(convertBtn, 0, Qt::AlignCenter);
    mainVertical->addLayout(resultLayout);

    centralWidget->setLayout(mainVertical);
    setCentralWidget(centralWidget);

    // Установка темной темы
    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::WindowText, Qt::white);
    darkPalette.setColor(QPalette::Base, QColor(42, 42, 42));
    darkPalette.setColor(QPalette::AlternateBase, QColor(66, 66, 66));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
    darkPalette.setColor(QPalette::ToolTipText, Qt::white);
    darkPalette.setColor(QPalette::Text, Qt::white);
    darkPalette.setColor(QPalette::Button, QColor(53, 53, 53));
    darkPalette.setColor(QPalette::ButtonText, Qt::white);
    darkPalette.setColor(QPalette::BrightText, Qt::red);
    darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::Highlight, QColor(42, 130, 218));
    darkPalette.setColor(QPalette::HighlightedText, Qt::black);

    qApp->setPalette(darkPalette);
    qApp->setStyle("Fusion"); // стиль, подходящий под тёмную тему


    setStyleSheet(R"(
    QLabel {
        font-size: 14px;
        color: white;
    }
    QComboBox {
        padding: 5px;
        font-size: 14px;
        background-color: #2c2c2c;
        color: white;
        border: 1px solid #555;
        border-radius: 4px;
    }
    QLineEdit {
        border: 1px solid #555;
        border-radius: 4px;
        background-color: #2c2c2c;
        color: white;
        padding: 5px;
        font-size: 14px;
    }
    QPushButton {
        background-color: #2980b9;
        color: white;
        padding: 10px 15px;
        font-size: 15px;
        border-radius: 5px;
    }
    QPushButton:hover {
        background-color: #3498db;
    }
    QSlider::groove:horizontal {
        border: 1px solid #444;
        height: 6px;
        background: #2c2c2c;
        margin: 0px;
        border-radius: 3px;
    }
    QSlider::handle:horizontal {
        background: #2980b9;
        border: 1px solid #5c5c5c;
        width: 15px;
        height: 15px;
        margin: -5px 0;
        border-radius: 7px;
    }
)");
}
MainWindow::~MainWindow() {}


void MainWindow::toggleTheme(bool enabled) {
    if (enabled) {
        // Улучшенная темная тема
        QPalette darkPalette;
        darkPalette.setColor(QPalette::Window, QColor("#1e1e1e"));
        darkPalette.setColor(QPalette::WindowText, Qt::white);
        darkPalette.setColor(QPalette::Base, QColor("#2d2d2d"));
        darkPalette.setColor(QPalette::AlternateBase, QColor("#3c3c3c"));
        darkPalette.setColor(QPalette::ToolTipBase, Qt::white);
        darkPalette.setColor(QPalette::ToolTipText, Qt::white);
        darkPalette.setColor(QPalette::Text, Qt::white);
        darkPalette.setColor(QPalette::Button, QColor("#2d2d2d"));
        darkPalette.setColor(QPalette::ButtonText, Qt::white);
        darkPalette.setColor(QPalette::BrightText, Qt::red);
        darkPalette.setColor(QPalette::Link, QColor("#569CD6"));
        darkPalette.setColor(QPalette::Highlight, QColor("#569CD6"));
        darkPalette.setColor(QPalette::HighlightedText, Qt::black);

        qApp->setPalette(darkPalette);
    } else {
        // Светлая тема
        qApp->setPalette(style()->standardPalette());
    }

    // Обновим стили вручную, чтобы включалась анимация/оттенки
    setStyleSheet(enabled ? R"(
        QLabel {
            font-size: 14px;
            color: #f0f0f0;
        }
        QComboBox {
            padding: 6px;
            font-size: 14px;
            background-color: #2d2d2d;
            color: white;
            border: 1px solid #444;
            border-radius: 4px;
        }
        QLineEdit {
            background-color: #2d2d2d;
            color: white;
            border: 1px solid #444;
            border-radius: 4px;
            padding: 6px;
            font-size: 14px;
        }
        QPushButton {
            background-color: #569CD6;
            color: white;
            padding: 10px 15px;
            font-size: 15px;
            border: none;
            border-radius: 5px;
        }
        QPushButton:hover {
            background-color: #6aaef0;
        }
        QSlider::groove:horizontal {
            border: none;
            height: 6px;
            background: #3c3c3c;
            border-radius: 3px;
        }
        QSlider::handle:horizontal {
            background: #569CD6;
            border: none;
            width: 14px;
            height: 14px;
            margin: -5px 0;
            border-radius: 7px;
        }
        QCheckBox {
            color: #cccccc;
            font-size: 13px;
        }
    )" : R"( /* Светлая тема — можно задать отдельно при желании */ )");
}
