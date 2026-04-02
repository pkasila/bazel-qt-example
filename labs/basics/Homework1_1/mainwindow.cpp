#include "mainwindow.h"

#include <QColor>
#include <QComboBox>
#include <QDialog>
#include <QDialogButtonBox>
#include <QDir>
#include <QFile>
#include <QFormLayout>
#include <QFrame>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QProgressBar>
#include <QPushButton>
#include <QRandomGenerator>
#include <QSizePolicy>
#include <QSpinBox>
#include <QStandardPaths>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

MainWindow::MainWindow(QWidget* parent) : QMainWindow(parent) {
    SetupUi();
    RebuildTickets(count_spinbox_->value());
    LoadFromFile();
}

QString MainWindow::DefaultTicketName(int index) const {
    return QString("Билет %1").arg(index + 1);
}

QColor MainWindow::BackgroundColorForStatus(TicketStatus status) const {
    switch (status) {
        case DefaultStatus:
            return QColor(229, 233, 238);
        case YellowStatus:
            return QColor(242, 231, 184);
        case GreenStatus:
            return QColor(195, 223, 201);
    }
    return QColor(229, 233, 238);
}

QColor MainWindow::StripeColorForStatus(TicketStatus status) const {
    switch (status) {
        case DefaultStatus:
            return QColor(130, 140, 150);
        case YellowStatus:
            return QColor(190, 156, 58);
        case GreenStatus:
            return QColor(76, 140, 92);
    }
    return QColor(130, 140, 150);
}

QWidget* MainWindow::CreateTicketWidget(int index) const {
    auto* root = new QWidget();
    root->setObjectName("ticketRoot");

    auto* layout = new QHBoxLayout(root);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);

    auto* stripe = new QFrame(root);
    stripe->setFixedWidth(8);
    stripe->setObjectName("ticketStripe");

    auto* content = new QWidget(root);
    auto* content_layout = new QVBoxLayout(content);
    content_layout->setContentsMargins(12, 10, 12, 10);
    content_layout->setSpacing(4);

    auto* number = new QLabel(QString("Билет %1").arg(index + 1), content);
    number->setStyleSheet("font-weight: 700; color: #22303c; background: transparent;");

    auto* name = new QLabel(tickets_[index].name, content);
    name->setWordWrap(true);
    name->setStyleSheet("color: #2f3b46; background: transparent;");

    auto* hint_marker = new QLabel(tickets_[index].hint.trimmed().isEmpty() ? "" : "Есть подсказка", content);
    hint_marker->setStyleSheet("color: #5a6b79; font-size: 12px; background: transparent;");

    content_layout->addWidget(number);
    content_layout->addWidget(name);
    content_layout->addWidget(hint_marker);

    layout->addWidget(stripe);
    layout->addWidget(content);

    QColor bg = BackgroundColorForStatus(tickets_[index].status);
    QColor stripe_color = StripeColorForStatus(tickets_[index].status);

    root->setStyleSheet(QString(
        "#ticketRoot {"
        "background-color: rgb(%1,%2,%3);"
        "border: 1px solid #cfd7df;"
        "border-radius: 10px;"
        "}"
        "#ticketStripe {"
        "background-color: rgb(%4,%5,%6);"
        "border-top-left-radius: 10px;"
        "border-bottom-left-radius: 10px;"
        "}"
    )
        .arg(bg.red()).arg(bg.green()).arg(bg.blue())
        .arg(stripe_color.red()).arg(stripe_color.green()).arg(stripe_color.blue()));

    root->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    return root;
}

QString MainWindow::SaveFilePath() const {
    QString dir = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (dir.isEmpty()) {
        dir = QDir::homePath() + "/.tickets_app";
    }
    QDir().mkpath(dir);
    return dir + "/progress.json";
}

bool MainWindow::SaveToFile() {
    QJsonObject root;
    root["count"] = tickets_.size();
    root["current_index"] = current_index_;

    QJsonArray history_array;
    for (int idx : history_) {
        history_array.append(idx);
    }
    root["history"] = history_array;

    QJsonArray tickets_array;
    for (const auto& ticket : tickets_) {
        QJsonObject obj;
        obj["name"] = ticket.name;
        obj["hint"] = ticket.hint;
        obj["status"] = static_cast<int>(ticket.status);
        tickets_array.append(obj);
    }
    root["tickets"] = tickets_array;

    QFile file(SaveFilePath());
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    file.write(QJsonDocument(root).toJson(QJsonDocument::Indented));
    return true;
}

bool MainWindow::LoadFromFile() {
    QFile file(SaveFilePath());
    if (!file.exists()) {
        return false;
    }
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    const QByteArray data = file.readAll();
    const QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()) {
        return false;
    }

    const QJsonObject root = doc.object();
    const int count = root["count"].toInt(-1);
    if (count <= 0) {
        return false;
    }

    count_spinbox_->blockSignals(true);
    count_spinbox_->setValue(count);
    count_spinbox_->blockSignals(false);

    tickets_.clear();
    tickets_.resize(count);

    const QJsonArray tickets_array = root["tickets"].toArray();
    for (int i = 0; i < count; ++i) {
        if (i < tickets_array.size() && tickets_array[i].isObject()) {
            QJsonObject obj = tickets_array[i].toObject();
            tickets_[i].name = obj["name"].toString(DefaultTicketName(i));
            tickets_[i].hint = obj["hint"].toString("");
            tickets_[i].status = static_cast<TicketStatus>(obj["status"].toInt(0));
        } else {
            tickets_[i].name = DefaultTicketName(i);
            tickets_[i].hint = "";
            tickets_[i].status = DefaultStatus;
        }
    }

    history_.clear();
    const QJsonArray history_array = root["history"].toArray();
    for (const auto& v : history_array) {
        int idx = v.toInt();
        if (idx >= 0 && idx < tickets_.size()) {
            history_.push(idx);
        }
    }

    current_index_ = root["current_index"].toInt(0);
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        current_index_ = tickets_.isEmpty() ? -1 : 0;
    }

    UpdateView();
    UpdateProgressBars();
    UpdateQuestionView();

    if (current_index_ >= 0) {
        view_->setCurrentRow(current_index_);
    }

    return true;
}

void MainWindow::SetupUi() {
    auto* central = new QWidget(this);
    setCentralWidget(central);

    setWindowTitle("Повторение билетов");
    resize(1200, 760);
    setMinimumSize(980, 660);

    central->setStyleSheet(R"(
        QWidget {
            background-color: #f4f6f8;
            color: #2f3640;
            font-size: 14px;
        }

        QGroupBox {
            background-color: #ffffff;
            border: 1px solid #d8dee4;
            border-radius: 12px;
            margin-top: 12px;
            padding-top: 12px;
            font-weight: 600;
        }

        QGroupBox::title {
            subcontrol-origin: margin;
            left: 14px;
            padding: 0 6px 0 6px;
            color: #2d3748;
        }

        QLabel {
            background: transparent;
        }

        QLineEdit, QComboBox, QSpinBox {
            background-color: #fbfcfd;
            border: 1px solid #bcc7d1;
            border-radius: 8px;
            padding: 6px 8px;
            min-height: 32px;
            color: #1f2933;
        }

        QLineEdit:focus, QComboBox:focus, QSpinBox:focus {
            border: 2px solid #4f83b3;
            background-color: #ffffff;
        }

        QPushButton {
            background-color: #3f6f99;
            color: white;
            border: 1px solid #315978;
            border-radius: 10px;
            padding: 8px 14px;
            min-height: 38px;
            font-weight: 600;
        }

        QPushButton:hover {
            background-color: #4b7ba5;
        }

        QPushButton:pressed {
            background-color: #355d7e;
        }

        QPushButton:disabled {
            background-color: #b9c3cc;
            color: #eef2f5;
            border: 1px solid #a7b1ba;
        }

        QListWidget {
            background-color: #ffffff;
            border: 1px solid #d8dee4;
            border-radius: 12px;
            padding: 8px;
            outline: 0;
        }

        QListWidget::item {
            margin: 5px 3px;
        }

        QListWidget::item:selected {
            background: transparent;
        }

        QProgressBar {
            background-color: #edf1f4;
            border: 1px solid #d7dde3;
            border-radius: 8px;
            text-align: center;
            min-height: 26px;
            color: #25323d;
            font-weight: 600;
        }

        QProgressBar::chunk {
            background-color: #6d9bb7;
            border-radius: 7px;
        }
    )");

    count_spinbox_ = new QSpinBox(this);
    count_spinbox_->setMinimum(1);
    count_spinbox_->setMaximum(500);
    count_spinbox_->setValue(30);
    count_spinbox_->setButtonSymbols(QAbstractSpinBox::NoButtons);
    count_spinbox_->setFixedWidth(90);

    minus_button_ = new QPushButton("−", this);
    plus_button_ = new QPushButton("+", this);

    minus_button_->setFixedSize(36, 32);
    plus_button_->setFixedSize(36, 32);

    minus_button_->setStyleSheet(R"(
        QPushButton {
            background-color: #8c5a5a;
            color: white;
            border: 1px solid #734747;
            border-radius: 8px;
            font-size: 18px;
            font-weight: 700;
            padding: 0px;
        }
        QPushButton:hover { background-color: #9b6666; }
        QPushButton:pressed { background-color: #7b4f4f; }
    )");

    plus_button_->setStyleSheet(R"(
        QPushButton {
            background-color: #4f7e5f;
            color: white;
            border: 1px solid #40684e;
            border-radius: 8px;
            font-size: 18px;
            font-weight: 700;
            padding: 0px;
        }
        QPushButton:hover { background-color: #5a8d6b; }
        QPushButton:pressed { background-color: #466f54; }
    )");

    save_button_ = new QPushButton("Сохранить", this);
    load_button_ = new QPushButton("Загрузить", this);
    reset_button_ = new QPushButton("Сбросить всё", this);

    save_button_->setFixedHeight(32);
    load_button_->setFixedHeight(32);
    reset_button_->setFixedHeight(32);

    auto* count_label = new QLabel("Количество билетов:", this);

    auto* top_layout = new QHBoxLayout();
    top_layout->addWidget(save_button_);
    top_layout->addWidget(load_button_);
    top_layout->addWidget(reset_button_);
    top_layout->addSpacing(12);
    top_layout->addWidget(count_label);
    top_layout->addWidget(minus_button_);
    top_layout->addWidget(count_spinbox_);
    top_layout->addWidget(plus_button_);
    top_layout->addStretch();

    view_ = new QListWidget(this);
    view_->setSelectionMode(QAbstractItemView::SingleSelection);
    view_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view_->setSpacing(4);

    question_box_ = new QGroupBox("Текущий билет", this);
    question_box_->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    number_label_ = new QLabel("-", this);
    name_label_ = new QLabel("-", this);
    name_label_->setWordWrap(true);
    name_label_->setMinimumHeight(40);

    name_edit_ = new QLineEdit(this);
    name_edit_->setPlaceholderText("Введите новое имя билета и нажмите Enter");

    status_combo_ = new QComboBox(this);
    status_combo_->addItem("Не повторялся");
    status_combo_->addItem("Нужно повторить ещё");
    status_combo_->addItem("Повторён");

    hint_button_ = new QPushButton("Подсказка", this);
    next_button_ = new QPushButton("Следующий случайный билет", this);
    previous_button_ = new QPushButton("Предыдущий билет", this);

    total_progress_ = new QProgressBar(this);
    green_progress_ = new QProgressBar(this);

    total_progress_->setFormat("Общий прогресс: %v / %m");
    green_progress_->setFormat("Полностью повторено: %v / %m");

    auto* form_layout = new QFormLayout();
    form_layout->setVerticalSpacing(14);
    form_layout->setHorizontalSpacing(12);
    form_layout->addRow("Номер:", number_label_);
    form_layout->addRow("Имя:", name_label_);
    form_layout->addRow("Новое имя:", name_edit_);
    form_layout->addRow("Статус:", status_combo_);

    auto* button_row_1 = new QHBoxLayout();
    button_row_1->setSpacing(10);
    button_row_1->addWidget(hint_button_);
    button_row_1->addStretch();

    auto* button_row_2 = new QHBoxLayout();
    button_row_2->setSpacing(10);
    button_row_2->addWidget(previous_button_);
    button_row_2->addWidget(next_button_);

    auto* progress_box = new QGroupBox("Прогресс", this);
    auto* progress_layout = new QVBoxLayout();
    progress_layout->setSpacing(12);
    progress_layout->addWidget(total_progress_);
    progress_layout->addWidget(green_progress_);
    progress_box->setLayout(progress_layout);
    progress_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);

    auto* right_layout = new QVBoxLayout();
    right_layout->setSpacing(16);
    right_layout->addLayout(form_layout);
    right_layout->addLayout(button_row_1);
    right_layout->addLayout(button_row_2);
    right_layout->addWidget(progress_box);
    right_layout->addStretch(1);

    question_box_->setLayout(right_layout);

    auto* left_box = new QGroupBox("Список билетов", this);
    auto* left_box_layout = new QVBoxLayout();
    left_box_layout->setSpacing(12);
    left_box_layout->addLayout(top_layout);
    left_box_layout->addWidget(view_, 1);
    left_box->setLayout(left_box_layout);
    left_box->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

    auto* main_layout = new QHBoxLayout();
    main_layout->setContentsMargins(16, 16, 16, 16);
    main_layout->setSpacing(16);
    main_layout->addWidget(left_box, 5);
    main_layout->addWidget(question_box_, 6);

    central->setLayout(main_layout);

    connect(count_spinbox_, QOverload<int>::of(&QSpinBox::valueChanged),
            this, &MainWindow::OnCountChanged);
    connect(minus_button_, &QPushButton::clicked,
            this, &MainWindow::OnMinusClicked);
    connect(plus_button_, &QPushButton::clicked,
            this, &MainWindow::OnPlusClicked);
    connect(view_, &QListWidget::itemClicked,
            this, &MainWindow::OnItemClicked);
    connect(view_, &QListWidget::itemDoubleClicked,
            this, &MainWindow::OnItemDoubleClicked);
    connect(status_combo_, QOverload<int>::of(&QComboBox::currentIndexChanged),
            this, &MainWindow::OnStatusChanged);
    connect(name_edit_, &QLineEdit::returnPressed,
            this, &MainWindow::OnNameEditReturnPressed);
    connect(hint_button_, &QPushButton::clicked,
            this, &MainWindow::OnHintClicked);
    connect(next_button_, &QPushButton::clicked,
            this, &MainWindow::OnNextQuestionClicked);
    connect(previous_button_, &QPushButton::clicked,
            this, &MainWindow::OnPreviousQuestionClicked);
    connect(save_button_, &QPushButton::clicked,
            this, &MainWindow::OnSaveClicked);
    connect(load_button_, &QPushButton::clicked,
            this, &MainWindow::OnLoadClicked);
    connect(reset_button_, &QPushButton::clicked,
            this, &MainWindow::OnResetClicked);
}

void MainWindow::RebuildTickets(int count) {
    tickets_.clear();
    tickets_.resize(count);

    for (int i = 0; i < count; ++i) {
        tickets_[i].name = DefaultTicketName(i);
        tickets_[i].hint = "";
        tickets_[i].status = DefaultStatus;
    }

    history_.clear();
    current_index_ = (count > 0 ? 0 : -1);

    UpdateView();
    UpdateProgressBars();
    UpdateQuestionView();

    if (current_index_ >= 0) {
        view_->setCurrentRow(current_index_);
    }
}

void MainWindow::ResizeTicketsPreserveProgress(int new_count) {
    int old_count = tickets_.size();
    if (new_count == old_count) {
        return;
    }

    if (new_count > old_count) {
        tickets_.resize(new_count);
        for (int i = old_count; i < new_count; ++i) {
            tickets_[i].name = DefaultTicketName(i);
            tickets_[i].hint = "";
            tickets_[i].status = DefaultStatus;
        }
    } else {
        tickets_.resize(new_count);

        QStack<int> new_history;
        QStack<int> temp;
        while (!history_.isEmpty()) {
            int idx = history_.pop();
            if (idx >= 0 && idx < new_count) {
                temp.push(idx);
            }
        }
        while (!temp.isEmpty()) {
            new_history.push(temp.pop());
        }
        history_ = new_history;

        if (current_index_ >= new_count) {
            current_index_ = new_count - 1;
        }
    }

    if (tickets_.isEmpty()) {
        current_index_ = -1;
        history_.clear();
    } else if (current_index_ < 0) {
        current_index_ = 0;
    }

    UpdateView();
    UpdateProgressBars();
    UpdateQuestionView();

    if (current_index_ >= 0) {
        view_->setCurrentRow(current_index_);
    }
}

void MainWindow::ResetToInitialState() {
    RebuildTickets(count_spinbox_->value());
}

void MainWindow::UpdateView() {
    view_->clear();

    for (int i = 0; i < tickets_.size(); ++i) {
        auto* item = new QListWidgetItem();
        item->setData(Qt::UserRole, i);
        item->setSizeHint(QSize(100, 84));
        view_->addItem(item);
        view_->setItemWidget(item, CreateTicketWidget(i));
    }

    if (current_index_ >= 0 && current_index_ < view_->count()) {
        view_->setCurrentRow(current_index_);
    }
}

void MainWindow::UpdateSingleItem(int index) {
    if (index < 0 || index >= tickets_.size()) {
        return;
    }
    if (index >= view_->count()) {
        return;
    }

    QListWidgetItem* item = view_->item(index);
    if (!item) {
        return;
    }

    item->setSizeHint(QSize(100, 84));
    view_->setItemWidget(item, CreateTicketWidget(index));
}

void MainWindow::UpdateQuestionView() {
    const bool valid = current_index_ >= 0 && current_index_ < tickets_.size();

    question_box_->setEnabled(valid);
    previous_button_->setEnabled(!history_.isEmpty());

    if (!valid) {
        number_label_->setText("-");
        name_label_->setText("-");
        name_edit_->clear();
        status_combo_->setCurrentIndex(0);
        return;
    }

    number_label_->setText(QString::number(current_index_ + 1));
    name_label_->setText(tickets_[current_index_].name);

    if (!name_edit_->hasFocus()) {
        name_edit_->setText(tickets_[current_index_].name);
    }

    status_combo_->blockSignals(true);
    status_combo_->setCurrentIndex(static_cast<int>(tickets_[current_index_].status));
    status_combo_->blockSignals(false);

    UpdateSingleItem(current_index_);
}

void MainWindow::UpdateProgressBars() {
    const int total = tickets_.size();
    int total_done = 0;
    int green_done = 0;

    for (const auto& ticket : tickets_) {
        if (ticket.status != DefaultStatus) {
            ++total_done;
        }
        if (ticket.status == GreenStatus) {
            ++green_done;
        }
    }

    total_progress_->setRange(0, qMax(1, total));
    green_progress_->setRange(0, qMax(1, total));

    total_progress_->setValue(total_done);
    green_progress_->setValue(green_done);
}

void MainWindow::SelectTicket(int index, bool push_to_history) {
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    if (push_to_history && current_index_ >= 0 && current_index_ != index) {
        history_.push(current_index_);
    }

    current_index_ = index;
    view_->setCurrentRow(index);
    UpdateSingleItem(index);
    UpdateQuestionView();
}

int MainWindow::RandomAvailableTicket() const {
    QVector<int> available;
    for (int i = 0; i < tickets_.size(); ++i) {
        if (tickets_[i].status == DefaultStatus || tickets_[i].status == YellowStatus) {
            available.push_back(i);
        }
    }

    if (available.isEmpty()) {
        return -1;
    }

    int pos = QRandomGenerator::global()->bounded(available.size());
    return available[pos];
}

void MainWindow::MarkCurrentAsYellowIfDefault() {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }

    if (tickets_[current_index_].status == DefaultStatus) {
        tickets_[current_index_].status = YellowStatus;
        UpdateSingleItem(current_index_);
        UpdateProgressBars();
        UpdateQuestionView();
    }
}

void MainWindow::OnCountChanged(int value) {
    ResizeTicketsPreserveProgress(value);
}

void MainWindow::OnItemClicked(QListWidgetItem* item) {
    if (!item) {
        return;
    }
    int index = item->data(Qt::UserRole).toInt();
    SelectTicket(index);
}

void MainWindow::OnItemDoubleClicked(QListWidgetItem* item) {
    if (!item) {
        return;
    }

    int index = item->data(Qt::UserRole).toInt();
    if (index < 0 || index >= tickets_.size()) {
        return;
    }

    if (tickets_[index].status == GreenStatus) {
        tickets_[index].status = YellowStatus;
    } else {
        tickets_[index].status = GreenStatus;
    }

    UpdateSingleItem(index);
    UpdateProgressBars();

    if (current_index_ == index) {
        UpdateQuestionView();
    }
}

void MainWindow::OnStatusChanged(int combo_index) {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }

    tickets_[current_index_].status = static_cast<TicketStatus>(combo_index);
    UpdateSingleItem(current_index_);
    UpdateProgressBars();
    UpdateQuestionView();
}

void MainWindow::OnNameEditReturnPressed() {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }

    if (!name_edit_->hasFocus()) {
        return;
    }

    QString text = name_edit_->text().trimmed();
    if (text.isEmpty()) {
        return;
    }

    tickets_[current_index_].name = text;
    UpdateSingleItem(current_index_);
    UpdateQuestionView();
}

void MainWindow::OnHintClicked() {
    if (current_index_ < 0 || current_index_ >= tickets_.size()) {
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle(QString("Подсказка к билету %1").arg(current_index_ + 1));
    dialog.resize(500, 300);

    auto* layout = new QVBoxLayout(&dialog);

    auto* edit = new QTextEdit(&dialog);
    edit->setPlainText(tickets_[current_index_].hint);
    edit->setPlaceholderText("Введите подсказку к билету...");

    auto* buttons = new QDialogButtonBox(QDialogButtonBox::Save | QDialogButtonBox::Cancel, &dialog);

    layout->addWidget(edit);
    layout->addWidget(buttons);

    connect(buttons, &QDialogButtonBox::accepted, &dialog, &QDialog::accept);
    connect(buttons, &QDialogButtonBox::rejected, &dialog, &QDialog::reject);

    if (dialog.exec() == QDialog::Accepted) {
        tickets_[current_index_].hint = edit->toPlainText();
        UpdateSingleItem(current_index_);
    }
}

void MainWindow::OnNextQuestionClicked() {
    int next_index = RandomAvailableTicket();
    if (next_index == -1) {
        return;
    }

    if (next_index == current_index_ && tickets_.size() > 1) {
        QVector<int> available;
        for (int i = 0; i < tickets_.size(); ++i) {
            if ((tickets_[i].status == DefaultStatus || tickets_[i].status == YellowStatus) &&
                i != current_index_) {
                available.push_back(i);
            }
        }
        if (!available.isEmpty()) {
            next_index = available[QRandomGenerator::global()->bounded(available.size())];
        }
    }

    MarkCurrentAsYellowIfDefault();
    SelectTicket(next_index);
}

void MainWindow::OnPreviousQuestionClicked() {
    if (history_.isEmpty()) {
        return;
    }

    MarkCurrentAsYellowIfDefault();

    int prev_index = history_.pop();
    SelectTicket(prev_index, false);
    previous_button_->setEnabled(!history_.isEmpty());
}

void MainWindow::OnSaveClicked() {
    if (SaveToFile()) {
        QMessageBox::information(this, "Сохранение", "Прогресс успешно сохранён.");
    } else {
        QMessageBox::warning(this, "Сохранение", "Не удалось сохранить прогресс.");
    }
}

void MainWindow::OnLoadClicked() {
    if (LoadFromFile()) {
        QMessageBox::information(this, "Загрузка", "Прогресс успешно загружен.");
    } else {
        QMessageBox::warning(this, "Загрузка", "Не удалось загрузить прогресс.");
    }
}

void MainWindow::OnResetClicked() {
    auto answer = QMessageBox::question(
        this,
        "Сброс",
        "Сбросить все билеты к начальному состоянию?"
    );

    if (answer != QMessageBox::Yes) {
        return;
    }

    ResetToInitialState();
}

void MainWindow::OnPlusClicked() {
    count_spinbox_->setValue(count_spinbox_->value() + 1);
}

void MainWindow::OnMinusClicked() {
    if (count_spinbox_->value() > count_spinbox_->minimum()) {
        count_spinbox_->setValue(count_spinbox_->value() - 1);
    }
}