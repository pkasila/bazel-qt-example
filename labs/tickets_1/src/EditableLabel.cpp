#include "EditableLabel.h"

EditableLabel::EditableLabel(const QString &text, QWidget *parent) :
    QLabel(text, parent) {
    setStyleSheet(Styles::editableLabelStyle);
}

void EditableLabel::mouseDoubleClickEvent(QMouseEvent *event) {
    if (event->button() == Qt::LeftButton) {
        //Creating QLineEdit
        QLineEdit *edit = new QLineEdit(text(), this->parentWidget());
        edit->setGeometry(this->geometry());  //Copying geometry of edit
        edit->setAlignment(Qt::AlignCenter);
        edit->setMaxLength(Parameters::MaxNumberOfSymbolsInEditableLabel);
        edit->setText(text());
        edit->setFrame(false);

        //Update
        edit->setFocus();
        edit->show();

        this->hide();

        connect(edit, &QLineEdit::editingFinished, [=]() {
            handleTextEditing(edit);
        });
    }
    QLabel::mouseDoubleClickEvent(event);
}

void EditableLabel::handleTextEditing(QLineEdit *edit) {
    if (edit->text().isEmpty()) {
        QMessageBox::warning(this, WindowTitles::warningWindowTitle,
                             WindowTexts::lableWarningText);
        edit->setFocus();
    } else {
        this->setText(edit->text());
        this->show();
        emit nameIsChanged(edit->text());
        edit->deleteLater();
    }
}
