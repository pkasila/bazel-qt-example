#include "mainwindow.h"

#include "data.h"

#include <QItemDelegate>
#include <QLabel>
#include <QPainter>
#include <QRandomGenerator>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QtCore/QVariant>
#include <QtGui/QIcon>
#include <QtWidgets/QAbstractButton>
#include <QtWidgets/QApplication>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QTextEdit>
#include <QtWidgets/QVBoxLayout>

#include <algorithm>
#include <cassert>
#include <ranges>
#include <utility>

QIcon source_icon(const QString &str)
{
    const auto *theme = "light";
    const QPalette defaultPalette;
    const auto text = defaultPalette.color(QPalette::WindowText);
    const auto window = defaultPalette.color(QPalette::Window);
    if (text.lightness() > window.lightness()) {
        theme = "dark";
    }
    return QIcon(QString(":/resources/%1/%2.svg").arg(theme, str));
}

void Flag::rotate_backward()
{
    state = (state + 2) % 3;
}

void Flag::rotate()
{
    state = (state + 1) % 3;
}

QIcon Flag::icon() const
{
    switch (state) {
    case Flag::Default: {
        auto pixmap = QPixmap(128, 128);
        auto color = QColor();
        color.setAlpha(0);
        pixmap.fill(color);
        return { pixmap };
    }
    case Flag::Green:
        return source_icon("flag-green");
    case Flag::Yellow:
        return source_icon("flag-yellow");
    default:
        assert(false);
    }
}

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent)
{
    clickItemId = -1;
    this->setObjectName("Questionnaire 99");
    this->resize(800, 600);
    this->setMouseTracking(false);

    auto *centralwidget = new QWidget(this);
    centralwidget->setObjectName("centralwidget");
    auto *verticalLayout = new QVBoxLayout(centralwidget);
    verticalLayout->setObjectName("verticalLayout");
    listView = new QListWidget(centralwidget);
    listView->setObjectName("listView");

    for (auto i : std::ranges::views::iota(0UL, kQuestions.size())) {
        add_question(static_cast<int>(i), kQuestions.at(i));
    }
    shuffle_remaining();

    // listView->setSortingEnabled(true);

    auto *horizontalLayout_2 = new QHBoxLayout();
    horizontalLayout_2->setObjectName("horizontalLayout_2");

    auto *label = new QLabel(centralwidget);
    label->setText("Current");

    horizontalLayout_2->addWidget(label);

    lineEdit = new QLineEdit(centralwidget);
    lineEdit->setObjectName("lineEdit");
    lineEdit->setPlaceholderText("Click next...");
    lineEdit->setReadOnly(true);

    horizontalLayout_2->addWidget(lineEdit);

    auto *horizontalSpacer_2 =
            new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    horizontalLayout_2->addItem(horizontalSpacer_2);

    auto *shuffleButton = new QPushButton(centralwidget);
    shuffleButton->setObjectName("shuffleButton");
    horizontalLayout_2->addWidget(shuffleButton);

    progressBar = new QProgressBar(centralwidget);
    progressBar->setObjectName("progressBar");
    progressBar->setEnabled(true);
    progressBar->setMouseTracking(false);
    progressBar->setFocusPolicy(Qt::FocusPolicy::NoFocus);
    progressBar->setAutoFillBackground(true);
    progressBar->setValue(0);
    progressBar->setMinimum(0);
    progressBar->setMaximum(listView->count());
    progressBar->setTextVisible(true);
    progressBar->setOrientation(Qt::Orientation::Horizontal);
    progressBar->setInvertedAppearance(false);
    progressBar->setTextDirection(QProgressBar::Direction::TopToBottom);

    horizontalLayout_2->addWidget(progressBar);

    spinBox = new QSpinBox(centralwidget);
    spinBox->setObjectName("spinBox");
    spinBox->setMinimum(0);
    spinBox->setMaximum(99);
    spinBox->setValue(listView->count());

    horizontalLayout_2->addWidget(spinBox);
    verticalLayout->addLayout(horizontalLayout_2);

    verticalLayout->addWidget(listView);

    auto *horizontalLayout = new QHBoxLayout();
    auto *horizontalSpacer =
            new QSpacerItem(40, 20, QSizePolicy::MinimumExpanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(horizontalSpacer);

    horizontalLayout = new QHBoxLayout();
    horizontalLayout->setObjectName("horizontalLayout");
    auto *previousButton = new QPushButton(centralwidget);
    previousButton->setObjectName("previousButton");

    horizontalLayout->addWidget(previousButton);

    auto *nextButton = new QPushButton(centralwidget);
    nextButton->setObjectName("nextButton");

    horizontalLayout->addWidget(nextButton);

    verticalLayout->addLayout(horizontalLayout);

    this->setCentralWidget(centralwidget);

    this->setWindowTitle("Questionnaire 99");
    previousButton->setText("Previous Question");
    nextButton->setText("Next Question");
    shuffleButton->setText("Shuffle Remaining");
    progressBar->setFormat("%v/%m");

    QMetaObject::connectSlotsByName(this);
}

void MainWindow::shuffle_remaining()
{
    if (remaining.size() > 1) {
        for (auto i : std::views::iota(0, remaining.size() - 1)) {
            auto num = QRandomGenerator::global()->bounded(i, remaining.size() - 1);
            std::swap(remaining[i], remaining[num]);
        }
    }
}

void MainWindow::on_spinBox_valueChanged(int val)
{
    if (val < questions.size()) {
        for (auto i : std::ranges::iota_view(val, static_cast<int>(questions.size()))
                     | std::views::reverse) {
            listView->model()->removeRow(i);
            questions.remove(i);
            issued.removeIf([&](auto &x) { return x == i; });
            remaining.removeIf([&](auto &x) { return x == i; });
        }
    } else {
        for (auto i : std::ranges::iota_view(static_cast<int>(questions.size()), val)) {
            add_question(i);
        }
    }
    clickItemId = -1;
    update_progress_bar();
}
void MainWindow::add_question(int id, QString desc)
{
    questions.push_back(Question{
            .name = QString("Question %1").arg(id + 1),
            .desc = std::move(desc),
            .flag = { Flag::Default },
    });
    auto &question = questions.back();
    listView->addItem(question.name);
    listView->item(id)->setIcon(question.flag.icon());
    remaining.push_back(id);
}

class Dialog : public QDialog
{
public:
    QVBoxLayout *verticalLayout;
    QHBoxLayout *horizontalLayout;
    QPushButton *pushButton;
    QLineEdit *lineEdit;
    QTextEdit *textEdit;
    QDialogButtonBox *buttonBox;

    Flag flag;
    MainWindow *parent;
    int item_id;

    explicit Dialog(MainWindow *parent, int item_id) : QDialog(parent)
    {
        this->parent = parent;
        this->item_id = item_id;

        auto &question = parent->questions[item_id];
        flag = question.flag;

        this->setObjectName("Dialog");
        this->resize(400, 300);
        verticalLayout = new QVBoxLayout(this);
        verticalLayout->setObjectName("verticalLayout");
        horizontalLayout = new QHBoxLayout();
        horizontalLayout->setObjectName("horizontalLayout");
        pushButton = new QPushButton(this);
        pushButton->setObjectName("pushButton");
        QSizePolicy sizePolicy(QSizePolicy::Minimum, QSizePolicy::Minimum);
        sizePolicy.setHorizontalStretch(0);
        sizePolicy.setVerticalStretch(0);
        pushButton->setSizePolicy(sizePolicy);
        pushButton->setIcon(question.flag.icon());
        pushButton->setAutoDefault(true);
        pushButton->setFlat(true);

        horizontalLayout->addWidget(pushButton);

        lineEdit = new QLineEdit(this);
        lineEdit->setObjectName("lineEdit");

        horizontalLayout->addWidget(lineEdit);

        verticalLayout->addLayout(horizontalLayout);

        textEdit = new QTextEdit(this);
        textEdit->setObjectName("textEdit");

        verticalLayout->addWidget(textEdit);

        buttonBox = new QDialogButtonBox(this);
        buttonBox->setObjectName("buttonBox");
        buttonBox->setOrientation(Qt::Orientation::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::StandardButton::Cancel
                                      | QDialogButtonBox::StandardButton::Ok);

        verticalLayout->addWidget(buttonBox);

        this->setWindowTitle("Dialog");
        pushButton->setText(QString());
        lineEdit->setText(question.name);
        textEdit->setText(question.desc);
        textEdit->setPlaceholderText("Question description...");

        QObject::connect(buttonBox, &QDialogButtonBox::accepted, this,
                         qOverload<>(&QDialog::accept));
        QObject::connect(buttonBox, &QDialogButtonBox::rejected, this,
                         qOverload<>(&QDialog::reject));

        pushButton->setDefault(false);

        QObject::connect(pushButton, &QPushButton::clicked, this,
                         qOverload<>(&Dialog::on_pushButton_clicked));

        QMetaObject::connectSlotsByName(this);
    }

private slots:
    void on_pushButton_clicked()
    {
        flag.rotate();
        pushButton->setIcon(flag.icon());
    }
};

void MainWindow::on_previousButton_clicked()
{
    if (issued.size() == 0) {
        lineEdit->setText("");
        lastShownItemId.reset();
        return;
    }
    auto item_id = issued.back();
    if (issued.size() > 1 && lastShownItemId == item_id) {
        issued.pop_back();
        remaining.push_back(item_id);
        item_id = issued.back();
    }
    auto selected = listView->selectedItems();
    issued.pop_back();
    remaining.push_back(item_id);

    auto *item = listView->item(item_id);
    item->setSelected(true);
    listView->scrollToItem(item);
    lineEdit->setText(questions[item_id].name);
    lastShownItemId = item_id;
}

void MainWindow::on_shuffleButton_clicked()
{
    shuffle_remaining();
}

void MainWindow::on_nextButton_clicked()
{
    if (remaining.size() == 0) {
        return;
    }
    auto item_id = remaining.back();
    while (remaining.size() > 1
           && (lastShownItemId == item_id || questions[item_id].flag.state == Flag::Green)) {
        remaining.pop_back();
        issued.push_back(item_id);
        item_id = remaining.back();
    }
    if (remaining.size() > 1) {
        item_id = remaining.back();
        remaining.pop_back();
        issued.push_back(item_id);
    }

    auto *item = listView->item(item_id);
    item->setSelected(true);
    listView->scrollToItem(item);
    lineEdit->setText(questions[item_id].name);
    lastShownItemId = item_id;
}

void MainWindow::on_listView_itemDoubleClicked()
{
    auto item_id = listView->currentIndex().row();
    auto *item = listView->item(item_id);
    auto &q = questions[item_id];

    if (item_id == clickItemId) {
        auto &flag = q.flag;
        flag.rotate_backward();
        item->setIcon(flag.icon());
        update_progress_bar();
    }
    clickItemId = -1;

    Dialog dialog(this, item_id);
    if (dialog.exec() == QDialog::Accepted) {
        q.name = dialog.lineEdit->text();
        q.desc = dialog.textEdit->toPlainText();
        q.flag = dialog.flag;

        item->setText(q.name);
        item->setIcon(q.flag.icon());
        update_progress_bar();

        if (lastShownItemId.has_value() && lastShownItemId.value() == item_id) {
            lineEdit->setText(q.name);
        }
    }
}

void MainWindow::on_listView_itemClicked()
{
    auto item_id = listView->currentIndex().row();
    auto *item = listView->currentItem();

    auto &flag = questions[item_id].flag;
    flag.rotate();
    item->setIcon(flag.icon());
    update_progress_bar();

    clickItemId = item_id;
}

void MainWindow::update_progress_bar()
{
    int sum = 0;
    for (auto &i : questions) {
        sum += i.flag.state == Flag::Green ? 1 : 0;
    }
    progressBar->setValue(sum);
    progressBar->setMaximum(static_cast<int>(questions.size()));
}
