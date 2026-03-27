#include "TicketCell.h"

TicketCell::TicketCell(int ticketNumber, const QString& ticketName,
           int ticketStatus, QWidget *parent) :
    QWidget(parent), ticketNumber(ticketNumber),
    ticketName(ticketName), ticketStatus(ticketStatus) {
    ticketNameLabel = new QLabel(ticketName, this);
    ticketNameLabel->setAlignment(Qt::AlignLeft);

    statusFlag = new QFrame(this);
    statusFlag->setFrameShape(QFrame::Box);
    statusFlag->setFrameStyle(QFrame::NoFrame);
    statusFlag->setStyleSheet(ColorMaps::statusStyleSheets[ticketStatus]);
    statusFlag->setFixedSize(Geometry::statusFlagSize);

    QHBoxLayout *cellLayout = new QHBoxLayout(this);
    cellLayout->addWidget(ticketNameLabel);
    cellLayout->addWidget(statusFlag);
    setLayout(cellLayout);
}

//Setters
void TicketCell::setTicketStatus(int ticketStatus) {
    emit additionalPointsScored(ticketStatus - this->ticketStatus);
    if (ticketStatus == Status::Green && this->ticketStatus != Status::Green) {
        emit additionalGreenTicketAppeared(1);
    } else if (ticketStatus != Status::Green && this->ticketStatus == Status::Green) {
        emit additionalGreenTicketAppeared(-1);
    }
    this->ticketStatus = ticketStatus;
    statusFlag->setStyleSheet(ColorMaps::statusStyleSheets[ticketStatus]);
}

void TicketCell::setTicketName(const QString& ticketName) {
    this->ticketName = ticketName;
    ticketNameLabel->setText(ticketName);
}

void TicketCell::setTicketText(const QString& ticketText) {
    this->ticketText = ticketText;
}

//Getters
int TicketCell::getTicketStatus() const {
    return this->ticketStatus;
}

QString TicketCell::getTicketName() const {
    return this->ticketName;
}

QString TicketCell::getTicketText() const {
    return this->ticketText;
}

int TicketCell::getTicketNumber() const {
    return this->ticketNumber;
}
