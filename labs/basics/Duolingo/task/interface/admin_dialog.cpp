#include "admin_dialog.h"

#include "../database/database_manager.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QVariant>

AdminDialog::AdminDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Database Admin");
    resize(600, 400);
    setupUi();

    loadTranslations();
    loadGrammar();
}

void AdminDialog::setupUi() {
    auto* main_layout = new QVBoxLayout(this);
    tab_widget_ = new QTabWidget(this);
    main_layout->addWidget(tab_widget_);

    auto* trans_tab = new QWidget();
    auto* trans_layout = new QVBoxLayout(trans_tab);

    translation_list_ = new QListWidget();
    trans_layout->addWidget(translation_list_);

    auto* trans_input_layout = new QHBoxLayout();
    trans_prompt_edit_ = new QLineEdit();
    trans_prompt_edit_->setPlaceholderText("Prompt (e.g., 'Hello')");
    trans_answer_edit_ = new QLineEdit();
    trans_answer_edit_->setPlaceholderText("Answer (e.g., 'Привет')");
    trans_input_layout->addWidget(trans_prompt_edit_);
    trans_input_layout->addWidget(trans_answer_edit_);

    auto* add_trans_btn = new QPushButton("Add");
    auto* del_trans_btn = new QPushButton("Delete Selected");
    trans_input_layout->addWidget(add_trans_btn);
    trans_layout->addLayout(trans_input_layout);
    trans_layout->addWidget(del_trans_btn);

    connect(add_trans_btn, &QPushButton::clicked, this, &AdminDialog::addTranslation);
    connect(del_trans_btn, &QPushButton::clicked, this, &AdminDialog::deleteTranslation);

    tab_widget_->addTab(trans_tab, "Translations");

    auto* grammar_tab = new QWidget();
    auto* grammar_layout = new QVBoxLayout(grammar_tab);

    grammar_list_ = new QListWidget();
    grammar_layout->addWidget(grammar_list_);

    auto* grammar_input_layout = new QHBoxLayout();
    grammar_prompt_edit_ = new QLineEdit();
    grammar_prompt_edit_->setPlaceholderText("Prompt");
    grammar_options_edit_ = new QLineEdit();
    grammar_options_edit_->setPlaceholderText("Options (comma separated)");
    grammar_correct_edit_ = new QLineEdit();
    grammar_correct_edit_->setPlaceholderText("Correct Index (0-based)");

    grammar_input_layout->addWidget(grammar_prompt_edit_);
    grammar_input_layout->addWidget(grammar_options_edit_);
    grammar_input_layout->addWidget(grammar_correct_edit_);

    auto* add_grammar_btn = new QPushButton("Add");
    auto* del_grammar_btn = new QPushButton("Delete Selected");
    grammar_input_layout->addWidget(add_grammar_btn);

    grammar_layout->addLayout(grammar_input_layout);
    grammar_layout->addWidget(del_grammar_btn);

    connect(add_grammar_btn, &QPushButton::clicked, this, &AdminDialog::addGrammar);
    connect(del_grammar_btn, &QPushButton::clicked, this, &AdminDialog::deleteGrammar);

    tab_widget_->addTab(grammar_tab, "Grammar");
}

void AdminDialog::loadTranslations() {
    translation_list_->clear();
    auto questions = DatabaseManager::instance().getTranslationQuestions(100);
    for (const auto& q : questions) {
        auto* item =
            new QListWidgetItem(QString("[%1] %2 -> %3").arg(q.id).arg(q.prompt).arg(q.answer));
        item->setData(Qt::UserRole, q.id);
        translation_list_->addItem(item);
    }
}

void AdminDialog::loadGrammar() {
    grammar_list_->clear();
    auto questions = DatabaseManager::instance().getGrammarQuestions(100);
    for (const auto& q : questions) {
        auto* item = new QListWidgetItem(
            QString("[%1] %2 (Ans: %3)").arg(q.id).arg(q.prompt).arg(q.correct_index));
        item->setData(Qt::UserRole, q.id);
        grammar_list_->addItem(item);
    }
}

void AdminDialog::addTranslation() {
    QString prompt = trans_prompt_edit_->text();
    QString answer = trans_answer_edit_->text();
    if (prompt.isEmpty() || answer.isEmpty()) {
        QMessageBox::warning(this, "Error", "Fields cannot be empty.");
        return;
    }

    if (DatabaseManager::instance().addTranslationQuestion(prompt, answer)) {
        trans_prompt_edit_->clear();
        trans_answer_edit_->clear();
        loadTranslations();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add translation.");
    }
}

void AdminDialog::deleteTranslation() {
    auto* item = translation_list_->currentItem();
    if (!item) {
        return;
    }

    int id = item->data(Qt::UserRole).toInt();
    if (DatabaseManager::instance().deleteTranslationQuestion(id)) {
        loadTranslations();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete translation.");
    }
}

void AdminDialog::addGrammar() {
    QString prompt = grammar_prompt_edit_->text();
    QString options_str = grammar_options_edit_->text();
    QString correct_str = grammar_correct_edit_->text();

    if (prompt.isEmpty() || options_str.isEmpty() || correct_str.isEmpty()) {
        QMessageBox::warning(this, "Error", "Fields cannot be empty.");
        return;
    }

    QStringList options = options_str.split(",", Qt::SkipEmptyParts);
    for (auto& o : options) {
        o = o.trimmed();
    }

    bool ok;
    int correct_index = correct_str.toInt(&ok);
    if (!ok || correct_index < 0 || correct_index >= options.size()) {
        QMessageBox::warning(this, "Error", "Invalid correct index.");
        return;
    }

    if (DatabaseManager::instance().addGrammarQuestion(prompt, options, correct_index)) {
        grammar_prompt_edit_->clear();
        grammar_options_edit_->clear();
        grammar_correct_edit_->clear();
        loadGrammar();
    } else {
        QMessageBox::warning(this, "Error", "Failed to add grammar.");
    }
}

void AdminDialog::deleteGrammar() {
    auto* item = grammar_list_->currentItem();
    if (!item) {
        return;
    }

    int id = item->data(Qt::UserRole).toInt();
    if (DatabaseManager::instance().deleteGrammarQuestion(id)) {
        loadGrammar();
    } else {
        QMessageBox::warning(this, "Error", "Failed to delete grammar.");
    }
}
