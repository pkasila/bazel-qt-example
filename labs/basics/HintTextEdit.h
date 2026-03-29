#ifndef HINTTEXTEDIT_H
#define HINTTEXTEDIT_H

#include <QTextEdit>
#include <QKeyEvent>

class HintTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit HintTextEdit(QWidget *parent = nullptr) : QTextEdit(parent) {}

signals:
    void enterPressed();  // Сигнал, который будет emit при нажатии Enter

protected:
    void keyPressEvent(QKeyEvent *event) override {
        if (event->key() == Qt::Key_Return || event->key() == Qt::Key_Enter) {
            if (event->modifiers() & Qt::ShiftModifier) {
                // Если нажат Shift + Enter, добавляем новую строку
                QTextEdit::keyPressEvent(event);
            } else {
                // Если нажат Enter, генерируем сигнал
                emit enterPressed();
                event->accept();  // Принимаем событие, чтобы не добавлять новую строку
            }
        } else {
            QTextEdit::keyPressEvent(event);  // Обрабатываем другие клавиши как обычно
        }
    }
};

#endif // HINTTEXTEDIT_H
