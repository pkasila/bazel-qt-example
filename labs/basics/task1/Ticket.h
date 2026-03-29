#pragma once

#include "QuestionView.h"
#include "QuestionManager.h"
#include <QListWidget>

class Ticket : public QListWidgetItem {
public:
    explicit Ticket(const QString& text, QListWidget* parent = nullptr)
        : QListWidgetItem(text, parent), question(new QuestionView(dynamic_cast<QWidget*>(this))) {
        setBackground(grey);
        setForeground(black);
        QFont font("Areal", 14);
        setFont(font);
        setData(Qt::UserRole + 1, 2);
    }

    QuestionView* getQuestionView() const {
        return question;
    }

    ~Ticket() {
        delete question;
    }

    void setColor(const QColor& color, QuestionManager* manager, int& total_value, int& green_value) {
        total_value += colorIndex(color) - colorIndex(curColor);
        green_value += (color == green) - (curColor == green);
        setBackground(color);
        manager->changeType(getNumber(), colorIndex(color));
        curColor = color;
    }

    QColor getColor() const {
        return curColor;
    }

    void setNumber(const int number) {
        question->setNumber(number);
    }

    const int getNumber() const {
        return question->getNumber();
    }

    void setName(const QString& name) {
        setText(name);
        question->setName(name);
    }

private:
    QColor curColor = grey;
    QuestionView* question;
};
