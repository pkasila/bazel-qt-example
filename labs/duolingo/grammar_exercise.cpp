#include "grammar_exercise.h"

#include <QButtonGroup>
#include <QMessageBox>
#include <QSoundEffect>
#include <QSqlQuery>
#include <QVBoxLayout>

GrammarExercise::GrammarExercise(const int difficult_level, const QSqlDatabase &db, QWidget *parent) : difficult_level_(
        difficult_level), db_(db) {
    question_label_ = new QLabel(this);
    question_label_->setAlignment(Qt::AlignCenter);
    option_1_ = new QRadioButton(this);
    option_2_ = new QRadioButton(this);
    option_3_ = new QRadioButton(this);
    option_4_ = new QRadioButton(this);
    options_group_ = new QButtonGroup(this);
    options_group_->addButton(option_1_, 1);
    options_group_->addButton(option_2_, 2);
    options_group_->addButton(option_3_, 3);
    options_group_->addButton(option_4_, 4);
    options_group_->setExclusive(true);
    submit_button_ = new QPushButton("Check", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(question_label_);

    QHBoxLayout *options_layout = new QHBoxLayout; {
        QVBoxLayout *left_col = new QVBoxLayout;
        left_col->addWidget(option_1_);
        left_col->addWidget(option_2_);

        QVBoxLayout *right_col = new QVBoxLayout;
        right_col->addWidget(option_3_);
        right_col->addWidget(option_4_);

        options_layout->addLayout(left_col);
        options_layout->addLayout(right_col);
    }
    layout->addLayout(options_layout);

    layout->addWidget(submit_button_);
    setLayout(layout);

    question_label_->setStyleSheet("color: white;");

    const QString rb_style = R"(
    QRadioButton {
        spacing: 5px;
        padding: 4px;
        color: white;
    }
    QRadioButton::indicator {
        width: 16px;
        height: 16px;
        border-radius: 8px;
        border: 1px solid white;
        background: transparent;
    }
    QRadioButton::indicator:hover {
        border-color: #ddddff;
    }
    QRadioButton::indicator:checked {
        background-color: pink;
        border: 1px solid pink;
    }
)";
    option_1_->setStyleSheet(rb_style);
    option_2_->setStyleSheet(rb_style);
    option_3_->setStyleSheet(rb_style);
    option_4_->setStyleSheet(rb_style);

    submit_button_->setStyleSheet(R"(
        QPushButton {
            background-color: #FFC0CB;
            color: white;
            border: none;
            border-radius: 12px;
            padding: 10px 20px;
            font-size: 16px;
        }
        QPushButton:hover {
            background-color: #FFB6C1;
        }
        QPushButton:pressed {
            background-color: #FF69B4;
        }
    )");

    connect(submit_button_, &QPushButton::clicked, this, &GrammarExercise::OnSubmit);

    GrammarExercise::LoadQuestion();
}

void GrammarExercise::OnParentResized(double /*w_ratio*/, const double h_ratio) const { {
        const int font_size = static_cast<int>(36 * h_ratio);
        QFont label_font = question_label_->font();
        label_font.setPointSize(font_size);
        question_label_->setFont(label_font);
    } {
        const int dia = qBound(8, static_cast<int>(16 * h_ratio), 32);
        const int radius = dia / 2;
        const int text_fs = static_cast<int>(16 * h_ratio);
        const QString rb_style = QString(R"(
            QRadioButton {
                spacing: 5px;
                padding: 4px;
                font-size: %3px;
                color: white;
            }
            QRadioButton::indicator {
                width: %1px;
                height: %1px;
                border-radius: %2px;       /* половина размера — идеально круг */
                border: 2px solid white;
                background: transparent;
            }
            QRadioButton::indicator:hover {
                border-color: pink;
            }
            QRadioButton::indicator:checked {
                background-color: pink;
                border: 2px solid pink;
            }
        )").arg(dia).arg(radius).arg(text_fs);
        option_1_->setStyleSheet(rb_style);
        option_2_->setStyleSheet(rb_style);
        option_3_->setStyleSheet(rb_style);
        option_4_->setStyleSheet(rb_style);
    } {
        const int font_size = static_cast<int>(20 * h_ratio);
        const QString sheet = QString(R"(
            QPushButton {
                background-color: #FFC0CB;
                color: white;
                border: none;
                border-radius: 12px;
                padding: 10px 20px;
                font-size: %1px;
            }
            QPushButton:hover {
                background-color: #FFB6C1;
            }
            QPushButton:pressed {
                background-color: #FF69B4;
            }
        )").arg(font_size);
        submit_button_->setStyleSheet(sheet);
    }
}


GrammarExercise::~GrammarExercise() {
}

bool GrammarExercise::ValidateAnswer() {
    return options_group_->checkedId() == answer_index_;
}

void GrammarExercise::LoadQuestion() {
    QSqlQuery select_query;
    select_query.exec(
        "SELECT question_id FROM " + difficulty_indices_table_names_[difficult_level_] +
        " ORDER BY question_id LIMIT 1");
    if (!select_query.next()) {
        QMessageBox::information(this, "Congratulation!",
                                 "There's no more questions with this difficulty! Select another difficulty");
    }
    index_ = select_query.value(0).toInt();

    QSqlQuery delete_query;
    delete_query.prepare(
        "DELETE FROM " + difficulty_indices_table_names_[difficult_level_] + " WHERE question_id = :id");
    delete_query.bindValue(":id", index_);
    delete_query.exec(); {
        QSqlQuery query;
        if (query.exec(
            "SELECT task, options1, options2, options3, options4, answer, rule from " + difficulties_[difficult_level_]
            + " where id == " +
            QString::number(index_) + ";")) {
            while (query.next()) {
                const QString task = query.value(0).toString();
                const QString option1 = query.value(1).toString();
                const QString option2 = query.value(2).toString();
                const QString option3 = query.value(3).toString();
                const QString option4 = query.value(4).toString();
                answer_index_ = query.value(5).toString().toInt();
                switch (answer_index_) {
                    case 1:
                        answer_ = option1;
                        break;
                    case 2:
                        answer_ = option2;
                        break;
                    case 3:
                        answer_ = option3;
                        break;
                    case 4:
                        answer_ = option4;
                        break;
                }
                rule_name_ = query.value(6).toString();
                question_label_->setText(task);
                option_1_->setText(option1);
                option_2_->setText(option2);
                option_3_->setText(option3);
                option_4_->setText(option4);
            }
        }
    } {
        QSqlQuery query;
        if (query.exec(
            "SELECT rule from rules where topic == '" + rule_name_ + "'")) {
            while (query.next()) {
                hint_ = query.value(0).toString();
            }
        }
    }
}

void GrammarExercise::ShowHintBox() {
    QMessageBox::information(this, "Help",
                             rule_name_ + ": " + hint_);
}
