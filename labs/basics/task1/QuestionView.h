#pragma once

#include <QGridLayout>
#include <QLabel>
#include <QColor>
#include <QLineEdit>
#include <QGroupBox>
#include <QComboBox>


const QColor grey = QColor(176, 184, 184);
const QColor black = QColor(42, 51, 50);
const QColor yellow = QColor(248, 250, 27);
const QColor green =    QColor(34, 216, 32);
const QColor priority[3] = {grey, yellow, green};

inline const int colorIndex(const QColor& color) {
    return (color == grey ? 0 : (color == yellow ? 1 : 2));
}

class QuestionView : public QWidget {
    Q_OBJECT
public:
QuestionView(QWidget* parent = nullptr) : QWidget(parent) {
        layout = new QVBoxLayout(this);

        groupBox = new QGroupBox("Информация о билете", this);

        groupLayout = new QVBoxLayout(groupBox);

        numberLabel = new QLabel(groupBox);
        numberLabel->setStyleSheet("background: rgb(221, 24, 24)");
        groupLayout->addWidget(numberLabel);

        nameLabel = new QLabel(groupBox);
        groupLayout->addWidget(nameLabel);

        nameEdit = new QLineEdit(groupBox);
        groupLayout->addWidget(nameEdit);
        connect(nameEdit, &QLineEdit::returnPressed, this, &QuestionView::updateName);

        status = new QComboBox(groupBox);
        status->addItems({"Ещё не учил", "Учил, но хочу вернуться", "Выучил"});
        status->setPlaceholderText("Выберите статус"); 
        status->setCurrentIndex(-1);

        groupLayout->addWidget(status);
        connect(status, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &QuestionView::updateStatus);

        groupBox->setLayout(groupLayout);

        layout->addWidget(groupBox);

        setLayout(layout);
    }

    void setNumber(const int number) {
        index = number;
        numberLabel->setText(QString("%1").arg(number));
        numberLabel->setAlignment(Qt::AlignCenter);
    }

    int getNumber() const {
        return index - 1;
    }

    void setName(const QString& name) {
        nameLabel->setText(name);
    }
Q_SIGNALS:
    void nameUpdated(const QString& name);
    void statusUpdated(const QColor& color);

private Q_SLOTS:
    void updateName() {
        QString name = nameEdit->text();
        if(!name.isEmpty()) {
            setName(name);
            Q_EMIT nameUpdated(nameEdit->text());
        }
    }

    void updateStatus(int index) {
        if (index == -1) {
            return;
        }
        //qDebug() << index;
        Q_EMIT statusUpdated(priority[index]);
        //qDebug() << "Selected:" << status->itemText(index);
        status->setCurrentIndex(-1);
    }

private:
    int index = 0;
    QVBoxLayout* layout;
    QVBoxLayout* groupLayout;
    QGroupBox* groupBox;
    QLabel* numberLabel;
    QLabel* nameLabel;
    QLineEdit* nameEdit;
    QComboBox* status;
};
