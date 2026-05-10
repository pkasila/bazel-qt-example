#include "mainwindow.h"

#include "styles.h"

#include <QHBoxLayout>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget* parent)
    : QMainWindow(parent),
      time_left_(0),
      score_(0),
      hearts_(3),
      difficulty_("Beginner"),
      default_duration_(60) {
    setWindowTitle("DuoLingo Clone");
    resize(1000, 700);
    setStyleSheet(Styles::MAIN_WINDOW);

    setupMenus();

    auto* central = new QWidget(this);
    auto* main_layout = new QHBoxLayout(central);
    main_layout->setContentsMargins(0, 0, 0, 0);
    main_layout->setSpacing(0);
    setCentralWidget(central);

    sidebar_ = new SidebarWidget(this);
    main_layout->addWidget(sidebar_);

    auto* content_area = new QVBoxLayout();
    timer_label_ = new QLabel(this);
    timer_label_->setStyleSheet(Styles::TIMER_TEXT);
    timer_label_->setAlignment(Qt::AlignRight);

    progress_bar_ = new QProgressBar(this);
    progress_bar_->setStyleSheet(Styles::PROGRESS_BAR);
    progress_bar_->setTextVisible(false);
    progress_bar_->setRange(0, 5);
    progress_bar_->setValue(0);

    content_area->addWidget(timer_label_);
    content_area->addWidget(progress_bar_);

    stack_ = new QStackedWidget(this);
    content_area->addWidget(stack_);
    main_layout->addLayout(content_area, 1);

    welcome_widget_ = new QWidget(this);
    auto* welcome_layout = new QVBoxLayout(welcome_widget_);
    auto* welcome_label = new QLabel(
        "Welcome to DuoLingo Clone!\nSelect an exercise from the sidebar to begin.",
        welcome_widget_);
    welcome_label->setStyleSheet(Styles::QUESTION_TEXT);
    welcome_label->setAlignment(Qt::AlignCenter);
    welcome_layout->addWidget(welcome_label);
    stack_->addWidget(welcome_widget_);
    translation_widget_ = new TranslationWidget(this);
    stack_->addWidget(translation_widget_);
    connect(
        translation_widget_, &TranslationWidget::answerSubmitted, this,
        &MainWindow::onAnswerSubmitted);
    grammar_widget_ = new GrammarWidget(this);
    stack_->addWidget(grammar_widget_);
    connect(grammar_widget_, &GrammarWidget::answerSubmitted, this, &MainWindow::onAnswerSubmitted);
    connect(
        sidebar_, &SidebarWidget::translationClicked, this, &MainWindow::startTranslationSession);
    connect(sidebar_, &SidebarWidget::grammarClicked, this, &MainWindow::startGrammarSession);
    exercise_timer_ = new QTimer(this);
    connect(exercise_timer_, &QTimer::timeout, this, &MainWindow::updateTimer);
}

void MainWindow::setupMenus() {
    auto* settingsMenu = menuBar()->addMenu("Settings");
    auto* difficultyAction = settingsMenu->addAction("Change Difficulty");
    connect(difficultyAction, &QAction::triggered, this, &MainWindow::showDifficultyDialog);

    auto* helpMenu = menuBar()->addMenu("Help");
    auto* aboutAction = helpMenu->addAction("Show Help (H)");
    connect(aboutAction, &QAction::triggered, this, &MainWindow::showHelp);

    auto* adminAction = settingsMenu->addAction("Admin Panel");
    connect(adminAction, &QAction::triggered, this, &MainWindow::showAdminDialog);

    auto* evalSettingsAction = settingsMenu->addAction("Evaluation Settings");
    connect(evalSettingsAction, &QAction::triggered, this, &MainWindow::showLTSettingsDialog);
}

void MainWindow::startTranslationSession() {
    is_translation_mode_ = true;
    trans_questions_ = DatabaseManager::instance().getTranslationQuestions(5);
    if (trans_questions_.isEmpty()) {
        QMessageBox::warning(this, "No Questions", "Database is empty!");
        return;
    }
    questions_left_ = trans_questions_.size();
    current_question_index_ = 0;
    time_left_ = default_duration_;
    timer_label_->setText(QString("Time: %1s").arg(time_left_));
    exercise_timer_->start(1000);
    sidebar_->setActiveButton(0);
    nextQuestion();
}

void MainWindow::startGrammarSession() {
    is_translation_mode_ = false;
    grammar_questions_ = DatabaseManager::instance().getGrammarQuestions(5);
    if (grammar_questions_.isEmpty()) {
        QMessageBox::warning(this, "No Questions", "Database is empty!");
        return;
    }
    questions_left_ = grammar_questions_.size();
    current_question_index_ = 0;
    time_left_ = default_duration_;
    timer_label_->setText(QString("Time: %1s").arg(time_left_));
    exercise_timer_->start(1000);
    sidebar_->setActiveButton(1);
    nextQuestion();
}

void MainWindow::nextQuestion() {
    int total_questions = is_translation_mode_ ? trans_questions_.size() : grammar_questions_.size();
    progress_bar_->setMaximum(total_questions);
    progress_bar_->setValue(total_questions - questions_left_);
    if (questions_left_ <= 0) {
        endSession(true, "Congratulations! You completed the exercise.");
        return;
    }

    if (is_translation_mode_) {
        auto q = trans_questions_[current_question_index_];
        translation_widget_->setQuestion(q.prompt, q.answer);
        stack_->setCurrentWidget(translation_widget_);
    } else {
        auto q = grammar_questions_[current_question_index_];
        grammar_widget_->setQuestion(q.prompt, q.options, q.correct_index);
        stack_->setCurrentWidget(grammar_widget_);
    }
}

void MainWindow::onAnswerSubmitted(bool correct) {
    if (correct) {
        questions_left_--;
        current_question_index_++;
        nextQuestion();
    } else {
        hearts_--;
        sidebar_->setHearts(hearts_);
        if (hearts_ <= 0) {
            endSession(false, "Out of hearts! Try again later.");
        } else {
            QMessageBox::warning(this, "Incorrect", "That's not quite right. Try again!");
        }
    }
}

void MainWindow::updateTimer() {
    time_left_--;
    timer_label_->setText(QString("Time: %1s").arg(time_left_));
    if (time_left_ <= 0) {
        endSession(false, "Time is up!");
    }
}

void MainWindow::endSession(bool success, const QString& message) {
    exercise_timer_->stop();
    timer_label_->clear();

    if (success) {
        score_ += 100;
        sidebar_->setScore(score_);
        QMessageBox::information(this, "Success", message);
    } else {
        QMessageBox::critical(this, "Session Ended", message);
        hearts_ = 3;
        sidebar_->setHearts(hearts_);
    }

    stack_->setCurrentWidget(welcome_widget_);
    sidebar_->setActiveButton(-1);
}

void MainWindow::showDifficultyDialog() {
    DifficultyDialog diag(this);
    if (diag.exec() == QDialog::Accepted) {
        difficulty_ = diag.getDifficulty();
        default_duration_ = diag.getDuration();
        QMessageBox::information(
            this, "Settings Updated",
            QString("Difficulty: %1\nTime limit: %2s").arg(difficulty_).arg(default_duration_));
    }
}

void MainWindow::showHelp() {
    if (stack_->currentWidget() == welcome_widget_) {
        return; // Block hints when not doing tasks
    }

    QString correct_answer;
    if (is_translation_mode_) {
        if (current_question_index_ >= 0 && current_question_index_ < trans_questions_.size()) {
            correct_answer = trans_questions_[current_question_index_].answer;
        }
    } else {
        if (current_question_index_ >= 0 && current_question_index_ < grammar_questions_.size()) {
            auto q = grammar_questions_[current_question_index_];
            correct_answer = q.options[q.correct_index];
        }
    }

    QMessageBox::information(this, "Hint", "The correct answer is:\n" + correct_answer);
}

void MainWindow::keyPressEvent(QKeyEvent* event) {
    if (event->key() == Qt::Key_H || event->key() == 0x0420) { // 0x0420 is Cyrillic 'Р'
        showHelp();
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::showAdminDialog() {
    AdminDialog diag(this);
    diag.exec();
}

void MainWindow::showLTSettingsDialog() {
    LTSettingsDialog diag(this);
    diag.exec();
}
