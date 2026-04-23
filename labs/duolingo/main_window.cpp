#include "main_window.h"
#include "exercise_widget.h"
#include <QMainWindow>
#include <QStackedWidget>
#include <QProgressBar>
#include <QLabel>
#include <QPushButton>
#include <QTimer>
#include <QVBoxLayout>
#include <QMenuBar>
#include <QInputDialog>
#include <QMessageBox>
#include <QSoundEffect>
#include <qobjectdefs.h>
#include <qtmetamacros.h>
#include <QUrl>
#include <QtSql/QSqlDatabase>
#include <QtSql/QSqlQuery>
#include <QTime>

#include "translation_exercise.h"
#include "grammar_exercise.h"


MainWindow::MainWindow() {
    SetupMenu();
    SetupUI();
    ApplyStyles();
    OpenDB();
    UpdateEXP();
}

void MainWindow::SetupMenu() {
    QMenu *settings_menu = menuBar()->addMenu("Settings");
    QAction *difficulty_action = settings_menu->addAction("Select Difficulty");
    difficulty_action->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_D));

    QMenu *helpMenu = menuBar()->addMenu(tr("&Help"));
    QAction *hintAction = helpMenu->addAction("Show &Hint");
    hintAction->setShortcut(QKeySequence(Qt::Key_H));

    connect(hintAction, &QAction::triggered, this, &MainWindow::CallHint);
    connect(difficulty_action, &QAction::triggered, this, &MainWindow::OnSelectDifficulty);
}

MainWindow::~MainWindow() {
    SaveUserData();
}

void MainWindow::SetupUI() {
    central_widget_ = new QWidget(this);
    setCentralWidget(central_widget_);

    level_circle_ = new QLabel("Level: 1", this);
    level_circle_->setAlignment(Qt::AlignCenter);
    level_circle_->setObjectName("levelCircle");
    level_circle_->setFixedSize(160, 160);

    learn_german_label_ = new QLabel("LEARN GERMAN", this);
    learn_german_label_->setAlignment(Qt::AlignCenter);
    learn_german_label_->setObjectName("learnGermanLabel");

    exp_bar_ = new QProgressBar(this);
    exp_bar_->setRange(0, 100);
    exp_bar_->setValue(30);
    exp_bar_->setTextVisible(true);
    exp_bar_->setFixedHeight(14);
    exp_bar_->setObjectName("expBar");

    translation_button_ = new QPushButton("Translation", this);
    grammar_button_ = new QPushButton("Grammar", this);

    stacked_widget_ = new QStackedWidget(this);

    QHBoxLayout *btn_layout = new QHBoxLayout;
    btn_layout->addWidget(translation_button_);
    btn_layout->addWidget(grammar_button_);
    btn_layout->setAlignment(Qt::AlignHCenter | Qt::AlignBottom);

    QVBoxLayout *top_layout = new QVBoxLayout;
    top_layout->addWidget(level_circle_, 0, Qt::AlignHCenter);
    top_layout->addWidget(exp_bar_, 0, Qt::AlignHCenter);
    top_layout->setSpacing(10);

    main_layout_ = new QVBoxLayout;
    main_layout_->setContentsMargins(20, 20, 20, 20);
    main_layout_->setSpacing(20);

    main_layout_->addLayout(top_layout);
    main_layout_->addStretch(1);
    main_layout_->addWidget(learn_german_label_, 0, Qt::AlignCenter);
    main_layout_->addStretch(1);
    main_layout_->addLayout(btn_layout);
    main_layout_->addWidget(stacked_widget_, 0);

    central_widget_->setLayout(main_layout_);

    complete_effect_.setSource(QUrl::fromLocalFile("labs/duolingo/data/complete.wav"));

    connect(translation_button_, &QPushButton::clicked, this, &MainWindow::OnTranslation);
    connect(grammar_button_, &QPushButton::clicked, this, &MainWindow::OnGrammar);


    setMinimumSize(800, 600);
}

void MainWindow::ApplyStyles() const {
    const QString bg_style = R"(
        QWidget#centralWidget {
            background: qlineargradient(x1:0, y1:0, x2:1, y2:1,
                stop:0 #7f00ff, stop:1 #e100ff);
        }
    )";

    const QString circle_style = R"(
        QLabel#levelCircle {
            background-color: #d18cff;
            color: white;
            border-radius: 80px;
            font: bold 18px;
        }
    )";

    const QString bar_style = R"(
        QProgressBar#expBar {
            background: #eeeeee;
            border-radius: 7px;
            color: black;
        }
       QProgressBar#expBar::chunk {
            background-color: #ff66cc;
            border-radius: 7px;
        }
    )";

    const QString label_style = R"(
        QLabel#learnGermanLabel {
            color: white;
            text-transform: uppercase;
        }
    )";

    translation_button_->setStyleSheet(
        "QPushButton { "
        "  background-color: #cc66ff; "
        "  color: white; "
        "  padding: 8px 1px; "
        "  border-radius: 8px; "
        "} "
        "QPushButton:hover { "
        "  background-color: #dd88ff; "
        "} "
        "QPushButton:pressed { "
        "  background-color: #bb55dd; "
        "}"
    );

    grammar_button_->setStyleSheet(
        "QPushButton { "
        "  background-color: #ff99cc; "
        "  color: white; "
        "  padding: 8px 1px; "
        "  border-radius: 8px; "
        "} "
        "QPushButton:hover { "
        "  background-color: #ffaadf; "
        "} "
        "QPushButton:pressed { "
        "  background-color: #ee88b8; "
        "}"
    );

    central_widget_->setObjectName("centralWidget");
    central_widget_->setStyleSheet(bg_style + circle_style + bar_style + label_style);
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);

    constexpr int kBaseWidth = 800;
    constexpr int kBaseHeight = 600;

    const qreal w_ratio = static_cast<qreal>(width()) / kBaseWidth;
    const qreal h_ratio = static_cast<qreal>(height()) / kBaseHeight;
    emit ResizeEvent(w_ratio, h_ratio);

    const int font_size = qMax(24, static_cast<int>(48 * h_ratio));
    QFont label_font = learn_german_label_->font();
    label_font.setPointSize(font_size);
    learn_german_label_->setFont(label_font);

    const int circle_size = qMin(static_cast<int>(160 * w_ratio), static_cast<int>(h_ratio * 160));
    level_circle_->setFixedSize(circle_size, circle_size);

    const int radius = circle_size / 2;
    const QString circle_style = QString(R"(
        QLabel#levelCircle {
            background-color: #d18cff;
            color: white;
            border-radius: %1px;
            font: bold %2px;
        }
    )").arg(radius).arg(qMax(12, static_cast<int>(18 * h_ratio)));

    level_circle_->setStyleSheet(circle_style);
    exp_bar_->setFixedSize(150 * w_ratio, 14 * h_ratio);
    exp_bar_->setAlignment(Qt::AlignCenter);
    QFont font = exp_bar_->font();
    font.setPointSize(12 * h_ratio);
    exp_bar_->setFont(font);

    const int btn_height = qMax(30, static_cast<int>(40 * h_ratio));
    const int btn_width = qMax(100, static_cast<int>(160 * w_ratio));
    translation_button_->setFixedSize(btn_width, btn_height);
    grammar_button_->setFixedSize(btn_width, btn_height);

    const int btn_font_size = qMax(10, static_cast<int>(14 * h_ratio));
    QFont btn_font;
    btn_font.setPointSize(btn_font_size);
    translation_button_->setFont(btn_font);
    grammar_button_->setFont(btn_font);
}

void MainWindow::OnSelectDifficulty() {
    bool ok = false;
    const QStringList levels = {"Easy", "Medium", "Hard"};
    const QString level = QInputDialog::getItem(this, "Select Difficulty", "Difficulty:", levels, difficult_level_,
                                                false,
                                                &ok);
    if (ok && !level.isEmpty()) {
        QMessageBox::information(this, "Difficulty Selected", "You chose: " + level);
        difficult_level_ = levels.indexOf(level);
    }
}

void MainWindow::UpdateEXP() {
    if (exp_ >= necessary_exp_) {
        level_++;
        exp_ -= necessary_exp_;
        necessary_exp_ += 10;
    }
    exp_bar_->setRange(0, necessary_exp_);
    level_circle_->setText("Level: " + QString::number(level_));
    exp_bar_->setFormat(QString("%1/%2 XP").arg(exp_).arg(necessary_exp_));
    exp_bar_->setValue(exp_);
}

void MainWindow::OnTranslation() {
    if (stacked_widget_->count() != 0) {
        return;
    }

    translation_button_->hide();
    grammar_button_->hide();

    not_in_task_ = false;

    for (int i = 0; i < 5; ++i) {
        TranslationExercise *translation_widget = new TranslationExercise(
            difficult_level_, db_);
        connect(translation_widget, &ExerciseWidget::ExerciseAnswered,
                this, &MainWindow::OnExerciseAnswered);
        connect(this, &MainWindow::ResizeEvent,
                translation_widget, &TranslationExercise::OnParentResized);
        stacked_widget_->addWidget(translation_widget);
    }

    StartTimer();

    constexpr int kBaseWidth = 800;
    constexpr int kBaseHeight = 600;

    const qreal w_ratio = static_cast<qreal>(width()) / kBaseWidth;
    const qreal h_ratio = static_cast<qreal>(height()) / kBaseHeight;
    emit ResizeEvent(w_ratio, h_ratio);
}

void MainWindow::StartTimer() {
    int seconds_left = 40;

    timer_label_ = new QLabel(QString::number(seconds_left));
    main_layout_->addWidget(timer_label_);
    QFont font = timer_label_->font();
    font.setPointSize(16);
    timer_label_->setFont(font);

    QTimer *countdown_timer = new QTimer;

    QMetaObject::Connection conn;
    conn = connect(
        countdown_timer, &QTimer::timeout, this,
        [&conn, countdown_timer, this, seconds_left]() mutable {
            seconds_left--;
            timer_label_->setText(QString::number(seconds_left));
            if (seconds_left <= 0 || not_in_task_ || stacked_widget_->count() == 0) {
                countdown_timer->stop();
                main_layout_->removeWidget(timer_label_);
                timer_label_->hide();
                while (stacked_widget_->count() != 0) {
                    QWidget *current = stacked_widget_->currentWidget();
                    stacked_widget_->removeWidget(current);
                }
                translation_button_->show();
                grammar_button_->show();
                if (stacked_widget_->count() == 0) {
                    QTimer::singleShot(50, this, [this]() {
                        complete_effect_.play();
                    });
                }
                QMessageBox::information(this, "Complete",
                                         "The exercise completed. You answered on " + QString::number(correct_answer_) +
                                         "/5 questions correctly");
                correct_answer_ = 0;
            }
        });

    countdown_timer->start(1000);
}

void MainWindow::OnGrammar() {
    if (stacked_widget_->count() != 0) {
        return;
    }

    translation_button_->hide();
    grammar_button_->hide();

    not_in_task_ = false;

    for (int i = 0; i < 5; ++i) {
        GrammarExercise *grammar_widget = new GrammarExercise(
            difficult_level_, db_);
        connect(grammar_widget, &ExerciseWidget::ExerciseAnswered,
                this, &MainWindow::OnExerciseAnswered);
        connect(this, &MainWindow::ResizeEvent,
                grammar_widget, &GrammarExercise::OnParentResized);
        stacked_widget_->addWidget(grammar_widget);
    }

    StartTimer();

    constexpr int kBaseWidth = 800;
    constexpr int kBaseHeight = 600;

    const qreal w_ratio = static_cast<qreal>(width()) / kBaseWidth;
    const qreal h_ratio = static_cast<qreal>(height()) / kBaseHeight;
    emit ResizeEvent(w_ratio, h_ratio);
}

void MainWindow::OnExerciseAnswered(const bool ok) {
    if (ok) {
        exp_ += 1;
        correct_answer_++;
        UpdateEXP();
    }
    qobject_cast<ExerciseWidget *>(stacked_widget_->currentWidget())->ShowMessageBox();
    QWidget *current = stacked_widget_->currentWidget();
    stacked_widget_->removeWidget(current);
}

void MainWindow::OpenDB() {
    db_ = QSqlDatabase::addDatabase("QSQLITE");
    db_.setDatabaseName("labs/duolingo/data/questions.db");
    db_.open();
    QSqlQuery query;
    query.prepare("SELECT level, experience FROM user WHERE user_id = 1");
    if (query.exec()) {
        if (query.next()) {
            level_ = query.value(0).toInt();
            exp_ = query.value(1).toInt();
            necessary_exp_ = level_ * 10;
        }
    }
}

void MainWindow::SaveUserData() {
    QSqlQuery query;
    query.prepare("UPDATE user SET level = :level, experience = :xp WHERE user_id = :id");
    query.bindValue(":level", level_);
    query.bindValue(":xp", exp_);
    query.bindValue(":id", 1);
    query.exec();
}

void MainWindow::CallHint() {
    if (const auto grammar = qobject_cast<GrammarExercise *>(stacked_widget_->currentWidget())) {
        grammar->ShowHintBox();
    }
}
