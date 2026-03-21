#include "TicketCardWidget.h"

#include "Ticket.h"

#include <QPainter>
#include <QVBoxLayout>

TicketCardWidget::TicketCardWidget(  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    const Ticket& ticket, bool is_selected,
    QWidget* parent)
    : QWidget(parent)
    , id_(ticket.id)
    , name_(ticket.name)
    , status_(ticket.status)
    , selected_(is_selected) {
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(5, 5, 5, 5);
    layout->setSpacing(2);

    id_label_ = new QLabel(QString::number(id_), this);
    id_label_->setAlignment(Qt::AlignCenter);
    id_label_->setStyleSheet("font-weight: bold; font-size: 14px; color: white;");

    name_label_ = new QLabel(name_, this);
    name_label_->setAlignment(Qt::AlignCenter);
    name_label_->setWordWrap(true);
    name_label_->setStyleSheet("font-size: 11px; color: white;");

    reset_button_ = new QPushButton("↺", this);
    reset_button_->setFixedSize(20, 20);
    reset_button_->setStyleSheet(
        "font-weight: bold; font-size: 12px; color: #ff6666; background: rgba(0,0,0,0.2); border: "
        "none;");
    reset_button_->setToolTip("Сбросить билет");

    layout->addWidget(id_label_);
    layout->addWidget(name_label_);
    layout->addWidget(reset_button_, 0, Qt::AlignRight | Qt::AlignBottom);

    connect(reset_button_, &QPushButton::clicked, this, &TicketCardWidget::OnResetClicked);

    setAttribute(Qt::WA_TranslucentBackground);
}

void TicketCardWidget::OnResetClicked() {
    emit ResetRequested(id_);  // NOLINT(readability-identifier-naming)
}

void TicketCardWidget::UpdateStatus(TicketStatus status) {
    status_ = status;
    update();
}

void TicketCardWidget::UpdateSelection(bool selected) {
    selected_ = selected;
    update();
}

void TicketCardWidget::UpdateName(const QString& name) {
    name_ = name;
    name_label_->setText(name);
}

void TicketCardWidget::paintEvent(  // NOLINT(readability-identifier-naming)
    QPaintEvent* /*event*/) {  // NOLINT(readability-identifier-naming)
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    QColor bg_color("#3d3d3d");
    if (status_ == TicketStatus::Yellow) {
        bg_color = QColor("#6b6b28");
    } else if (status_ == TicketStatus::Green) {
        bg_color = QColor("#2d5a2d");
    }

    painter.setBrush(bg_color);
    painter.setPen(Qt::NoPen);
    painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 5, 5);

    if (selected_) {
        painter.setBrush(Qt::NoBrush);
        painter.setPen(QPen(QColor("#4DAAF2"), 2));
        painter.drawRoundedRect(rect().adjusted(2, 2, -2, -2), 5, 5);
    }
}
