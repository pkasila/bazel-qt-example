#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QListWidget>
#include <QSpinBox>
#include <QLineEdit>
#include <QComboBox>
#include <QProgressBar>
#include <QPushButton>
#include <QTextEdit>
#include <QVector>
#include <QColor>

class MainWindow : public QMainWindow {
public:
    explicit MainWindow(QWidget* parent = nullptr);

private:
    enum class Status { Default = 0, Yellow = 1, Green = 2 };

    struct Ticket {
        QString name;
        Status status;
    };

    void init(int count);
    void updateAll();
    void updateOne(int index);
    void setCurrent(int index, bool useHistory = true);
    void applyStatus(int index, Status status);
    void updateProgress();

    static QColor color(Status status);

    QSpinBox* spin = nullptr;
    QListWidget* list = nullptr;

    QLabel* labelNumber = nullptr;
    QLabel* labelName = nullptr;

    QLineEdit* editName = nullptr;
    QComboBox* comboStatus = nullptr;

    QPushButton* buttonNext = nullptr;
    QPushButton* buttonPrev = nullptr;

    QProgressBar* progressTotal = nullptr;
    QProgressBar* progressGreen = nullptr;

    QVector<Ticket> tickets;
    QVector<int> history;

    int currentIndex = -1;
};

#endif // MAINWINDOW_H