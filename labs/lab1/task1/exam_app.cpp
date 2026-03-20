#include "exam_app.h"

#include <QFile>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QKeySequence>
#include <QMessageBox>
#include <QPainter>
#include <QRandomGenerator>
#include <QShortcut>
#include <QVBoxLayout>

void TicketDelegate::paint(
    QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const {
    painter->save();
    painter->setRenderHint(QPainter::Antialiasing);
    QRect rect = option.rect;
    int status = index.data(Qt::UserRole).toInt();

    QColor bgColor = QColor(245, 245, 245);
    if (status == 1) {
        bgColor = QColor(255, 255, 180);
    } else if (status == 2) {
        bgColor = QColor(190, 255, 190);
    }

    painter->fillRect(rect, bgColor);

    if (option.state & QStyle::State_Selected) {
        painter->setPen(QPen(QColor(0, 100, 255), 3));
        painter->drawRect(rect.adjusted(1, 1, -1, -1));
    }

    painter->setPen(Qt::black);
    painter->drawText(rect, Qt::AlignCenter, index.data(Qt::DisplayRole).toString());
    painter->restore();
}

QSize TicketDelegate::sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const {
    return QSize(option.rect.width(), 45);
}

ExamApp::ExamApp(QWidget* parent) : QWidget(parent) {
    setupUi();
    setupShortcuts();
    loadData();
    if (tickets.empty()) {
        applyNewTicketCount();
    }
    this->setFocus();
}

ExamApp::~ExamApp() {
    saveData();
}

void ExamApp::setupShortcuts() {
    new QShortcut(QKeySequence(Qt::Key_Right), this, SLOT(nextRandomQuestion()));
    new QShortcut(QKeySequence(Qt::Key_Left), this, SLOT(previousQuestion()));
    new QShortcut(QKeySequence(Qt::Key_R), this, SLOT(resetProgress()));
    new QShortcut(QKeySequence(Qt::Key_H), this, SLOT(showHelp()));

    new QShortcut(QKeySequence(Qt::Key_1), this, [this]() {
        if (!nameEdit->hasFocus() && !countSpinBox->hasFocus()) {
            onStatusChanged(0);
        }
    });
    new QShortcut(QKeySequence(Qt::Key_2), this, [this]() {
        if (!nameEdit->hasFocus() && !countSpinBox->hasFocus()) {
            onStatusChanged(1);
        }
    });
    new QShortcut(QKeySequence(Qt::Key_3), this, [this]() {
        if (!nameEdit->hasFocus() && !countSpinBox->hasFocus()) {
            onStatusChanged(2);
        }
    });
}

void ExamApp::setupUi() {
    auto* mainLayout = new QVBoxLayout(this);

    auto* topLayout = new QHBoxLayout();
    topLayout->addWidget(new QLabel("Билетов (Enter):"));
    countSpinBox = new QSpinBox();
    countSpinBox->setRange(1, 500);
    countSpinBox->setValue(10);
    countSpinBox->setKeyboardTracking(false);
    topLayout->addWidget(countSpinBox);
    topLayout->addStretch();
    mainLayout->addLayout(topLayout);

    auto* contentLayout = new QHBoxLayout();
    ticketList = new QListWidget();
    ticketList->setItemDelegate(new TicketDelegate());
    ticketList->setFocusPolicy(Qt::NoFocus);
    contentLayout->addWidget(ticketList, 1);

    auto* detailsGroup = new QVBoxLayout();
    ticketNumLabel = new QLabel("-");
    ticketNameLabel = new QLabel("Выберите билет");
    nameEdit = new QLineEdit();
    nameEdit->setPlaceholderText("Имя + Enter...");
    statusCombo = new QComboBox();
    statusCombo->addItems({"Не начато", "Повторить", "Готово"});

    auto* formLayout = new QFormLayout();
    formLayout->addRow("№:", ticketNumLabel);
    formLayout->addRow("Имя:", ticketNameLabel);
    formLayout->addRow("Правка:", nameEdit);
    formLayout->addRow("Статус:", statusCombo);
    detailsGroup->addLayout(formLayout);

    btnPrev = new QPushButton("Назад (left)");
    btnNext = new QPushButton("Рандом (right)");
    btnReset = new QPushButton("Сброс прогресса (R)");
    btnHelp = new QPushButton("Помощь (H)");

    btnReset->setStyleSheet("color: #d32f2f; font-weight: bold; padding: 5px;");
    btnHelp->setStyleSheet("background-color: #f0f0f0; border: 1px solid #ccc; padding: 5px;");

    detailsGroup->addWidget(btnPrev);
    detailsGroup->addWidget(btnNext);
    detailsGroup->addWidget(btnReset);

    detailsGroup->addStretch();
    detailsGroup->addWidget(btnHelp);

    contentLayout->addLayout(detailsGroup, 1);
    mainLayout->addLayout(contentLayout);

    totalProgress = new QProgressBar();
    greenProgress = new QProgressBar();
    mainLayout->addWidget(new QLabel("Общий прогресс:"));
    mainLayout->addWidget(totalProgress);
    mainLayout->addWidget(new QLabel("Выучено:"));
    mainLayout->addWidget(greenProgress);

    connect(
        countSpinBox->findChild<QLineEdit*>(), &QLineEdit::returnPressed, this,
        &ExamApp::applyNewTicketCount);
    connect(nameEdit, &QLineEdit::returnPressed, this, &ExamApp::updateTicketName);
    connect(ticketList, &QListWidget::itemClicked, this, &ExamApp::onTicketClicked);
    connect(
        statusCombo, QOverload<int>::of(&QComboBox::currentIndexChanged), this,
        &ExamApp::onStatusChanged);
    connect(btnNext, &QPushButton::clicked, this, &ExamApp::nextRandomQuestion);
    connect(btnPrev, &QPushButton::clicked, this, &ExamApp::previousQuestion);
    connect(btnReset, &QPushButton::clicked, this, &ExamApp::resetProgress);
    connect(btnHelp, &QPushButton::clicked, this, &ExamApp::showHelp);
}

void ExamApp::resetProgress() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(
        this, "Сброс", "Обнулить прогресс всех билетов?", QMessageBox::Yes | QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        for (auto& t : tickets) {
            t.status = TicketStatus::Default;
        }
        for (int i = 0; i < ticketList->count(); ++i) {
            ticketList->item(i)->setData(Qt::UserRole, (int)TicketStatus::Default);
        }
        updateProgress();
        saveData();
        if (currentTicketIndex != -1) {
            refreshTicketView(currentTicketIndex);
        }
    }
    this->setFocus();
}

void ExamApp::showHelp() {
    QString helpText =
        "<b>Горячие клавиши:</b><br>"
        "<b>→ (Right)</b>: Следующий случайный билет (не готовый)<br>"
        "<b>← (Left)</b>: Вернуться к предыдущему билету<br>"
        "<b>1, 2, 3</b>: Установить статус (Не начато, Повторить, Готово)<br>"
        "<b>R</b>: Сбросить прогресс всех билетов<br>"
        "<b>H</b>: Показать это окно помощи<br><br>"
        "<b>Поля ввода:</b><br>"
        "Введите текст и нажмите <b>Enter</b>, чтобы сохранить изменения.<br><br>"
        "Прогресс сохраняется при перезапуске приложения.";
    QMessageBox::information(this, "Справка по управлению", helpText);
    this->setFocus();
}

void ExamApp::applyNewTicketCount() {
    int count = countSpinBox->value();
    if (count == (int)tickets.size()) {
        return;
    }
    tickets.clear();
    ticketList->clear();
    for (int i = 0; i < count; ++i) {
        Ticket t;
        t.number = i + 1;
        t.name = QString("Билет %1").arg(t.number);
        tickets.push_back(t);
        auto* item = new QListWidgetItem(t.name);
        item->setData(Qt::UserRole, (int)t.status);
        ticketList->addItem(item);
    }
    saveData();
    updateProgress();
    countSpinBox->clearFocus();
    this->setFocus();
}

void ExamApp::updateTicketName() {
    if (currentTicketIndex == -1) {
        return;
    }
    QString txt = nameEdit->text().trimmed();
    if (!txt.isEmpty()) {
        tickets[currentTicketIndex].name = txt;
        ticketNameLabel->setText(txt);
        ticketList->item(currentTicketIndex)->setText(txt);
        saveData();
    }
    nameEdit->clear();
    nameEdit->clearFocus();
    this->setFocus();
}

void ExamApp::onTicketClicked(QListWidgetItem* item) {
    refreshTicketView(ticketList->row(item));
}

void ExamApp::refreshTicketView(int index) {
    if (index < 0 || index >= (int)tickets.size()) {
        return;
    }
    currentTicketIndex = index;
    Ticket& t = tickets[index];
    ticketNumLabel->setText(QString::number(t.number));
    ticketNameLabel->setText(t.name);
    statusCombo->blockSignals(true);
    statusCombo->setCurrentIndex((int)t.status);
    statusCombo->blockSignals(false);
    ticketList->setCurrentRow(index);
    ticketList->scrollToItem(ticketList->currentItem());
}

void ExamApp::onStatusChanged(int index) {
    if (currentTicketIndex == -1) {
        return;
    }
    tickets[currentTicketIndex].status = static_cast<TicketStatus>(index);
    ticketList->item(currentTicketIndex)->setData(Qt::UserRole, index);
    statusCombo->blockSignals(true);
    statusCombo->setCurrentIndex(index);
    statusCombo->blockSignals(false);
    updateProgress();
    saveData();
}

void ExamApp::nextRandomQuestion() {
    std::vector<int> avail;
    for (int i = 0; i < (int)tickets.size(); ++i) {
        if (tickets[i].status != TicketStatus::Green) {
            avail.push_back(i);
        }
    }

    if (avail.empty()) {
        QMessageBox::information(this, "Готово!", "Все билеты выучены!");
        return;
    }
    if (currentTicketIndex != -1) {
        history.push(currentTicketIndex);
    }
    int r = avail[QRandomGenerator::global()->bounded((int)avail.size())];
    refreshTicketView(r);
}

void ExamApp::previousQuestion() {
    if (!history.isEmpty()) {
        refreshTicketView(history.pop());
    }
}

void ExamApp::saveData() {
    QJsonArray arr;
    for (const auto& t : tickets) {
        QJsonObject o;
        o["n"] = t.number;
        o["t"] = t.name;
        o["s"] = (int)t.status;
        arr.append(o);
    }
    QFile f("save.json");
    if (f.open(QIODevice::WriteOnly)) {
        f.write(QJsonDocument(arr).toJson());
    }
}

void ExamApp::loadData() {
    QFile f("save.json");
    if (!f.open(QIODevice::ReadOnly)) {
        return;
    }
    QJsonArray arr = QJsonDocument::fromJson(f.readAll()).array();
    tickets.clear();
    ticketList->clear();
    for (auto v : arr) {
        QJsonObject o = v.toObject();
        Ticket t;
        t.number = o["n"].toInt();
        t.name = o["t"].toString();
        t.status = (TicketStatus)o["s"].toInt();
        tickets.push_back(t);
        auto* item = new QListWidgetItem(t.name);
        item->setData(Qt::UserRole, (int)t.status);
        ticketList->addItem(item);
    }
    countSpinBox->setValue(tickets.size());
    updateProgress();
}

void ExamApp::updateProgress() {
    int tot = tickets.size(), yg = 0, g = 0;
    for (const auto& t : tickets) {
        if (t.status == TicketStatus::Green) {
            g++;
            yg++;
        } else if (t.status == TicketStatus::Yellow) {
            yg++;
        }
    }
    totalProgress->setRange(0, tot);
    totalProgress->setValue(yg);
    greenProgress->setRange(0, tot);
    greenProgress->setValue(g);
}