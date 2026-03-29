#include "addgrammardialog.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QSpinBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QLabel>

AddGrammarDialog::AddGrammarDialog(QWidget *parent) : QDialog(parent), buttonBox(nullptr) {
    setupUi();
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddGrammarDialog::validateAndAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AddGrammarDialog::setupUi() {
    setWindowTitle(tr("Новый вопрос по грамматике"));
    setMinimumWidth(500);

    QFormLayout *layout = new QFormLayout(this);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setLabelAlignment(Qt::AlignLeft);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    questionEdit = new QLineEdit(this);
    questionEdit->setPlaceholderText(tr("Введите полный текст вопроса"));
    layout->addRow(new QLabel(tr("Вопрос:")), questionEdit);

    for (int i = 0; i < 4; ++i) {
        optionEdits[i] = new QLineEdit(this);
        optionEdits[i]->setPlaceholderText(tr("Текст варианта ответа %1").arg(i + 1));
        layout->addRow(new QLabel(tr("Вариант %1:").arg(i + 1)), optionEdits[i]);
    }

    correctOptionSpin = new QSpinBox(this);
    correctOptionSpin->setRange(1, 4);
    correctOptionSpin->setToolTip(tr("Укажите номер правильного варианта ответа"));
    layout->addRow(new QLabel(tr("Номер правильного варианта (1-4):")), correctOptionSpin);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addRow(buttonBox);

    setLayout(layout);
}

QString AddGrammarDialog::getQuestionText() const {
    return questionEdit->text().trimmed();
}

QStringList AddGrammarDialog::getOptions() const {
    QStringList options;
    for (int i = 0; i < 4; ++i) {
        options << optionEdits[i]->text().trimmed();
    }
    return options;
}

int AddGrammarDialog::getCorrectOptionIndex() const {
    return correctOptionSpin->value() - 1;
}

void AddGrammarDialog::validateAndAccept() {
    if (getQuestionText().isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"), tr("Текст вопроса не может быть пустым."));
        questionEdit->setFocus();
        return;
    }

    for (int i = 0; i < 4; ++i) {
        if (optionEdits[i]->text().trimmed().isEmpty()) {
            QMessageBox::warning(this, tr("Ошибка ввода"), tr("Вариант ответа %1 не может быть пустым.").arg(i + 1));
            optionEdits[i]->setFocus();
            return;
        }
    }
    accept();
}
