#include <QtWidgets>
#include <QTextEdit>

#include "custom_widgets.h"
#include "main_window.h"

#include <random>

MainWindow::MainWindow() {
    this->setMinimumSize(1280, 720);
    this->setObjectName("main_window");
    this->setWindowTitle("Procrastination");

    QWidget* central = new QWidget(this);
    central->setObjectName("central_widget");
    setCentralWidget(central);

    QHBoxLayout* main_horizontal_layout = new QHBoxLayout(central);
    main_horizontal_layout->setContentsMargins(12, 12, 12, 12);
    main_horizontal_layout->setSpacing(12);

    QFrame* left_frame = new QFrame(central);
    left_frame->setObjectName("left_frame");
    left_frame->setFrameShape(QFrame::StyledPanel);
    left_frame->setFixedWidth(350);

    QVBoxLayout* left_layout = new QVBoxLayout(left_frame);
    left_layout->setContentsMargins(12, 12, 12, 12);
    left_layout->setSpacing(10);

    QLabel* left_title = new QLabel(tr("Билеты"), left_frame);
    left_title->setObjectName("left_title");
    left_title->setFont(QFont("Segoe UI", 12, QFont::DemiBold));
    left_layout->addWidget(left_title);

    QWidget* count_container = new QWidget(left_frame);
    QHBoxLayout* count_layout = new QHBoxLayout(count_container);
    count_layout->setContentsMargins(0, 0, 0, 0);

    QLabel* count_label = new QLabel(tr("Количество билетов:"), count_container);
    count = new QSpinBox(count_container);
    count->setObjectName("count");
    count->setMinimum(1);
    count->setValue(20);
    count->setMinimumHeight(30);

    count_layout->addWidget(count_label);
    count_layout->addWidget(count, 1);
    left_layout->addWidget(count_container);

    view = new QListWidget(left_frame);
    view->setObjectName("view");
    view->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    view->setSelectionMode(QAbstractItemView::SingleSelection);
    view->setSpacing(6);
    view->setUniformItemSizes(true);
    view->setStyleSheet(
        "QListWidget#view {"
        "  background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #f5f7fb);"
        "  border-radius: 8px;"
        "  padding: 8px;"
        "  outline: none;"
        "}"
    );
    connect(view, &QListWidget::itemDoubleClicked, this, &MainWindow::changeStatusOnDoubleClick);
    left_layout->addWidget(view, 1);
    
    QLabel* left_hint = new QLabel(
        tr("Дважды кликните по билету для переключения статуса\n"
        "← → для переключения между билетами"),
        left_frame
    );
    left_hint->setWordWrap(true);
    left_hint->setStyleSheet("color: #667788; font-size: 9pt;");
    left_layout->addWidget(left_hint);

    main_horizontal_layout->addWidget(left_frame);

    QWidget* right_column = new QWidget(central);
    QVBoxLayout* right_col_layout = new QVBoxLayout(right_column);
    right_col_layout->setContentsMargins(0, 0, 0, 0);
    right_col_layout->setSpacing(12);

    QScrollArea* scroll = new QScrollArea(right_column);
    scroll->setObjectName("question_scroll");
    scroll->setWidgetResizable(true);
    scroll->setFrameShape(QFrame::NoFrame);

    QWidget* scroll_content = new QWidget(scroll);
    scroll_content->setObjectName("scroll_content");
    QVBoxLayout* scroll_layout = new QVBoxLayout(scroll_content);
    scroll_layout->setContentsMargins(0, 0, 0, 0);
    scroll_layout->setSpacing(12);
    scroll_layout->addStretch();

    scroll->setWidget(scroll_content);
    right_col_layout->addWidget(scroll, 1);

    QHBoxLayout* nav_buttons_layout = new QHBoxLayout();
    previous_question = new QPushButton(tr("Предыдущий вопрос"), right_column);
    previous_question->setObjectName("previous_question");
    previous_question->setMinimumHeight(44);
    connect(previous_question, &QPushButton::clicked, this, &MainWindow::toPreviousQuestion);

    next_question = new QPushButton(tr("Следующий вопрос"), right_column);
    next_question->setObjectName("next_question");
    next_question->setMinimumHeight(44);
    connect(next_question, &QPushButton::clicked, this, &MainWindow::toNextQuestion);

    nav_buttons_layout->addWidget(previous_question);
    nav_buttons_layout->addWidget(next_question);
    right_col_layout->addLayout(nav_buttons_layout);

    QGroupBox* progress_group = new QGroupBox(tr("Прогресс"), right_column);
    QFormLayout* progress_layout = new QFormLayout(progress_group);

    total_progress = new QProgressBar(progress_group);
    total_progress->setObjectName("total_progress");
    total_progress->setTextVisible(true);
    total_progress->setMinimumHeight(18);
    total_progress->setFormat("%v / %m");
    total_progress->setAlignment(Qt::AlignCenter);

    green_progress = new QProgressBar(progress_group);
    green_progress->setObjectName("green_progress");
    green_progress->setTextVisible(true);
    green_progress->setMinimumHeight(18);
    green_progress->setFormat("%v / %m");
    green_progress->setAlignment(Qt::AlignCenter);

    QString progress_bar_common =
        "QProgressBar {"
        "  border: 0px solid transparent;"
        "  border-radius: 9px;"
        "  background: rgba(0,0,0,0.04);"
        "  padding: 2px;"
        "  font-size: 10pt;"
        "}"
        "QProgressBar::chunk {"
        "  border-radius: 7px;"
        "}";
    total_progress->setStyleSheet(progress_bar_common +
        "QProgressBar::chunk { background: qlineargradient(x1:0 y1:0, x2:1 y2:0, stop:0 #6aa8ff, stop:1 #2e86de); }");
    green_progress->setStyleSheet(progress_bar_common +
        "QProgressBar::chunk { background: qlineargradient(x1:0 y1:0, x2:1 y2:0, stop:0 #7be495, stop:1 #2ecc71); }");

    progress_layout->addRow(tr("Общий:"), total_progress);
    progress_layout->addRow(tr("Повторено:"), green_progress);

    right_col_layout->addWidget(progress_group);

    main_horizontal_layout->addWidget(right_column, 1);

    QFont baseFont("Segoe UI", 10);
    qApp->setFont(baseFont);

    this->setStyleSheet(
        "QWidget#central_widget { background: qlineargradient(x1:0 y1:0, x2:1 y2:1, stop:0 #fbfdff, stop:1 #eef6ff); }"
        "QFrame#left_frame { background: white; border-radius: 10px; padding: 6px; }"
        "QFrame#question_container { background: transparent; }"
        "QLabel#left_title { color: #213049; }"
        "QGroupBox {"
        "  border: 1px solid rgba(34,50,78,0.06);"
        "  border-radius: 8px;"
        "  padding: 8px;"
        "}"
        "QPushButton {"
        "  border-radius: 8px;"
        "  padding: 8px 14px;"
        "  background: qlineargradient(x1:0 y1:0, x2:0 y2:1, stop:0 #ffffff, stop:1 #e6f0ff);"
        "  border: 1px solid #dbeafe;"
        "}"
        "QPushButton:hover { background: #f0f8ff; }"
    );

    createActions();
    resetAll();
}

void MainWindow::resetAll() {
    if (!question_view.empty()) {
        for (auto group_box : question_view) {
            if (group_box->isVisible()) group_box->hide();
            group_box->setParent(nullptr);
            group_box->deleteLater();
        }
        question_view.clear();
    }

    view->clear();
    white_and_yellow.clear();
    total_progress->reset();
    green_progress->reset();

    total_progress->setMaximum(count->value());
    green_progress->setMaximum(count->value());

    descriptions.resize(count->value());
    for (int i = 0; i < descriptions.size(); ++i)
        descriptions[i].clear();

    QWidget* scroll_content = findChild<QWidget*>("scroll_content");
    QVBoxLayout* scroll_layout = nullptr;
    if (scroll_content) {
        scroll_layout = qobject_cast<QVBoxLayout*>(scroll_content->layout());
        if (!scroll_layout) {
            scroll_layout = new QVBoxLayout(scroll_content);
            scroll_layout->setContentsMargins(0, 0, 0, 0);
            scroll_layout->setSpacing(12);
            scroll_layout->addStretch();
        }
    }

    for (int i = 0; i < count->value(); ++i) {
        view->addItem(tr("Билет %1").arg(i + 1));
        view->item(i)->setData(Qt::UserRole, 0);

        QGroupBox* group_box = new QGroupBox(this);
        group_box->setObjectName(QString("group_box_%1").arg(i));
        group_box->setProperty("index", i);

        group_box->hide();

        if (scroll_layout) {
            int insert_at = qMax(0, scroll_layout->count() - 1);
            scroll_layout->insertWidget(insert_at, group_box);
        } else {
            main_layout->insertWidget(main_layout->count() - 1, group_box);
        }

        QVBoxLayout* box_layout = new QVBoxLayout(group_box);
        box_layout->setContentsMargins(10, 10, 10, 10);
        box_layout->setSpacing(8);

        QWidget* header_row = new QWidget(group_box);
        QHBoxLayout* header_layout = new QHBoxLayout(header_row);
        header_layout->setContentsMargins(0, 0, 0, 0);
        header_layout->setSpacing(8);

        QLabel* number = new QLabel(header_row);
        number->setObjectName("number");
        number->setText(QString("%1").arg(i + 1));
        number->setFixedSize(28, 28);
        number->setAlignment(Qt::AlignCenter);
        number->setStyleSheet("background: #f0f7ff; border-radius: 14px; font-weight: bold;");

        header_layout->addWidget(number);

        QLabel* name = new QLabel(header_row);
        name->setObjectName("name");
        name->setText(tr("Билет %1").arg(i + 1));
        name->setFont(QFont("Segoe UI", 11, QFont::DemiBold));
        header_layout->addWidget(name, 1);

        box_layout->addWidget(header_row);

        CustomLineEdit* name_edit = new CustomLineEdit(group_box);
        name_edit->setObjectName("name_edit");
        name_edit->setPlaceholderText(tr("Изменить название билета"));
        name_edit->setMinimumHeight(30);
        name_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
        box_layout->addWidget(name_edit);
        connect(name_edit, &CustomLineEdit::hitEnter, this, &MainWindow::changeName);

        QWidget* status_row = new QWidget(group_box);
        QHBoxLayout* status_layout = new QHBoxLayout(status_row);
        status_layout->setContentsMargins(0, 0, 0, 0);
        status_layout->setSpacing(8);

        QLabel* status_label = new QLabel(tr("Статус:"), status_row);
        status_layout->addWidget(status_label);

        CustomComboBox* status = new CustomComboBox(status_row);
        status->setObjectName("status");
        status->setMinimumHeight(32);
        status->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);

        status->addItem("Не повторён");
        status->setItemData(0, QColor(Qt::white), Qt::BackgroundRole);
        status->addItem("Повторить позже");
        status->setItemData(1, QColor(Qt::yellow), Qt::BackgroundRole);
        status->addItem("Повторён");
        status->setItemData(2, QColor(Qt::green), Qt::BackgroundRole);

        {
            QColor initial = status->itemData(0, Qt::BackgroundRole).value<QColor>();
            QPalette p = status->palette();
            p.setColor(QPalette::Base, initial);
            p.setColor(QPalette::Button, initial);
            status->setPalette(p);
        }

        connect(status, &CustomComboBox::currentIndexChanged, this, [status, i] (int index) {
            QColor color = status->itemData(index, Qt::BackgroundRole).value<QColor>();

            QPalette palette = status->palette();
            palette.setColor(QPalette::Base, color);
            palette.setColor(QPalette::Button, color);
            status->setPalette(palette);

            status->indexChangedWithSender(i);
        });

        connect(status, &CustomComboBox::indexChangedWithSender, this, &MainWindow::changeStatus);

        status_layout->addWidget(status, 0, Qt::AlignLeft);
        status_layout->addStretch();

        box_layout->addWidget(status_row);

        QLabel* desc_label = new QLabel(tr("Описание:"), group_box);
        desc_label->setStyleSheet("color: #445566; font-size: 9pt;");
        box_layout->addWidget(desc_label);

        QTextEdit* description_edit = new QTextEdit(group_box);
        description_edit->setObjectName("description_edit");
        description_edit->setPlaceholderText(tr("Добавьте заметки к билету..."));
        description_edit->setMinimumHeight(80);
        description_edit->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
        box_layout->addWidget(description_edit);
        
        connect(description_edit, &QTextEdit::textChanged, this, [this, i, description_edit]() {
            changeDescription(i, description_edit->toPlainText());
        });
        
        description_edit->setPlainText(descriptions[i]);

        box_layout->addStretch();

        question_view.push_back(group_box);

        white_and_yellow.insert(i);
    }

    white_amount = count->value();
    question_view_history.clear();

    view->setCurrentRow(0);
    total_progress->setValue(0);
    green_progress->setValue(0);

    if (view->currentItem()) {
        showQuestionView(view->currentItem());
    }
}

void MainWindow::changeDescription(int index, const QString& text) {
    if (index < 0 || index >= descriptions.size())
        return;
    descriptions[index] = text;
}

void MainWindow::showQuestionView(QListWidgetItem* item) {
    if (!question_view_history.empty()) {
        question_view_history.back()->hide();
    }
    int index = view->row(item);
    if (index < 0 || index >= static_cast<int>(question_view.size())) {
        return;
    }
    if (question_view_history.empty() || question_view_history.back() != question_view[index]) {
        question_view_history.push_back(question_view[index]);
    }
    question_view_history.back()->show();

    QWidget* scroll_content = findChild<QWidget*>("scroll_content");
    if (scroll_content) {
        QWidget* w = question_view[index];
        if (w && w->parentWidget()) {
            QScrollArea* scroll = findChild<QScrollArea*>("question_scroll");
            if (scroll) {
                QWidget* cont = scroll->widget();
                if (cont) {
                    QPoint p = w->mapTo(cont, QPoint(0, 0));
                    scroll->ensureVisible(p.x() + 1, p.y() + 1, 20, 20);
                }
            }
        }
    }
}

void MainWindow::changeName(int index, QString text) {
    if (index < 0 || index >= static_cast<int>(question_view.size())) return;

    QLabel* name = question_view[index]->findChild<QLabel*>("name");
    if (name) {
        if (text.isEmpty()) {
            name->setText(tr("Билет %1").arg(index + 1));
            view->item(index)->setText(tr("Билет %1").arg(index + 1));
        } else {
            name->setText(text);
            view->item(index)->setText(text);
        }
    }
}

void MainWindow::changeStatus(int box_index) {
    CustomComboBox* status = question_view[box_index]->findChild<CustomComboBox*>("status");
    if (!status) return;

    int old_status = view->item(box_index)->data(Qt::UserRole).toInt();
    int new_status = status->currentIndex();

    if (old_status == 0) --white_amount;
    if (new_status == 0) ++white_amount;

    if (new_status == 2)
        white_and_yellow.erase(box_index);
    else
        white_and_yellow.insert(box_index);

    QColor color = status->itemData(new_status, Qt::BackgroundRole).value<QColor>();

    view->item(box_index)->setBackground(QBrush(color));
    view->item(box_index)->setData(Qt::UserRole, new_status);

    int green_amount = question_view.size() - white_and_yellow.size();
    int yellow_and_green_amount = question_view.size() - white_amount;

    total_progress->setValue(yellow_and_green_amount);
    green_progress->setValue(green_amount);
}

void MainWindow::changeStatusOnDoubleClick(QListWidgetItem* item) {
    int index = view->row(item);
    if (index < 0 || index >= static_cast<int>(question_view.size())) return;

    CustomComboBox* status = question_view[index]->findChild<CustomComboBox*>("status");
    if (!status) return;

    int current_index = status->currentIndex();
    if (current_index == 2) {
        status->setCurrentIndex(1);
    } else {
        status->setCurrentIndex(2);
    }
}

void MainWindow::toPreviousQuestion() {
    if (question_view_history.size() <= 1) {
        return;
    }
    question_view_history.back()->hide();
    question_view_history.pop_back();
    question_view_history.back()->show();

    view->setCurrentRow(question_view_history.back()->property("index").toInt());

    showQuestionView(view->currentItem());
}

void MainWindow::toNextQuestion() {
    if (white_and_yellow.empty()) {
        return;
    }

    int current_index = -1;

    if (!question_view_history.empty()) {
        question_view_history.back()->hide();
        current_index = question_view_history.back()->property("index").toInt();
    }

    std::vector<int> candidates;
    candidates.reserve(white_and_yellow.size());

    for (int i : white_and_yellow) {
        if (i != current_index) {
            candidates.push_back(i);
        }
    }

    if (candidates.empty()) {
        if (!question_view_history.empty()) {
            question_view_history.back()->show();
        }
        return;
    }

    static std::mt19937 gen(std::random_device{}());
    std::uniform_int_distribution<> dist(0, candidates.size() - 1);

    int next_index = candidates[dist(gen)];

    view->setCurrentRow(next_index);

    QGroupBox* next_view = question_view[next_index];

    question_view_history.push_back(next_view);
    next_view->show();

    showQuestionView(view->currentItem());
}

void MainWindow::createActions() {
    connect(count, &QSpinBox::valueChanged, this, &MainWindow::resetAll);

    connect(view, &QListWidget::itemClicked, this, &MainWindow::showQuestionView);

    QShortcut* prevShortcut = new QShortcut(QKeySequence(Qt::Key_Left), this);
    connect(prevShortcut, &QShortcut::activated, this, &MainWindow::toPreviousQuestion);

    QShortcut* nextShortcut = new QShortcut(QKeySequence(Qt::Key_Right), this);
    connect(nextShortcut, &QShortcut::activated, this, &MainWindow::toNextQuestion);
}
