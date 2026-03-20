#include "ticketdialog.h"

#include <QWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QString>

TicketDialog::TicketDialog(MainWindow *parent, QListWidgetItem *item)
    : QDialog(parent){
    window_ = parent;
    item_ = item;
    index_ = window_->listWidget_->row(item_);
    status_ = &window_->status_[window_->listWidget_->row(item_)];

    this->setStyleSheet("background-color: #352e4a;");
    this->setWindowTitle("Ticket Info");

    QVBoxLayout *layout = new QVBoxLayout(this);
    QHBoxLayout *idLayout = new QHBoxLayout;
    QHBoxLayout *nameLayout = new QHBoxLayout;
    QHBoxLayout *setNewNameLayout = new QHBoxLayout;
    QHBoxLayout *statusLayout = new QHBoxLayout;

    labelTicketID_ = new QLabel("Ticket №"
     + QString::fromStdString(std::to_string(index_ + 1)), this);
    labelTicketID_->setStyleSheet("color: white;");
    idLayout->addWidget(labelTicketID_);

    idLayout->addStretch(1);

    labelTicketName_ = new QLabel(item_->text(), this);
    labelTicketName_->setStyleSheet("color: white;");
    labelTicketName_->setWordWrap(true);
    labelTicketName_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    labelTicketName_->setMaximumWidth(1080);
    nameLayout->addWidget(labelTicketName_);
    nameLayout->addStretch(1);

    lineEditNewName_ = new QLineEdit(this);
    lineEditNewName_->setStyleSheet("color: white; background-color: #241f31");
    lineEditNewName_->setSizePolicy(QSizePolicy::Expanding,
                                    QSizePolicy::Fixed);
    buttonSetNewName_ = new QPushButton("Set new ticket name", this);
    buttonSetNewName_->setStyleSheet("color: white;");
    buttonSetNewName_->setSizePolicy(QSizePolicy::Fixed,
                                     QSizePolicy::Preferred);
    setNewNameLayout->addWidget(lineEditNewName_);
    setNewNameLayout->addWidget(buttonSetNewName_);

    labelStatus_ = new QLabel("Status: ", this);
    labelStatus_->setStyleSheet("color: white;");
    comboBoxStatus_ = new QComboBox(this);
    comboBoxStatus_->addItem("Не рассмотрен");
    comboBoxStatus_->addItem("Частично готов");
    comboBoxStatus_->addItem("Готов");
    comboBoxStatus_->setCurrentIndex(*status_);
    comboBoxStatus_->setStyleSheet(
        "QComboBox {"
        "   background-color: #352e4a;"
        "   color: white;"
        "}"
        "QComboBox QAbstractItemView {"
        "   background-color: #5d5280;"
        "   color: #000080;"
        "   selection-background-color: #625685;"
        "   selection-color: #000080;"
        "}"
        );

    statusLayout->addWidget(labelStatus_);
    statusLayout->addWidget(comboBoxStatus_);
    statusLayout->addStretch(1);

    layout->addLayout(idLayout);
    layout->addLayout(nameLayout);
    layout->addLayout(setNewNameLayout);
    layout->addLayout(statusLayout);
    layout->addStretch(1);

    connect(comboBoxStatus_,
            QOverload<int>::of(&QComboBox::currentIndexChanged),
            this,
            &TicketDialog::ChangeStatus);

    connect(buttonSetNewName_, &QPushButton::clicked,
            this, &TicketDialog::SetNewName);
}

void TicketDialog::ChangeStatus(int newStatus) {
    if (*status_ == 0 && newStatus == 1) {
        window_->cntStarted_++;
    } else if(*status_ == 0 && newStatus == 2) {
        window_->cntStarted_++;
        window_->cntTotal_++;
    } else if(*status_ == 1 && newStatus == 0) {
        window_->cntStarted_--;
    } else if(*status_ == 1 && newStatus == 2) {
        window_->cntTotal_++;
    } else if(*status_ == 2 && newStatus == 0) {
        window_->cntStarted_--;
        window_->cntTotal_--;
    } else if(*status_ == 2 && newStatus == 1) {
        window_->cntTotal_--;
    }
    *status_ = newStatus;
    window_->progBarStarted_->setValue(static_cast<int>(100.0* window_->cntStarted_/window_->cnt_));
    window_->progBarTotal_->setValue(static_cast<int>(100.0*window_->cntTotal_/window_->cnt_));

    QColor clGreen(51, 209, 122);
    QColor clYellow(248, 228, 92);
    QColor clDefault(40, 62, 86);
    QColor clWhite(222, 221, 218);
    QColor clBlack(0, 0, 0);
    QColor colorBg;
    QColor colorFg;
    if (newStatus == 0){
        colorBg = clDefault;
        colorFg = clWhite;
    } else if (newStatus == 1) {
        colorBg = clYellow;
        colorFg = clBlack;
    } else {
        colorBg = clGreen;
        colorFg = clBlack;
    }
    item_->setBackground(colorBg);
    item_->setForeground(colorFg);

}

void TicketDialog::SetNewName() {
    if (lineEditNewName_->text() != "") {
        labelTicketName_->setText(lineEditNewName_->text());
        item_->setText(lineEditNewName_->text());
    }
}
