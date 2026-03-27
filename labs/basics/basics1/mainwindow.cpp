#include "mainwindow.h"

// #include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent)
    // , ui(new Ui::MainWindow)
    , spinbox(new QSpinBox)
    , listwidget(new ListWidget(this))
    , next_question(new QPushButton("next question", this))
    , prev_question(new QPushButton("prev question", this))
    , green_progress_bar(new ProgressBarGreen(this))
    , progress_bar(new ProgressBar(this)) {
    QWidget* widget = new QWidget;
    setCentralWidget(widget);
    QBoxLayout* boxlayout = new QBoxLayout(QBoxLayout::LeftToRight);
    boxlayout->addWidget(spinbox);
    boxlayout->addWidget(next_question);
    boxlayout->addWidget(prev_question);
    QVBoxLayout* layout = new QVBoxLayout;
    layout->addLayout(boxlayout);
    green_progress_bar->setRange(0, 0);
    green_progress_bar->setValue(0);
    green_progress_bar->setStyleSheet("QProgressBar::chunk {background-color: lime;}");
    green_progress_bar->setAlignment(Qt::AlignCenter);
    progress_bar->setRange(0, 0);
    progress_bar->setValue(0);
    progress_bar->setStyleSheet("QProgressBar::chunk {background-color: yellow;}");
    progress_bar->setAlignment(Qt::AlignCenter);
    layout->addWidget(progress_bar);
    layout->addWidget(green_progress_bar);
    layout->addWidget(listwidget);
    widget->setLayout(layout);
    connect(spinbox, &QSpinBox::valueChanged, this, &MainWindow::AddElement);
    connect(next_question, &QPushButton::clicked, listwidget, &ListWidget::NextQuestion);
    connect(prev_question, &QPushButton::clicked, listwidget, &ListWidget::PrevQuestion);
    connect(spinbox, &QSpinBox::valueChanged, green_progress_bar, &QProgressBar::setMaximum);
    connect(spinbox, &QSpinBox::valueChanged, progress_bar, &QProgressBar::setMaximum);
    setWindowTitle(tr("Tickets"));
    setMinimumSize(160, 160);
    resize(960, 640);
    // ui->setupUi(this);
}

void MainWindow::AddElement() {
    listwidget->AddElement(spinbox->value());
}

MainWindow::~MainWindow() {
    // delete ui;
}

ProgressBarGreen::ProgressBarGreen(MainWindow* parent_) : parent(parent_) {
}

ProgressBar::ProgressBar(MainWindow* parent_) : parent(parent_) {
}

void ProgressBarGreen::ValueChange() {
    if (parent->listwidget->current_ticket->question_view->green) {
        if (parent->listwidget->current_ticket->question_view->status->currentText() != "lime") {
            this->setValue(this->value() - 1);
            parent->listwidget->green--;
            parent->listwidget->current_ticket->question_view->green = false;
        }
    } else {
        if (parent->listwidget->current_ticket->question_view->status->currentText() == "lime") {
            this->setValue(this->value() + 1);
            parent->listwidget->current_ticket->question_view->green = true;
            parent->listwidget->green++;
        }
    }
}

void ProgressBar::ValueChange() {
    if (parent->listwidget->current_ticket->question_view->yellow) {
        if (parent->listwidget->current_ticket->question_view->status->currentText() != "lime" &&
            (parent->listwidget->current_ticket->question_view->status->currentText() !=
             "yellow")) {
            this->setValue(this->value() - 1);
            parent->listwidget->current_ticket->question_view->yellow = false;
        }
    } else {
        if (parent->listwidget->current_ticket->question_view->status->currentText() == "lime" ||
            (parent->listwidget->current_ticket->question_view->status->currentText() ==
             "yellow")) {
            this->setValue(this->value() + 1);
            parent->listwidget->current_ticket->question_view->yellow = true;
        }
    }
}

QuestionView::QuestionView(QString title, QString number_)
    : name(new QLabel(title)), number(new QLabel(number_)), name_edit(new QLineEdit(title)) {
    status = new QComboBox;
    status->addItem("white");
    status->addItem("yellow");
    status->addItem("lime");
    QVBoxLayout* layout = new QVBoxLayout(0);
    // layout->addWidget(name);
    layout->addWidget(name_edit);
    layout->addWidget(number);
    layout->addWidget(status);
    layout->addStretch(1);
    this->setLayout(layout);
    resize(240, 180);
}

ListWidget::ListWidget(MainWindow* parent_) : parent(parent_) {
    connect(this, &ListWidget::itemClicked, this, &ListWidget::ElementClicked);
    connect(this, &ListWidget::currentItemChanged, this, &ListWidget::CurrentElementChanged);
    connect(this, &ListWidget::itemDoubleClicked, this, &ListWidget::ElementDoubleClicked);
}

Ticket::Ticket(QString title, int number) {
    QString str;
    str.setNum(number);
    question_view = new QuestionView(title, str);
}

void ListWidget::AddElement(int val) {
    this->clear();
    i = 1;
    green = 0;
    tickets.clear();
    history = std::stack<int>();
    parent->green_progress_bar->setValue(0);
    parent->progress_bar->setValue(0);
    while (i <= val) {
        QString str1;
        str1.setNum(i);
        QString str = "Question " + str1;
        // QBoxLayout* layout = new QBoxLayout();

        tickets.push_back(new Ticket(str, i));
        i++;
        current_ticket = tickets.back();
        this->addItem(str);
        connect(
            current_ticket->question_view->status, &QComboBox::currentTextChanged, this,
            &ListWidget::StatusChange);
        connect(
            current_ticket->question_view->status, &QComboBox::currentTextChanged,
            parent->green_progress_bar, &ProgressBarGreen::ValueChange);
        connect(
            current_ticket->question_view->status, &QComboBox::currentTextChanged,
            parent->progress_bar, &ProgressBar::ValueChange);
        connect(
            current_ticket->question_view->name_edit, &QLineEdit::editingFinished, this,
            &ListWidget::NameChanged);
        // this->currentItem()->setBackground(QBrush(QColor("red")));
    }
}

void ListWidget::ElementClicked() {
    history.push(row);
    current_ticket->question_view->show();
}

void ListWidget::CurrentElementChanged() {
    prev = row;
    row = this->currentRow();
    current_ticket = tickets[this->currentRow()];
}

void ListWidget::ElementDoubleClicked() {
    if (this->current_ticket->question_view->status->currentText() == "white") {
        this->current_ticket->question_view->status->setCurrentText("lime");
    } else if (this->current_ticket->question_view->status->currentText() == "lime") {
        this->current_ticket->question_view->status->setCurrentText("yellow");
    } else if (this->current_ticket->question_view->status->currentText() == "yellow") {
        this->current_ticket->question_view->status->setCurrentText("lime");
    }
    this->StatusChange();
}

void ListWidget::StatusChange() {
    this->currentItem()->setBackground(
        QBrush(QColor(this->current_ticket->question_view->status->currentText())));
}

void ListWidget::NextQuestion() {
    int j = std::rand() % (i - green - 1);
    int k = 0;
    for (auto& c : tickets) {
        if (c->question_view->green) {
            k++;
            continue;
        }
        if (j == 0) {
            break;
        }
        j--;
        k++;
    }
    auto* item = this->item(k);
    this->setCurrentItem(item);
    this->ElementClicked();
    // history.push(k);
}

void ListWidget::PrevQuestion() {
    if (history.size() > 0) {
        auto k = history.top();
        history.pop();
        // current_ticket = tickets[prev];
        auto* item = this->item(k);
        this->setCurrentItem(item);
        this->ElementClicked();
        history.pop();
    }
}

void ListWidget::NameChanged() {
    this->currentItem()->setText(this->current_ticket->question_view->name_edit->text());
}
