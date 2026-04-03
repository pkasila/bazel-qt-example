#include "difficultydialog.h"
#include <QMenuBar>
#include <QMenu>
#include <QAction>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QRandomGenerator>
#include <QGraphicsDropShadowEffect>

#include "mainwindow.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent) {
    LoadExerciseData();
    SetupMenu();
    SetupUi();
    main_stacked_widget_->setCurrentWidget(main_menu_page_);
    setWindowTitle("SoloGudman");
    resize(800, 600);
}

MainWindow::~MainWindow() {

}

void MainWindow::LoadExerciseData() {
    all_translation_questions_.push_back({"Hello", "Привет", "Standard greeting."});
    all_translation_questions_.push_back({"World", "Мир", "'мир' can also mean 'peace'."});
    all_translation_questions_.push_back({"Apple", "Яблоко", "A common fruit."});
    all_translation_questions_.push_back({"Dog", "Собака", "Man's best friend."});
    all_translation_questions_.push_back({"Cat", "Кот", "Meow meow."});
    all_translation_questions_.push_back({"Thank you", "Спасибо", "You say it, when someone did you good."});
    all_translation_questions_.push_back({"Good morning", "Доброе утро", "Morning greeting."});

    all_grammar_questions_.push_back({"He ", " to school every day.", {"go", "goes", "went"}, 1, "Present Simple, 3rd person singular."});
    all_grammar_questions_.push_back({"They ", " playing football now.", {"is", "are", "am"}, 1, "Present Continuous, plural."});
    all_grammar_questions_.push_back({"I ", " a student.", {"is", "are", "am"}, 2, "Verb 'to be', 1st person singular."});
    all_grammar_questions_.push_back({"She ", " happy yesterday.", {"was", "were", "is"}, 0, "Past Simple, verb 'to be'."});
    all_grammar_questions_.push_back({"This is ", " apple.", {"a", "an", "the"}, 1, "Article usage with vowel sounds."});
    all_grammar_questions_.push_back({"We ", " to the cinema last night.", {"go", "goes", "went"}, 2, "Past Simple, irregular verb."});
    all_grammar_questions_.push_back({"My brother ", " taller than me.", {"is", "are", "am"}, 0, "Comparative adjective."});
    all_grammar_questions_.push_back({"Listen! The baby ", ".", {"cry", "cries", "is crying"}, 2, "Present Continuous for actions happening now."});

    switch (cur_difficulty_) {
        case Difficulty::Easy:
            questions_in_set_ = 3;
            max_incorrect_attempts_ = 5;
            time_limit_seconds_ = 90;
            break;
        case Difficulty::Medium:
            questions_in_set_ = 5;
            max_incorrect_attempts_ = 3;
            time_limit_seconds_ = 60;
            break;
        case Difficulty::Hard:
            questions_in_set_ = 7;
            max_incorrect_attempts_ = 2;
            time_limit_seconds_ = 45;
            break;
    }
}

void MainWindow::SetupMenu() {

    menu_bar_ = new QMenuBar(this);

    QMenu *file_menu = menu_bar_->addMenu("&File");

    difficulty_action_ = new QAction("&Difficulty...", this);
    connect(difficulty_action_, &QAction::triggered, this, &MainWindow::ShowDifficultyDialog);
    file_menu->addAction(difficulty_action_);

    file_menu->addSeparator();

    QAction *exit_action = new QAction("&Exit", this);
    connect(exit_action, &QAction::triggered, this, &QWidget::close);
    file_menu->addAction(exit_action);

    setMenuBar(menu_bar_);
}

void MainWindow::SetupUi() {
    main_stacked_widget_ = new QStackedWidget(this);
    CreateMainMenuPage();
    CreateExercisePage();

    main_stacked_widget_->addWidget(main_menu_page_);
    main_stacked_widget_->addWidget(exercise_page_);

    setCentralWidget(main_stacked_widget_);

    exercise_timer_ = new QTimer(this);
    connect(exercise_timer_, &QTimer::timeout, this, &MainWindow::UpdateTimer);
}

void MainWindow::CreateMainMenuPage() {
    main_menu_page_ = new QWidget(this);
    QVBoxLayout *main_menu_layout = new QVBoxLayout(main_menu_page_);
    main_menu_layout->setAlignment(Qt::AlignCenter);

    QLabel *title_label = new QLabel("App for learning English", main_menu_page_);
    title_label->setAlignment(Qt::AlignCenter);
    title_label->setStyleSheet("color: #adff2f; font-size: 24px; font-weight: bold; margin-bottom: 20px;");
    main_menu_layout->addWidget(title_label);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::black);
    shadow->setOffset(1, 1);
    title_label->setGraphicsEffect(shadow);

    score_label_ = new QLabel("Total Score: 0", main_menu_page_);
    score_label_->setAlignment(Qt::AlignCenter);
    score_label_->setStyleSheet("font-size: 16px; margin-bottom: 30px;");
    main_menu_layout->addWidget(score_label_);

    translation_btn_ = new QPushButton("Translation", main_menu_page_);
    translation_btn_->setMinimumHeight(50);
    translation_btn_->setStyleSheet("background-color: #f0fff0; font-size: 18px;");
    connect(translation_btn_, &QPushButton::clicked, this, &MainWindow::StartTranslationExercise);
    main_menu_layout->addWidget(translation_btn_);

    grammar_btn_ = new QPushButton("Grammar", main_menu_page_);
    grammar_btn_->setMinimumHeight(50);
    grammar_btn_->setStyleSheet("background-color: #f0fff0; font-size: 18px; margin-top: 10px;");
    connect(grammar_btn_, &QPushButton::clicked, this, &MainWindow::StartGrammarExercise);
    main_menu_layout->addWidget(grammar_btn_);

    main_menu_layout->addStretch();
}

void MainWindow::CreateExercisePage() {
    exercise_page_ = new QWidget(this);
    QVBoxLayout *page_layout = new QVBoxLayout(exercise_page_);

    QHBoxLayout *top_bar_layout_ = new QHBoxLayout();
    exercise_title_label_ = new QLabel("Exercise", exercise_page_);
    exercise_title_label_->setStyleSheet("font-size: 18px; font-weight: bold;");
    progress_bar_ = new QProgressBar(exercise_page_);
    timer_label_ = new QLabel("Time: 00:00", exercise_page_);
    timer_label_->setStyleSheet("font-size: 14px;");

    top_bar_layout_->addWidget(exercise_title_label_, 1);
    top_bar_layout_->addWidget(progress_bar_, 2);
    top_bar_layout_->addWidget(timer_label_, 0, Qt::AlignRight);
    page_layout->addLayout(top_bar_layout_);

    question_text_label_ = new QLabel("Question will appear here.", exercise_page_);
    question_text_label_->setWordWrap(true);
    question_text_label_->setStyleSheet("font-size: 16px; margin-top: 20px; margin-bottom: 20px; border: 1px solid gray; padding: 10px;");
    question_text_label_->setAlignment(Qt::AlignCenter);
    page_layout->addWidget(question_text_label_);

    exercise_input_stacked_widget_ = new QStackedWidget(exercise_page_);

    translation_input_widget_ = new QWidget(exercise_input_stacked_widget_);
    QVBoxLayout *translate_layout = new QVBoxLayout(translation_input_widget_);
    translation_answer_edit_ = new QTextEdit(translation_input_widget_);
    translation_answer_edit_->setPlaceholderText("Enter your translation here:");
    translation_answer_edit_->setFixedHeight(100);
    translate_layout->addWidget(translation_answer_edit_);
    exercise_input_stacked_widget_->addWidget(translation_input_widget_);

    grammar_input_widget_ = new QWidget(exercise_input_stacked_widget_);
    QVBoxLayout *gramLayout = new QVBoxLayout(grammar_input_widget_);
    grammar_options_group_ = new QGroupBox("Choose the correct option:", grammar_input_widget_);
    grammar_options_layout_ = new QVBoxLayout();
    grammar_options_group_->setLayout(grammar_options_layout_);
    gramLayout->addWidget(grammar_options_group_);
    exercise_input_stacked_widget_->addWidget(grammar_input_widget_);

    page_layout->addWidget(exercise_input_stacked_widget_);

    page_layout->addStretch();

    submit_btn_ = new QPushButton("Submit", exercise_page_);
    submit_btn_->setMinimumHeight(40);
    submit_btn_->setStyleSheet("font-size: 16px;");
    connect(submit_btn_, &QPushButton::clicked, this, &MainWindow::HandleSubmit);
    page_layout->addWidget(submit_btn_);
}


void MainWindow::ShowDifficultyDialog() {
    DifficultyDialog dialog(this);
    dialog.SetCurrentDifficulty(cur_difficulty_);
    if (dialog.exec() == QDialog::Accepted) {
        Difficulty old_difficulty = cur_difficulty_;
        cur_difficulty_ = dialog.GetSelectedDifficulty();
        qDebug() << "New difficulty:" << int(cur_difficulty_);

        QString new_diff_str;
        switch(cur_difficulty_) {
            case Difficulty::Easy: new_diff_str = "Easy"; break;
            case Difficulty::Medium: new_diff_str = "Medium"; break;
            case Difficulty::Hard: new_diff_str = "Hard"; break;
        }

        if (old_difficulty != cur_difficulty_) {
            LoadExerciseData();
            QMessageBox::information(this, "Difficulty Changed",
                                     "Difficulty set to: " + new_diff_str +
                                         ".");
        } else {
            QMessageBox::information(this, "Difficulty",
                                     "Difficulty remains: " + new_diff_str);
        }
    }
}

void MainWindow::StartTranslationExercise() {
    cur_exercise_type_ = ExerciseType::Translation;
    exercise_title_label_->setText("Translation");
    exercise_title_label_->setStyleSheet("color: #adff2f;");

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::black);
    shadow->setOffset(1, 1);
    exercise_title_label_->setGraphicsEffect(shadow);

    exercise_input_stacked_widget_->setCurrentWidget(translation_input_widget_);
    StartNewExerciseSet(ExerciseType::Translation);
}

void MainWindow::StartGrammarExercise() {
    cur_exercise_type_ = ExerciseType::Grammar;
    exercise_title_label_->setText("Grammar");

    exercise_title_label_->setStyleSheet("color: #adff2f;");

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect;
    shadow->setBlurRadius(0);
    shadow->setColor(Qt::black);
    shadow->setOffset(1, 1);
    exercise_title_label_->setGraphicsEffect(shadow);

    exercise_input_stacked_widget_->setCurrentWidget(grammar_input_widget_);
    StartNewExerciseSet(ExerciseType::Grammar);
}

void MainWindow::StartNewExerciseSet(ExerciseType type) {
    if ((type == ExerciseType::Translation && all_translation_questions_.empty()) ||
        (type == ExerciseType::Grammar && all_grammar_questions_.empty())) {
        QMessageBox::warning(this, "No Questions", "No questions available for this exercise type.");
        return;
    }
    cur_question_index_ = 0;
    incorrect_attempts_count_ = 0;
    cur_exercise_score_ = 0;
    cur_translation_set_.clear();
    cur_grammar_set_.clear();

    QRandomGenerator *rnd = QRandomGenerator::global();

    if (type == ExerciseType::Translation) {
        QVector<int> indices;
        for(int i = 0; i < all_translation_questions_.size(); i++) {
            indices.append(i);
        }
        std::shuffle(indices.begin(), indices.end(), *rnd);
        for(int i = 0; i < std::min(questions_in_set_,
                                    static_cast<int>(all_translation_questions_.size())); i++) {
            cur_translation_set_.push_back(all_translation_questions_[indices[i]]);
        }
        if (cur_translation_set_.empty()) {
            QMessageBox::warning(this, "No Questions", "There are no translation questions to start.");
            return;
        }
    } else {
        QVector<int> indices;
        for (int i = 0; i < all_grammar_questions_.size(); i++) {
            indices.append(i);
        }
        std::shuffle(indices.begin(), indices.end(), *rnd);
        for (int i = 0; i < std::min(questions_in_set_,
                                     static_cast<int>(all_grammar_questions_.size())); i++) {
            cur_grammar_set_.push_back(all_grammar_questions_[indices[i]]);
        }
        if (cur_grammar_set_.empty()) {
            QMessageBox::warning(this, "No Questions", "There are no grammar questions to start.");
            return;
        }
    }

    progress_bar_->setMaximum( (type == ExerciseType::Translation) ? cur_translation_set_.size() : cur_grammar_set_.size() );
    progress_bar_->setValue(0);

    remaining_time_ = time_limit_seconds_;
    UpdateTimer();
    exercise_timer_->start(1000);

    main_stacked_widget_->setCurrentWidget(exercise_page_);
    LoadNextQuestion();
}

void MainWindow::LoadNextQuestion() {
    if (cur_exercise_type_ == ExerciseType::Translation) {
        if (cur_question_index_ < cur_translation_set_.size()) {
            DisplayTranslationQuestion(cur_translation_set_[cur_question_index_]);
        } else {
            EndExercise(true, "Well done!");
        }
    } else if (cur_exercise_type_ == ExerciseType::Grammar) {
        if (cur_question_index_ < cur_grammar_set_.size()) {
            DisplayGrammarQuestion(cur_grammar_set_[cur_question_index_]);
        } else {
            EndExercise(true, "Well done!");
        }
    }
    UpdateProgressBar();
}

void MainWindow::DisplayTranslationQuestion(const TranslationQuestion& q) {
    question_text_label_->setText("Translate the following: \n\n\"" + q.original_sentence + "\"");
    translation_answer_edit_->clear();
    translation_answer_edit_->setFocus();
}

void MainWindow::DisplayGrammarQuestion(const GrammarQuestion& q) {
    question_text_label_->setText(q.sentence_p1 + "______" + q.sentence_p2);

    qDeleteAll(grammar_radio_btns_);
    grammar_radio_btns_.clear();
    QLayoutItem* item;
    while ((item = grammar_options_layout_->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    for (const QString& option : q.options) {
        QRadioButton *rb = new QRadioButton(option, grammar_options_group_);
        grammar_radio_btns_.append(rb);
        grammar_options_layout_->addWidget(rb);
    }
    if (!grammar_radio_btns_.isEmpty()) {
        grammar_radio_btns_.first()->setChecked(true);
    }
}


void MainWindow::HandleSubmit() {
    CheckAnswer();
}

void MainWindow::CheckAnswer() {
    bool is_correct = false;
    if (cur_exercise_type_ == ExerciseType::Translation) {
        if (cur_question_index_ >= cur_translation_set_.size()) return;
        QString user_answer = translation_answer_edit_->toPlainText().trimmed();
        is_correct = (user_answer.compare(cur_translation_set_[cur_question_index_].correct_answer, Qt::CaseInsensitive) == 0);
    } else if (cur_exercise_type_ == ExerciseType::Grammar) {
        if (cur_question_index_ >= cur_grammar_set_.size()) return;
        int selectedOption = -1;
        for (int i = 0; i < grammar_radio_btns_.size(); i++) {
            if (grammar_radio_btns_[i]->isChecked()) {
                selectedOption = i;
                break;
            }
        }
        is_correct = (selectedOption == cur_grammar_set_[cur_question_index_].correct_option_index);
    }

    if (is_correct) {
        QMessageBox::information(this, "Correct!", "Correct answer.");
        cur_question_index_++;
        LoadNextQuestion();
    } else {
        incorrect_attempts_count_++;
        QMessageBox::warning(this, "Incorrect", QString("Incorrect answer. Attempts remaining: %1")
                                                    .arg(max_incorrect_attempts_ - incorrect_attempts_count_));
        if (incorrect_attempts_count_ >= max_incorrect_attempts_) {
            EndExercise(false, "Exercise failed. Too many incorrect attempts. Try again!");
        }
    }
}

void MainWindow::EndExercise(bool ok, const QString& msg) {
    exercise_timer_->stop();
    if (ok) {
        cur_exercise_score_ = 10;
        total_score_ += cur_exercise_score_;
        score_label_->setText(QString("Total Score: %1").arg(total_score_));
        QMessageBox::information(this, "Exercise finished", msg + QString("\nYou earned %1 points.").arg(cur_exercise_score_));
    } else {
        cur_exercise_score_ = 0;
        QMessageBox::critical(this, "Exercise failed", msg + "\nNo points awarded for this set.");
    }
    cur_exercise_type_ = ExerciseType::None;
    main_stacked_widget_->setCurrentWidget(main_menu_page_);
}

void MainWindow::UpdateProgressBar() {
    progress_bar_->setValue(cur_question_index_);
}

void MainWindow::UpdateTimer() {
    remaining_time_--;
    int minutes = remaining_time_ / 60;
    int seconds = remaining_time_ % 60;
    timer_label_->setText(QString("Time: %1:%2")
                            .arg(minutes, 2, 10, QChar('0'))
                            .arg(seconds, 2, 10, QChar('0')));

    if (remaining_time_ <= 0) {
        HandleTimeout();
    }
}

void MainWindow::HandleTimeout() {
    exercise_timer_->stop();
    EndExercise(false, "Time is up! Exercise failed.");
}


void MainWindow::keyPressEvent(QKeyEvent *event) {
    if (event->key() == Qt::Key_H) {
        if (main_stacked_widget_->currentWidget() == exercise_page_ && cur_exercise_type_ != ExerciseType::None) {
            ShowHelp();
        }
    } else {
        QMainWindow::keyPressEvent(event);
    }
}

void MainWindow::ShowHelp() {
    QString helpText = "No instructions for this question.";
    if (cur_exercise_type_ == ExerciseType::Translation) {
        if (cur_question_index_ < cur_translation_set_.size()) {
            helpText = cur_translation_set_[cur_question_index_].help_text;
        }
    } else if (cur_exercise_type_ == ExerciseType::Grammar) {
        if (cur_question_index_ < cur_grammar_set_.size()) {
            helpText = cur_grammar_set_[cur_question_index_].help_text;
        }
    }
    QMessageBox::information(this, "Help", helpText);
}
