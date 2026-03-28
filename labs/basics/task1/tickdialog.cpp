#include "tickdialog.h"

TickDialog::TickDialog(Ticket& ticket, QWidget *parent)
    : QDialog(parent), ticket_(ticket) {
    QVBoxLayout* layout = new QVBoxLayout(this);
    name_label_ = new QLabel("Имя билета: " + ticket.GetName(), this);
    layout->addWidget(name_label_);
    name_edit_ = new QLineEdit(ticket.GetName(), this);
    connect(name_edit_, &QLineEdit::returnPressed, this, &TickDialog::onNameChanged);
    layout->addWidget(name_edit_);
    status_label_ = new QLabel("Текущий статус:", this);
    layout->addWidget(status_label_);
    status_combo_ = new QComboBox(this);
    status_combo_->addItem("Не повторен", 0);
    status_combo_->addItem("Нужно повторить", 1);
    status_combo_->addItem("Выучен", 2);
    status_combo_->setCurrentIndex(ticket.GetStat());
    connect(status_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &TickDialog::onStatusChanged);
    layout->addWidget(status_combo_);
    hint_edit_ = new QTextEdit(ticket.GetHint(), this);
    connect(hint_edit_, &QTextEdit::textChanged, this, &TickDialog::onHintChanged);
    layout->addWidget(new QLabel("Подсказка:", this));
    layout->addWidget(hint_edit_);
    setWindowTitle("Информация о билете");
    setLayout(layout);
}

void TickDialog::onNameChanged() {
    if (!name_edit_->text().isEmpty()) {
        QString new_name = name_edit_->text();
        ticket_.ChangeName(new_name);
        name_label_->setText("Имя билета: " + new_name);
        emit nameChanged(ticket_.GetNum(), new_name);
    }
}

void TickDialog::onStatusChanged(int index) {
    ticket_.SetStatus(index);
    emit statusChanged(ticket_.GetNum(), index);
}

void TickDialog::onHintChanged() {
    QString new_hint = hint_edit_->toPlainText();
    ticket_.SetHint(new_hint);
    emit hintChanged(ticket_.GetNum(), new_hint);
}
