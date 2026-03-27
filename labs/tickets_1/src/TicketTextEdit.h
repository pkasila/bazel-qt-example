#ifndef TICKETTEXTEDIT_H
#define TICKETTEXTEDIT_H
#include <QWidget>
#include <QTextEdit>
#include <QFocusEvent>
#include <QString>

class TicketTextEdit : public QTextEdit {
    Q_OBJECT

public:
    explicit TicketTextEdit(QWidget *parent = nullptr);

protected:
    void focusOutEvent(QFocusEvent *event) override;

signals:
    void editingFinished(const QString& text);
};

#endif // TICKETTEXTEDIT_H
