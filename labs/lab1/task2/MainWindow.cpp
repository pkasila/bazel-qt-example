#include "MainWindow.h"

#include <QGridLayout>
#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(
    QWidget* parent)  // NOLINT(cppcoreguidelines-pro-type-member-init,hicpp-member-init)
    : QWidget(parent) {
    SetupUi();
    UpdateControls();
}

void MainWindow::SetupUi() {
    auto* main_layout = new QVBoxLayout(this);
    main_layout->setContentsMargins(20, 20, 20, 20);
    main_layout->setSpacing(20);

    info_label_ = new QLabel("Сделайте ставку", this);
    info_label_->setAlignment(Qt::AlignCenter);
    info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");

    slot_machine_ = new SlotMachineWidget(this);

    auto* controls_layout = new QGridLayout();
    controls_layout->setSpacing(15);

    auto* bet_label = new QLabel("Ставка:", this);
    bet_label->setStyleSheet("color: white; font-size: 16px;");
    bet_spin_box_ = new QSpinBox(this);
    bet_spin_box_->setRange(10, 500);
    bet_spin_box_->setValue(50);
    bet_spin_box_->setSingleStep(10);
    bet_spin_box_->setStyleSheet(
        "QSpinBox { background-color: #333; color: white; border: 1px solid #555; padding: 5px; "
        "font-size: 16px; }");

    auto_play_check_ = new QCheckBox("Авто-игра", this);
    auto_play_check_->setStyleSheet("color: white; font-size: 16px;");

    spin_button_ = new QPushButton("Дернуть рычаг!", this);
    spin_button_->setMinimumHeight(50);
    spin_button_->setStyleSheet(
        "QPushButton { background-color: #e63946; color: white; font-weight: bold; font-size: "
        "18px; border-radius: "
        "5px; }"
        "QPushButton:hover { background-color: #ff4d4d; }"
        "QPushButton:disabled { background-color: #555; color: #888; }");

    balance_bar_ = new QProgressBar(this);
    balance_bar_->setRange(0, 5000);
    balance_bar_->setValue(balance_);
    balance_bar_->setFormat("%v монет");
    balance_bar_->setAlignment(Qt::AlignCenter);
    balance_bar_->setFixedHeight(30);
    balance_bar_->setStyleSheet(
        "QProgressBar { border: 2px solid #444; border-radius: 5px; background-color: #222; color: "
        "white; "
        "font-weight: bold; }"
        "QProgressBar::chunk { background-color: #2a9d8f; width: 10px; margin: 1px; }");

    controls_layout->addWidget(bet_label, 0, 0);
    controls_layout->addWidget(bet_spin_box_, 0, 1);
    controls_layout->addWidget(auto_play_check_, 0, 2);
    controls_layout->addWidget(spin_button_, 1, 0, 1, 3);
    controls_layout->addWidget(balance_bar_, 2, 0, 1, 3);

    restart_button_ = new QPushButton("Начать заново", this);
    restart_button_->setMinimumHeight(35);
    restart_button_->setStyleSheet(
        "QPushButton { background-color: #457b9d; color: white; font-weight: bold; font-size: "
        "14px; border-radius: "
        "5px; }"
        "QPushButton:hover { background-color: #1d3557; }"
        "QPushButton:disabled { background-color: #555; color: #888; }");

    controls_layout->addWidget(restart_button_, 3, 0, 1, 3);

    main_layout->addWidget(info_label_);
    main_layout->addWidget(slot_machine_, 1, Qt::AlignCenter);
    main_layout->addLayout(controls_layout);

    auto_play_timer_ = new QTimer(this);
    auto_play_timer_->setSingleShot(true);

    connect(spin_button_, &QPushButton::clicked, this, &MainWindow::OnSpinClicked);
    connect(
        slot_machine_, &SlotMachineWidget::SpinningStopped, this, &MainWindow::OnSpinningStopped);
    connect(
        bet_spin_box_, QOverload<int>::of(&QSpinBox::valueChanged), this,
        &MainWindow::OnBetChanged);
    connect(auto_play_check_, &QCheckBox::toggled, this, &MainWindow::OnAutoPlayToggled);
    connect(auto_play_timer_, &QTimer::timeout, this, &MainWindow::OnAutoPlayTimerTick);
    connect(restart_button_, &QPushButton::clicked, this, &MainWindow::OnRestartClicked);

    setStyleSheet("background-color: #1a1a1a;");
    setMinimumSize(600, 500);
}

void MainWindow::UpdateControls() {
    if (!slot_machine_->IsSpinning()) {
        bet_spin_box_->setMaximum(std::max(1, balance_));
    }

    const bool can_spin =  // NOLINT(cppcoreguidelines-init-variables)
        !slot_machine_->IsSpinning() && balance_ >= bet_spin_box_->value() && balance_ > 0;

    spin_button_->setEnabled(can_spin);
    bet_spin_box_->setEnabled(!slot_machine_->IsSpinning() && balance_ > 0);
    auto_play_check_->setEnabled(!slot_machine_->IsSpinning() && balance_ > 0);
    restart_button_->setEnabled(!slot_machine_->IsSpinning());

    if (balance_ == 0 && !slot_machine_->IsSpinning()) {
        info_label_->setText("Вы банкрот! Нажмите 'Начать заново'.");
        info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: #e63946;");
    }
}

void MainWindow::OnSpinClicked() {
    if (balance_ < bet_spin_box_->value() || balance_ == 0) {
        return;
    }

    current_bet_ = bet_spin_box_->value();
    balance_ -= current_bet_;
    balance_bar_->setValue(balance_);

    info_label_->setText("Крутим...");
    info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");

    slot_machine_->StartSpinning();
    UpdateControls();
}

void MainWindow::OnSpinningStopped() {
    ProcessResults();
    UpdateControls();

    if (auto_play_check_->isChecked() && balance_ >= bet_spin_box_->value() &&
        balance_ > 0) {  // NOLINT(bugprone-branch-clone)
        auto_play_timer_->start(1000);
    } else if (auto_play_check_->isChecked()) {
        auto_play_check_->setChecked(false);
    }
}

void MainWindow::ProcessResults() {
    const auto results = slot_machine_->GetResults();
    const int bet = current_bet_;

    if (results[0] == 7 && results[1] == 7 && results[2] == 7) {
        const int win_amount = bet * 50;
        balance_ += win_amount;
        info_label_->setText(
            QString("МЕГА-ДЖЕКПОТ! [7][7][7] Вы выиграли %1 монет!").arg(win_amount));
        info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffb703;");
    } else if (results[0] == results[1] && results[1] == results[2]) {
        const int win_amount = bet * 10;
        balance_ += win_amount;
        info_label_->setText(QString("ДЖЕКПОТ! Вы выиграли %1 монет!").arg(win_amount));
        info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: #ffd700;");
    } else if (results[0] == results[1] || results[1] == results[2] || results[0] == results[2]) {
        const int win_amount = bet * 2;
        balance_ += win_amount;
        info_label_->setText(QString("Успех! Вы выиграли %1 монет!").arg(win_amount));
        info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: #2a9d8f;");
    } else {
        info_label_->setText(balance_ == 0 ? "Проигрыш. Вы банкрот!" : "Проигрыш");
        info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: #e63946;");
    }

    if (balance_ > balance_bar_->maximum()) {
        balance_bar_->setMaximum(balance_ * 2);
    }
    balance_bar_->setValue(balance_);
}

void MainWindow::OnBetChanged([[maybe_unused]] int bet) {
    UpdateControls();
}

void MainWindow::OnAutoPlayToggled(  // NOLINT(readability-convert-member-functions-to-static)
    [[maybe_unused]] bool checked) {
    if (auto_play_check_->isChecked() && !slot_machine_->IsSpinning() &&
        balance_ >= bet_spin_box_->value()) {
        OnSpinClicked();
    }
}

void MainWindow::OnAutoPlayTimerTick() {  // NOLINT(readability-convert-member-functions-to-static)
    if (auto_play_check_->isChecked() && balance_ >= bet_spin_box_->value() &&
        !slot_machine_->IsSpinning() && balance_ > 0) {
        OnSpinClicked();
    }
}

void MainWindow::OnRestartClicked() {
    balance_ = 1000;
    balance_bar_->setMaximum(5000);
    balance_bar_->setValue(balance_);
    info_label_->setText("Сделайте ставку");
    info_label_->setStyleSheet("font-size: 24px; font-weight: bold; color: white;");
    UpdateControls();
}
