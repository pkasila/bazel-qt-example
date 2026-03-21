#include "mainwindow.h"
#include <QMap>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("Unit Converter");
    setMinimumSize(700, 500);
    setupUI();
    applyStyle("Light");
    updateUnits();
}

MainWindow::~MainWindow()
{
}

void MainWindow::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    mainLayout->setContentsMargins(30, 30, 30, 30);
    mainLayout->setSpacing(25);

    QLabel *titleLabel = new QLabel("Unit Converter");
    titleLabel->setStyleSheet("font-size: 28px; font-weight: bold;");
    titleLabel->setAlignment(Qt::AlignCenter);
    mainLayout->addWidget(titleLabel);

    panel = new QFrame();
    panel->setFrameStyle(QFrame::StyledPanel | QFrame::Raised);
    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setSpacing(20);
    panelLayout->setContentsMargins(25, 25, 25, 25);

    QHBoxLayout *themeRow = new QHBoxLayout();
    themeRow->addWidget(new QLabel("Тема:"));
    comboTheme = new QComboBox();
    comboTheme->addItems({"Light", "Dark", "Ocean", "Forest", "Sunset"});
    comboTheme->setCursor(Qt::PointingHandCursor);
    connect(comboTheme, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onThemeChanged);
    themeRow->addWidget(comboTheme);
    themeRow->addStretch();
    panelLayout->addLayout(themeRow);

    QHBoxLayout *catRow = new QHBoxLayout();
    catRow->addWidget(new QLabel("Категория:"));
    comboCategory = new QComboBox();
    comboCategory->addItems({"Длина", "Масса", "Температура"});
    comboCategory->setCursor(Qt::PointingHandCursor);
    connect(comboCategory, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onCategoryChanged);
    catRow->addWidget(comboCategory);
    catRow->addStretch();
    panelLayout->addLayout(catRow);

    QHBoxLayout *fromRow = new QHBoxLayout();
    fromRow->addWidget(new QLabel("Из:"));
    comboFrom = new QComboBox();
    comboFrom->setCursor(Qt::PointingHandCursor);
    connect(comboFrom, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLiveConvert);
    fromRow->addWidget(comboFrom);
    fromRow->addStretch();
    panelLayout->addLayout(fromRow);

    QHBoxLayout *toRow = new QHBoxLayout();
    toRow->addWidget(new QLabel("В:"));
    comboTo = new QComboBox();
    comboTo->setCursor(Qt::PointingHandCursor);
    connect(comboTo, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::onLiveConvert);
    toRow->addWidget(comboTo);
    toRow->addStretch();
    panelLayout->addLayout(toRow);

    QHBoxLayout *inputRow = new QHBoxLayout();
    inputRow->addWidget(new QLabel("Значение:"));
    spinInput = new QDoubleSpinBox();
    spinInput->setRange(-1000000, 1000000);
    spinInput->setDecimals(6);
    spinInput->setValue(1.0);
    spinInput->setCursor(Qt::PointingHandCursor);
    connect(spinInput, QOverload<double>::of(&QDoubleSpinBox::valueChanged),
            this, &MainWindow::onLiveConvert);
    inputRow->addWidget(spinInput);
    inputRow->addStretch();
    panelLayout->addLayout(inputRow);

    btnConvert = new QPushButton("Конвертировать");
    btnConvert->setFixedHeight(45);
    btnConvert->setCursor(Qt::PointingHandCursor);
    connect(btnConvert, &QPushButton::clicked, this, &MainWindow::onConvert);
    panelLayout->addWidget(btnConvert);

    QPushButton *btnClear = new QPushButton("Очистить");
    btnClear->setFixedHeight(40);
    btnClear->setCursor(Qt::PointingHandCursor);
    connect(btnClear, &QPushButton::clicked, this, &MainWindow::onClear);
    panelLayout->addWidget(btnClear);

    mainLayout->addWidget(panel);

    labelResult = new QLabel("Результат: —");
    labelResult->setStyleSheet("font-size: 22px; font-weight: bold;");
    labelResult->setAlignment(Qt::AlignCenter);
    labelResult->setFrameStyle(QFrame::Panel | QFrame::Sunken);
    mainLayout->addWidget(labelResult);

    labelStatus = new QLabel("Нажмите Enter для конвертации, Esc для очистки");
    labelStatus->setAlignment(Qt::AlignCenter);
    labelStatus->setStyleSheet("font-size: 12px;");
    mainLayout->addWidget(labelStatus);

    mainLayout->addStretch();
}

void MainWindow::applyStyle(const QString &theme)
{
    QString style;
    if (theme == "Light") {
        style = R"(
            QMainWindow { background-color: #f8f9fa; }
            QFrame { background-color: #ffffff; border: 1px solid #dee2e6; border-radius: 12px; }
            QLabel { color: #2c3e50; }
            QPushButton { 
                background-color: #3498db; color: #ffffff; border: none; 
                border-radius: 8px; font-weight: 600; font-size: 14px;
            }
            QPushButton:hover { background-color: #2980b9; }
            QPushButton:pressed { background-color: #2471a3; }
            QComboBox { 
                padding: 8px; border: 1px solid #dee2e6; 
                border-radius: 8px; background: #ffffff; color: #2c3e50; font-size: 14px;
            }
            QDoubleSpinBox { 
                padding: 8px; border: 1px solid #dee2e6; 
                border-radius: 8px; background: #ffffff; color: #2c3e50; font-size: 14px;
            }
            QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                background: #e9ecef; border-radius: 6px;
            }
        )";
    } else if (theme == "Dark") {
        style = R"(
            QMainWindow { background-color: #1a1a2e; }
            QFrame { background-color: #16213e; border: 1px solid #0f3460; border-radius: 12px; }
            QLabel { color: #eaeaea; }
            QPushButton { 
                background-color: #0f3460; color: #ffffff; border: none; 
                border-radius: 8px; font-weight: 600; font-size: 14px;
            }
            QPushButton:hover { background-color: #1a4a7a; }
            QPushButton:pressed { background-color: #0a2a50; }
            QComboBox { 
                padding: 8px; border: 1px solid #0f3460; 
                border-radius: 8px; background: #1a1a2e; color: #eaeaea; font-size: 14px;
            }
            QDoubleSpinBox { 
                padding: 8px; border: 1px solid #0f3460; 
                border-radius: 8px; background: #1a1a2e; color: #eaeaea; font-size: 14px;
            }
            QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                background: #0f3460; border-radius: 6px;
            }
        )";
    } else if (theme == "Ocean") {
        style = R"(
            QMainWindow { background-color: #e0f7fa; }
            QFrame { background-color: #b2ebf2; border: 1px solid #80deea; border-radius: 12px; }
            QLabel { color: #006064; }
            QPushButton { 
                background-color: #00bcd4; color: #ffffff; border: none; 
                border-radius: 8px; font-weight: 600; font-size: 14px;
            }
            QPushButton:hover { background-color: #00acc1; }
            QPushButton:pressed { background-color: #0097a7; }
            QComboBox { 
                padding: 8px; border: 1px solid #80deea; 
                border-radius: 8px; background: #e0f7fa; color: #006064; font-size: 14px;
            }
            QDoubleSpinBox { 
                padding: 8px; border: 1px solid #80deea; 
                border-radius: 8px; background: #e0f7fa; color: #006064; font-size: 14px;
            }
            QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                background: #80deea; border-radius: 6px;
            }
        )";
    } else if (theme == "Forest") {
        style = R"(
            QMainWindow { background-color: #e8f5e9; }
            QFrame { background-color: #c8e6c9; border: 1px solid #a5d6a7; border-radius: 12px; }
            QLabel { color: #1b5e20; }
            QPushButton { 
                background-color: #4caf50; color: #ffffff; border: none; 
                border-radius: 8px; font-weight: 600; font-size: 14px;
            }
            QPushButton:hover { background-color: #43a047; }
            QPushButton:pressed { background-color: #388e3c; }
            QComboBox { 
                padding: 8px; border: 1px solid #a5d6a7; 
                border-radius: 8px; background: #e8f5e9; color: #1b5e20; font-size: 14px;
            }
            QDoubleSpinBox { 
                padding: 8px; border: 1px solid #a5d6a7; 
                border-radius: 8px; background: #e8f5e9; color: #1b5e20; font-size: 14px;
            }
            QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                background: #a5d6a7; border-radius: 6px;
            }
        )";
    } else if (theme == "Sunset") {
        style = R"(
            QMainWindow { background-color: #fff3e0; }
            QFrame { background-color: #ffe0b2; border: 1px solid #ffcc80; border-radius: 12px; }
            QLabel { color: #e65100; }
            QPushButton { 
                background-color: #ff9800; color: #ffffff; border: none; 
                border-radius: 8px; font-weight: 600; font-size: 14px;
            }
            QPushButton:hover { background-color: #f57c00; }
            QPushButton:pressed { background-color: #ef6c00; }
            QComboBox { 
                padding: 8px; border: 1px solid #ffcc80; 
                border-radius: 8px; background: #fff3e0; color: #e65100; font-size: 14px;
            }
            QDoubleSpinBox { 
                padding: 8px; border: 1px solid #ffcc80; 
                border-radius: 8px; background: #fff3e0; color: #e65100; font-size: 14px;
            }
            QDoubleSpinBox::up-button, QDoubleSpinBox::down-button {
                background: #ffcc80; border-radius: 6px;
            }
        )";
    }
    setStyleSheet(style);
}

void MainWindow::updateUnits()
{
    comboFrom->clear();
    comboTo->clear();
    
    QString cat = comboCategory->currentText();
    
    if (cat == "Длина") {
        comboFrom->addItems({"метр", "километр", "сантиметр", "миллиметр", "миля", "ярд", "фут", "дюйм"});
        comboTo->addItems({"метр", "километр", "сантиметр", "миллиметр", "миля", "ярд", "фут", "дюйм"});
    } else if (cat == "Масса") {
        comboFrom->addItems({"килограмм", "грамм", "тонна", "фунт", "унция"});
        comboTo->addItems({"килограмм", "грамм", "тонна", "фунт", "унция"});
    } else if (cat == "Температура") {
        comboFrom->addItems({"Цельсий", "Фаренгейт", "Кельвин"});
        comboTo->addItems({"Цельсий", "Фаренгейт", "Кельвин"});
    }
    
    if (comboFrom->count() > 0) comboFrom->setCurrentIndex(0);
    if (comboTo->count() > 1) comboTo->setCurrentIndex(1);
}

double MainWindow::convertValue(double input, const QString &from, const QString &to, const QString &category)
{
    if (from == to) return input;
    
    if (category == "Длина") {
        QMap<QString, double> toMeters = {
            {"метр", 1.0}, {"километр", 1000.0}, {"сантиметр", 0.01}, {"миллиметр", 0.001},
            {"миля", 1609.344}, {"ярд", 0.9144}, {"фут", 0.3048}, {"дюйм", 0.0254}
        };
        double meters = input * toMeters[from];
        return meters / toMeters[to];
    }
    
    if (category == "Масса") {
        QMap<QString, double> toGrams = {
            {"грамм", 1.0}, {"килограмм", 1000.0}, {"тонна", 1000000.0},
            {"фунт", 453.592}, {"унция", 28.3495}
        };
        double grams = input * toGrams[from];
        return grams / toGrams[to];
    }
    
    if (category == "Температура") {
        double celsius;
        if (from == "Цельсий") celsius = input;
        else if (from == "Фаренгейт") celsius = (input - 32) * 5.0 / 9.0;
        else celsius = input - 273.15;
        
        if (to == "Цельсий") return celsius;
        else if (to == "Фаренгейт") return celsius * 9.0 / 5.0 + 32;
        else return celsius + 273.15;
    }
    
    return input;
}

void MainWindow::onConvert()
{
    double input = spinInput->value();
    QString from = comboFrom->currentText();
    QString to = comboTo->currentText();
    QString cat = comboCategory->currentText();
    
    double result = convertValue(input, from, to, cat);
    
    labelResult->setText(QString("Результат: %1 %2 = %3 %4")
        .arg(input).arg(from).arg(result, 0, 'f', 4).arg(to));
    
    labelStatus->setText("Конвертация выполнена");
}

void MainWindow::onLiveConvert()
{
    onConvert();
}

void MainWindow::onCategoryChanged(int index)
{
    Q_UNUSED(index);
    updateUnits();
    onConvert();
}

void MainWindow::onThemeChanged(int index)
{
    QString theme = comboTheme->itemText(index);
    applyStyle(theme);
    labelStatus->setText(QString("Тема: %1").arg(theme));
}

void MainWindow::onClear()
{
    spinInput->setValue(0);
    labelResult->setText("Результат: —");
    labelStatus->setText("Очищено");
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
        onConvert();
    } else if (event->key() == Qt::Key_Escape) {
        onClear();
    } else if (event->key() == Qt::Key_Up) {
        int idx = comboCategory->currentIndex();
        if (idx > 0) comboCategory->setCurrentIndex(idx - 1);
    } else if (event->key() == Qt::Key_Down) {
        int idx = comboCategory->currentIndex();
        if (idx < comboCategory->count() - 1) comboCategory->setCurrentIndex(idx + 1);
    } else {
        QMainWindow::keyPressEvent(event);
    }
}