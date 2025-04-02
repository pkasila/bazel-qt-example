#ifndef EDITABLELABEL_H
#define EDITABLELABEL_H
#include <QWidget>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMessageBox>
//
#include "TicketCell.h"

class EditableLabel : public QLabel {
    Q_OBJECT

public:
    explicit EditableLabel(const QString &text, QWidget *parent = nullptr);

protected:
    void mouseDoubleClickEvent(QMouseEvent *event) override;

signals:
    void nameIsChanged(const QString& newName);

private:
    void handleTextEditing(QLineEdit *edit);
};
#endif // EDITABLELABEL_H
