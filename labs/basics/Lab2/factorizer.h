#ifndef PRIME_FACTORIZER_H
#define PRIME_FACTORIZER_H

#include <QMainWindow>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QTextEdit>
#include <QProgressBar>
#include <QSpinBox>
#include <QCheckBox>
#include <QSlider>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QGroupBox>
#include <QListWidget>
#include <QTimer>
#include <vector>
#include <string>

class PrimeFactorizer : public QMainWindow
{
    Q_OBJECT

public:
    PrimeFactorizer(QWidget *parent = nullptr);
    ~PrimeFactorizer();

private slots:
    void onFactorizeClicked();
    void onClearClicked();
    void onNumberChanged(const QString &text);
    void onUseSieveToggled(bool checked);
    void onDelayChanged(int value);
    void onHistoryItemClicked(QListWidgetItem *item);
    void onEnterPressed();
    void onAnimationTimer();

private:
    QLineEdit *numberInput;
    QPushButton *factorizeButton;
    QPushButton *clearButton;
    QTextEdit *resultDisplay;
    QProgressBar *progressBar;
    QCheckBox *useSieveCheck;
    QSlider *delaySlider;
    QLabel *delayLabel;
    QListWidget *historyList;
    QLabel *statusLabel;
    
    std::vector<std::pair<long long, std::vector<long long>>> history;
    std::vector<long long> currentFactors;
    int animationStep;
    QTimer *animationTimer;
    long long currentNumber;
    
    void setupUI();
    std::vector<long long> factorizeNumber(long long n);
    std::vector<long long> factorizeWithSieve(long long n);
    bool isPrime(long long n);
    void addToHistory(long long number, const std::vector<long long>& factors);
    void updateHistoryDisplay();
    void animateFactorization(const std::vector<long long>& factors);
    void updateStatus(const QString& status);
    void showResult(const std::vector<long long>& factors);
};

#endif // PRIME_FACTORIZER_H