#include "translation_exercise.h"

#include <qdialogbuttonbox.h>
#include <QWidget>
#include <QLineEdit>
#include <QPushButton>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <qurl.h>
#include <QtSql/QSqlQuery>
#include <QSoundEffect>
#include <QSqlDatabase>

TranslationExercise::TranslationExercise(const int difficult_level, const QSqlDatabase &db,
                                         QWidget * /*parent*/) : difficult_level_(difficult_level), db_(db) {
    question_label_ = new QLabel(this);
    question_label_->setAlignment(Qt::AlignCenter);
    answer_edit_ = new QLineEdit(this);
    submit_button_ = new QPushButton("Check", this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(question_label_);
    layout->addWidget(answer_edit_);
    layout->addWidget(submit_button_);
    setLayout(layout);

    question_label_->setStyleSheet("color: white;");
    answer_edit_->setStyleSheet(R"(
    QLineEdit {
        background-color: white;
        color: purple;
        border: 1px solid purple;
        border-radius: 8px;
        padding: 5px;
    }
    QLineEdit:focus {
        border: 2px solid #aa00ff;
    }
    QLineEdit:hover {
        background-color: #f9f9f9;
    }
    )");
    submit_button_->setStyleSheet("QPushButton {"
        "background-color: #FFC0CB;"
        "color: white;"
        "border: none;"
        "border-radius: 12px;"
        "padding: 10px 20px;"
        "font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "background-color: #FFB6C1;"
        "}"
        "QPushButton:pressed {"
        "background-color: #FF69B4;"
        "}");

    connect(submit_button_, &QPushButton::clicked, this, &TranslationExercise::OnSubmit);
    connect(answer_edit_, &QLineEdit::returnPressed, this, &TranslationExercise::OnSubmit);

    TranslationExercise::LoadQuestion();
}

bool TranslationExercise::ValidateAnswer() {
    return answer_.toLower() == answer_edit_->text().toLower();
}

TranslationExercise::~TranslationExercise() {
}

void TranslationExercise::LoadQuestion() {
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
            "SELECT english_word, german_word from " + difficulties_[difficult_level_] + " where id == " +
            QString::number(index_) + ";")) {
            while (query.next()) {
                const QString english_word = query.value(0).toString();
                const QString german_word = query.value(1).toString();
                question_label_->setText(english_word);
                answer_ = german_word;
            }
        }
    }
}

void TranslationExercise::OnParentResized(double /*w_ratio*/, const double h_ratio) const { {
        const int font_size = static_cast<int>(48 * h_ratio);
        QFont label_font = question_label_->font();
        label_font.setPointSize(font_size);
        question_label_->setFont(label_font);
    } {
        const int font_size = static_cast<int>(16 * h_ratio);
        QFont edit_font = answer_edit_->font();
        edit_font.setPointSize(font_size);
        answer_edit_->setFont(edit_font);
    } {
        const int font_size = static_cast<int>(20 * h_ratio);
        submit_button_->setStyleSheet("QPushButton {"
                                      "background-color: #FFC0CB;"
                                      "color: white;"
                                      "border: none;"
                                      "border-radius: 12px;"
                                      "padding: 10px 20px;"
                                      "font-size: " + QString::number(font_size) + "px;"
                                      "}"
                                      "QPushButton:hover {"
                                      "background-color: #FFB6C1;"
                                      "}"
                                      "QPushButton:pressed {"
                                      "background-color: #FF69B4;"
                                      "}");
    }
}
