#pragma once
#include <QDialog>
#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <vector>
#include <QDebug>

class Dialog : public QDialog {
    Q_OBJECT
public:
    Dialog(const std::vector<QString>& steps, QWidget* parent = nullptr) : QDialog(parent), items(steps), currentIndex(0) {
        setWindowTitle("Шаги для выполнения");
        setModal(false);
        resize(200, 50);

        layout = new QVBoxLayout(this);

        prevButton = new QPushButton("Назад", this);
        nextButton = new QPushButton("Вперёд", this);

        label = new QLabel(this);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("background: rgb(80, 211, 7); font: solid black");
        updateLabel();

        connect(prevButton, &QPushButton::clicked, this, &Dialog::showPrevious);
        connect(nextButton, &QPushButton::clicked, this, &Dialog::showNext);

        layout->addWidget(label);
        layout->addWidget(prevButton);
        layout->addWidget(nextButton);

        setLayout(layout);
    }

private Q_SLOTS:
    void showPrevious() {
        if (currentIndex == 0) {
            return;
        }
        currentIndex--;
        updateLabel();
    }

    void showNext() {
        if (currentIndex == items.size() - 1) {
            return;
        }
        currentIndex++;
        updateLabel();
    }
private:
    void updateLabel() {
        label->setText(items[currentIndex]);
        prevButton->setEnabled(currentIndex > 0);
        nextButton->setEnabled(currentIndex < items.size() - 1);
    }
    int currentIndex;
    QVBoxLayout* layout;
    QLabel* label;
    QPushButton* prevButton;
    QPushButton* nextButton;
    std::vector<QString> items;
};
