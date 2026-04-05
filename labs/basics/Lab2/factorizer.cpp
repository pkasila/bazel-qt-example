#include "factorizer.h"
#include <QMessageBox>
#include <QFont>
#include <QScrollBar>
#include <cmath>
#include <algorithm>
#include <vector>
#include <string>
#include <QApplication>

PrimeFactorizer::PrimeFactorizer(QWidget *parent)
    : QMainWindow(parent)
    , animationStep(0)
    , currentNumber(0)
{
    setupUI();
    
    animationTimer = new QTimer(this);
    connect(animationTimer, &QTimer::timeout, this, &PrimeFactorizer::onAnimationTimer);
}

PrimeFactorizer::~PrimeFactorizer()
{
}

void PrimeFactorizer::setupUI()
{
    QWidget *centralWidget = new QWidget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);
    
    QWidget *leftPanel = new QWidget();
    QVBoxLayout *leftLayout = new QVBoxLayout(leftPanel);
    
    QLabel *titleLabel = new QLabel("Prime Factorizer");
    titleLabel->setAlignment(Qt::AlignCenter);
    QFont titleFont = titleLabel->font();
    titleFont.setPointSize(18);
    titleFont.setBold(true);
    titleLabel->setFont(titleFont);
    leftLayout->addWidget(titleLabel);
    
    QGroupBox *inputGroup = new QGroupBox("Number Input");
    QVBoxLayout *inputLayout = new QVBoxLayout(inputGroup);
    
    QLabel *inputLabel = new QLabel("Enter a natural number:");
    numberInput = new QLineEdit();
    numberInput->setPlaceholderText("e.g., 84, 123456, 997");
    numberInput->setMaxLength(15);
    
    QHBoxLayout *buttonLayout = new QHBoxLayout();
    factorizeButton = new QPushButton("Factorize");
    factorizeButton->setEnabled(false);
    clearButton = new QPushButton("Clear");
    
    useSieveCheck = new QCheckBox("Use Sieve of Eratosthenes (faster for large numbers)");
    useSieveCheck->setChecked(false);

    useSieveCheck->setStyleSheet(
        "QCheckBox {"
        "    color: #333;"   
        "    font-weight: bold;"     
        "    font-size: 12px;"      
        "    spacing: 8px;"             
        "}"
        "QCheckBox::indicator {"
        "    width: 18px;"       
        "    height: 18px;"
        "}"
        "QCheckBox::indicator:checked {"
        "    background-color: #4CAF50;" 
        "    border: 2px solid #45a049;"
        "}"
        "QCheckBox::indicator:unchecked {"
        "    background-color: #ffffff;"
        "    border: 2px solid #cccccc;"
        "}"
        "QCheckBox:hover {"
        "    color: #3498db;"         
        "}"
    );

    buttonLayout->addWidget(factorizeButton);
    buttonLayout->addWidget(clearButton);
    
    inputLayout->addWidget(inputLabel);
    inputLayout->addWidget(numberInput);
    inputLayout->addLayout(buttonLayout);
    inputLayout->addWidget(useSieveCheck);
    leftLayout->addWidget(inputGroup);
    
    QGroupBox *settingsGroup = new QGroupBox("Algorithm Settings");
    QVBoxLayout *settingsLayout = new QVBoxLayout(settingsGroup);
    
    
    QHBoxLayout *sliderLayout = new QHBoxLayout();
    QLabel *sliderTextLabel = new QLabel("Animation speed:");
    delaySlider = new QSlider(Qt::Horizontal);
    delaySlider->setRange(0, 1000);
    delaySlider->setValue(200);
    delayLabel = new QLabel("200 ms");
    
    sliderLayout->addWidget(sliderTextLabel);
    sliderLayout->addWidget(delaySlider);
    sliderLayout->addWidget(delayLabel);
    
    settingsLayout->addLayout(sliderLayout);
    leftLayout->addWidget(settingsGroup);
    
    QGroupBox *progressGroup = new QGroupBox("Factorization Progress");
    QVBoxLayout *progressLayout = new QVBoxLayout(progressGroup);
    
    progressBar = new QProgressBar();
    progressBar->setRange(0, 100);
    progressBar->setTextVisible(true);
    statusLabel = new QLabel("Ready");
    statusLabel->setAlignment(Qt::AlignCenter);
    
    progressLayout->addWidget(progressBar);
    progressLayout->addWidget(statusLabel);
    leftLayout->addWidget(progressGroup);
    
    QWidget *rightPanel = new QWidget();
    QVBoxLayout *rightLayout = new QVBoxLayout(rightPanel);
    
    QGroupBox *resultGroup = new QGroupBox("Factorization Result");
    QVBoxLayout *resultLayout = new QVBoxLayout(resultGroup);
    
    resultDisplay = new QTextEdit();
    resultDisplay->setReadOnly(true);
    resultDisplay->setMinimumHeight(200);
    QFont monoFont("Courier New", 11);
    resultDisplay->setFont(monoFont);
    
    resultLayout->addWidget(resultDisplay);
    rightLayout->addWidget(resultGroup);

    QGroupBox *historyGroup = new QGroupBox("History (click to revisit)");
    QVBoxLayout *historyLayout = new QVBoxLayout(historyGroup);
    
    historyList = new QListWidget();
    historyList->setMaximumHeight(200);
    historyLayout->addWidget(historyList);
    rightLayout->addWidget(historyGroup);
    
    mainLayout->addWidget(leftPanel, 1);
    mainLayout->addWidget(rightPanel, 1);
    
    setCentralWidget(centralWidget);
    
    setWindowTitle("Prime Factorizer");
    resize(900, 700);
    
    setStyleSheet(
        "QMainWindow {"
        "    background-color: #f5f5f5;"
        "}"
        "QGroupBox {"
        "    font-weight: bold;"
        "    border: 2px solid #cccccc;"
        "    color: #333;"        
        "    border-radius: 5px;"
        "    margin-top: 1ex;"
        "    padding-top: 10px;"
        "}"
        "QGroupBox::title {"
        "    subcontrol-origin: margin;"
        "    left: 10px;"
        "    padding: 0 5px 0 5px;"
        "}"
        "QPushButton {"
        "    background-color: #4CAF50;"
        "    border: none;"
        "    color: white;"
        "    padding: 8px 16px;"
        "    border-radius: 4px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #45a049;"
        "}"
        "QPushButton:pressed {"
        "    background-color: #3d8b40;"
        "}"
        "QPushButton:disabled {"
        "    background-color: #cccccc;"
        "}"
        "#clearButton {"
        "    background-color: #f44336;"
        "}"
        "#clearButton:hover {"
        "    background-color: #da190b;"
        "}"
        "QLineEdit {"
        "    padding: 8px;"
        "    border: 2px solid #cccccc;"
        "    border-radius: 4px;"
        "    font-size: 12px;"
        "}"
        "QLineEdit:focus {"
        "    border-color: #4CAF50;"
        "}"
        "QTextEdit {"
        "    border: 2px solid #cccccc;"
        "    border-radius: 4px;"
        "    font-family: 'Courier New';"
        "}"
        "QProgressBar {"
        "    border: 2px solid #cccccc;"
        "    border-radius: 5px;"
        "    text-align: center;"
        "}"
        "QProgressBar::chunk {"
        "    background-color: #4CAF50;"
        "    border-radius: 3px;"
        "}"
        "QListWidget {"
        "    border: 2px solid #cccccc;"
        "    border-radius: 4px;"
        "}"
        "QListWidget::item {"
        "    padding: 5px;"
        "}"
        "QListWidget::item:selected {"
        "    background-color: #4CAF50;"
        "    color: white;"
        "}"
        "QLabel {"
        "    color: #333333;"
        "}"
        "QCheckBox {"
        "   color: #333;"
        "}"
    );
    
    clearButton->setObjectName("clearButton");
    
    connect(factorizeButton, &QPushButton::clicked, this, &PrimeFactorizer::onFactorizeClicked);
    connect(clearButton, &QPushButton::clicked, this, &PrimeFactorizer::onClearClicked);
    connect(numberInput, &QLineEdit::textChanged, this, &PrimeFactorizer::onNumberChanged);
    connect(useSieveCheck, &QCheckBox::toggled, this, &PrimeFactorizer::onUseSieveToggled);
    connect(delaySlider, &QSlider::valueChanged, this, &PrimeFactorizer::onDelayChanged);
    connect(historyList, &QListWidget::itemClicked, this, &PrimeFactorizer::onHistoryItemClicked);
    connect(numberInput, &QLineEdit::returnPressed, this, &PrimeFactorizer::onEnterPressed);
}

bool PrimeFactorizer::isPrime(long long n)
{
    if (n < 2) return false;
    if (n == 2) return true;
    if (n % 2 == 0) return false;
    
    long long limit = static_cast<long long>(std::sqrt(n));
    for (long long i = 3; i <= limit; i += 2) {
        if (n % i == 0) return false;
    }
    return true;
}

std::vector<long long> PrimeFactorizer::factorizeWithSieve(long long n)
{
    std::vector<long long> factors;
    long long num = n;
    
    while (num % 2 == 0) {
        factors.push_back(2);
        num /= 2;
    }
    
    long long limit = static_cast<long long>(std::sqrt(num));
    for (long long i = 3; i <= limit && num > 1; i += 2) {
        while (num % i == 0) {
            factors.push_back(i);
            num /= i;
            limit = static_cast<long long>(std::sqrt(num));
        }
    }
    
    if (num > 1) {
        factors.push_back(num);
    }
    
    return factors;
}

std::vector<long long> PrimeFactorizer::factorizeNumber(long long n)
{
    std::vector<long long> factors;
    long long num = n;
    
    if (useSieveCheck->isChecked()) {
        return factorizeWithSieve(n);
    }

    for (long long i = 2; i * i <= num; ++i) {
        while (num % i == 0) {
            factors.push_back(i);
            num /= i;
        }
    }
    
    if (num > 1) {
        factors.push_back(num);
    }
    
    return factors;
}

void PrimeFactorizer::animateFactorization(const std::vector<long long>& factors)
{
    currentFactors = factors;
    animationStep = 0;
    resultDisplay->clear();
    
    if (!factors.empty()) {
        animationTimer->start(delaySlider->value());
    } else {
        showResult(factors);
    }
}

void PrimeFactorizer::onAnimationTimer()
{
    if (animationStep < static_cast<int>(currentFactors.size())) {
        QString currentText = resultDisplay->toPlainText();
        if (animationStep == 0) {
            currentText = QString("%1 = ").arg(currentNumber);
        } else {
            currentText += " × ";
        }
        currentText += QString::number(currentFactors[animationStep]);
        resultDisplay->setPlainText(currentText);
        
        QScrollBar *sb = resultDisplay->verticalScrollBar();
        sb->setValue(sb->maximum());
        
        animationStep++;
        
        int progress = (animationStep * 100) / currentFactors.size();
        progressBar->setValue(progress);
        updateStatus(QString("Factoring... %1/%2 factors found")
                    .arg(animationStep).arg(currentFactors.size()));
    } else {
        animationTimer->stop();
        progressBar->setValue(100);
        updateStatus("Complete!");
        
        addToHistory(currentNumber, currentFactors);
    }
}

void PrimeFactorizer::showResult(const std::vector<long long>& factors)
{
    if (factors.empty()) {
        resultDisplay->setPlainText(QString("%1 is prime!").arg(currentNumber));
        updateStatus("Number is prime!");
        progressBar->setValue(100);
        addToHistory(currentNumber, factors);
    } else {
        QString result = QString("%1 = ").arg(currentNumber);
        for (size_t i = 0; i < factors.size(); ++i) {
            if (i > 0) result += " × ";
            result += QString::number(factors[i]);
        }
        resultDisplay->setPlainText(result);
        updateStatus("Complete!");
        progressBar->setValue(100);
        addToHistory(currentNumber, factors);
    }
}

void PrimeFactorizer::onFactorizeClicked()
{
    bool ok;
    long long number = numberInput->text().toLongLong(&ok);
    
    if (!ok || number < 2) {
        QMessageBox::warning(this, "Invalid Input", 
                            "Please enter a natural number greater than 1.");
        return;
    }
    
    if (number > 1000000000) {
        int ret = QMessageBox::question(this, "Large Number",
                                       "This number is very large and may take some time.\n"
                                       "Continue?",
                                       QMessageBox::Yes | QMessageBox::No);
        if (ret == QMessageBox::No) return;
    }
    
    currentNumber = number;
    progressBar->setValue(0);
    updateStatus("Factoring...");
    resultDisplay->clear();
    
    QApplication::processEvents();
    
    std::vector<long long> factors = factorizeNumber(number);
    
    if (delaySlider->value() > 0 && factors.size() > 1) {
        animateFactorization(factors);
    } else {
        showResult(factors);
    }
}

void PrimeFactorizer::onClearClicked()
{
    numberInput->clear();
    resultDisplay->clear();
    progressBar->setValue(0);
    updateStatus("Ready");
    currentFactors.clear();
    animationStep = 0;
    if (animationTimer->isActive()) {
        animationTimer->stop();
    }
}

void PrimeFactorizer::onNumberChanged(const QString &text)
{
    bool ok;
    long long number = text.toLongLong(&ok);
    factorizeButton->setEnabled(ok && number >= 2);
    
    if (ok && number >= 2) {
        updateStatus("Ready to factorize");
    } else if (!text.isEmpty()) {
        updateStatus("Please enter a number ≥ 2");
    } else {
        updateStatus("Ready");
    }
}

void PrimeFactorizer::onUseSieveToggled(bool checked)
{
    updateStatus(checked ? "Sieve algorithm enabled (faster for large numbers)" 
                         : "Trial division algorithm enabled");
}

void PrimeFactorizer::onDelayChanged(int value)
{
    delayLabel->setText(QString("%1 ms").arg(value));
    updateStatus(QString("Animation delay set to %1 ms").arg(value));
}

void PrimeFactorizer::addToHistory(long long number, const std::vector<long long>& factors)
{
    for (const auto& entry : history) {
        if (entry.first == number) {
            return;
        }
    }
    
    history.push_back({number, factors});
    
    if (history.size() > 20) {
        history.erase(history.begin());
    }
    
    updateHistoryDisplay();
}

void PrimeFactorizer::updateHistoryDisplay()
{
    historyList->clear();
    
    for (const auto& entry : history) {
        QString display;
        if (entry.second.empty()) {
            display = QString("%1 is prime").arg(entry.first);
        } else {
            display = QString("%1 = ").arg(entry.first);
            for (size_t i = 0; i < entry.second.size(); ++i) {
                if (i > 0) display += " × ";
                display += QString::number(entry.second[i]);
            }
        }
        
        QListWidgetItem *item = new QListWidgetItem(display);
        item->setData(Qt::UserRole, static_cast<long long>(entry.first));
        historyList->addItem(item);
    }
}

void PrimeFactorizer::onHistoryItemClicked(QListWidgetItem *item)
{
    if (item) {
        long long number = item->data(Qt::UserRole).toLongLong();
        numberInput->setText(QString::number(number));
        onFactorizeClicked();
    }
}

void PrimeFactorizer::onEnterPressed()
{
    if (factorizeButton->isEnabled()) {
        onFactorizeClicked();
    }
}

void PrimeFactorizer::updateStatus(const QString& status)
{
    statusLabel->setText(status);
}