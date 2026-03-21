#include "SlotMachineWidget.h"

#include <QHBoxLayout>
#include <QRandomGenerator>

SlotMachineWidget::SlotMachineWidget(
    QWidget* parent)  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    : QWidget(parent) {
    SetupUi();
}

void SlotMachineWidget::SetupUi() {
    auto* layout = new QHBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(20);

    reel1_ = new QLCDNumber(this);
    reel2_ = new QLCDNumber(this);
    reel3_ = new QLCDNumber(this);

    for (auto* reel : {reel1_, reel2_, reel3_}) {
        reel->setDigitCount(1);
        reel->display(0);
        reel->setSegmentStyle(QLCDNumber::Flat);
        reel->setMinimumSize(100, 150);
        reel->setStyleSheet(
            "QLCDNumber { background-color: #222; color: #ffaa00; border: 4px solid #444; "
            "border-radius: 10px; }");
        layout->addWidget(reel);
    }

    spin_timer_ = new QTimer(this);
    connect(spin_timer_, &QTimer::timeout, this, &SlotMachineWidget::OnTimerTick);
}

void SlotMachineWidget::StartSpinning() {
    if (is_spinning_) {
        return;
    }
    is_spinning_ = true;
    ticks_remaining_ = 40;
    spin_timer_->start(50);
}

bool SlotMachineWidget::IsSpinning() const {
    return is_spinning_;
}

std::vector<int> SlotMachineWidget::GetResults() const {
    return {
      static_cast<int>(reel1_->value()), static_cast<int>(reel2_->value()),
      static_cast<int>(reel3_->value())};
}

void SlotMachineWidget::OnTimerTick() {
    reel1_->display(static_cast<int>(QRandomGenerator::global()->bounded(10)));
    reel2_->display(static_cast<int>(QRandomGenerator::global()->bounded(10)));
    reel3_->display(static_cast<int>(QRandomGenerator::global()->bounded(10)));

    ticks_remaining_--;
    if (ticks_remaining_ <= 0) {
        spin_timer_->stop();
        is_spinning_ = false;
        emit SpinningStopped();
    }
}
