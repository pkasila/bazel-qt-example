#include "lt_settings_dialog.h"

#include "../languagetool/answer_evaluator.h"
#include "styles.h"

#include <QGroupBox>
#include <QHBoxLayout>
#include <QLabel>
#include <QVBoxLayout>

LTSettingsDialog::LTSettingsDialog(QWidget* parent) : QDialog(parent) {
    setWindowTitle("Evaluation Settings");
    setMinimumWidth(400);
    setStyleSheet("QDialog { background-color: #1a1a24; color: #ffffff; }");

    auto* main_layout = new QVBoxLayout(this);
    main_layout->setSpacing(20);
    main_layout->setContentsMargins(20, 20, 20, 20);

    auto* title = new QLabel("LanguageTool Strictness", this);
    title->setStyleSheet("font-size: 20px; font-weight: bold; color: #ffffff;");
    main_layout->addWidget(title);

    auto* desc = new QLabel(
        "Select which language issues should fail the answer (Strict)\n"
        "and which should just show a warning (Lenient).",
        this);
    desc->setWordWrap(true);
    desc->setStyleSheet("font-size: 14px; color: #aaaaaa;");
    main_layout->addWidget(desc);

    auto* group_box = new QGroupBox("Treat as Strict Error", this);
    group_box->setStyleSheet(
        "QGroupBox { font-weight: bold; color: #ffffff; border: 1px solid #333; "
        "border-radius: 8px; margin-top: 10px; padding-top: 15px; }");
    auto* group_layout = new QVBoxLayout(group_box);

    grammar_cb_ = new QCheckBox("Grammar Rules", this);
    syntax_cb_ = new QCheckBox("Syntax / Capitalization", this);
    typos_cb_ = new QCheckBox("Spelling / Typos", this);
    punctuation_cb_ = new QCheckBox("Punctuation", this);
    style_cb_ = new QCheckBox("Style & Formatting", this);

    QString cb_style =
        "QCheckBox { font-size: 15px; color: #dddddd; padding: 5px; } "
        "QCheckBox::indicator { width: 18px; height: 18px; }";
    grammar_cb_->setStyleSheet(cb_style);
    syntax_cb_->setStyleSheet(cb_style);
    typos_cb_->setStyleSheet(cb_style);
    punctuation_cb_->setStyleSheet(cb_style);
    style_cb_->setStyleSheet(cb_style);

    grammar_cb_->setChecked(AnswerEvaluator::globalConfig.strictGrammar);
    syntax_cb_->setChecked(AnswerEvaluator::globalConfig.strictSyntax);
    typos_cb_->setChecked(AnswerEvaluator::globalConfig.strictTypos);
    punctuation_cb_->setChecked(AnswerEvaluator::globalConfig.strictPunctuation);
    style_cb_->setChecked(AnswerEvaluator::globalConfig.strictStyle);

    group_layout->addWidget(grammar_cb_);
    group_layout->addWidget(syntax_cb_);
    group_layout->addWidget(typos_cb_);
    group_layout->addWidget(punctuation_cb_);
    group_layout->addWidget(style_cb_);
    main_layout->addWidget(group_box);

    save_btn_ = new QPushButton("Save Settings", this);
    save_btn_->setStyleSheet(Styles::CHECK_BUTTON);
    connect(save_btn_, &QPushButton::clicked, this, &LTSettingsDialog::onSaveClicked);
    main_layout->addWidget(save_btn_, 0, Qt::AlignRight);
}

void LTSettingsDialog::onSaveClicked() {
    AnswerEvaluator::globalConfig.strictGrammar = grammar_cb_->isChecked();
    AnswerEvaluator::globalConfig.strictSyntax = syntax_cb_->isChecked();
    AnswerEvaluator::globalConfig.strictTypos = typos_cb_->isChecked();
    AnswerEvaluator::globalConfig.strictPunctuation = punctuation_cb_->isChecked();
    AnswerEvaluator::globalConfig.strictStyle = style_cb_->isChecked();
    accept();
}
