#include "addtranslationdialog.h"

#include <QFormLayout>
#include <QLineEdit>
#include <QComboBox>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QLabel>

AddTranslationDialog::AddTranslationDialog(QWidget *parent) : QDialog(parent), buttonBox(nullptr) {
    setupUi();
    connect(buttonBox, &QDialogButtonBox::accepted, this, &AddTranslationDialog::validateAndAccept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);
}

void AddTranslationDialog::setupUi() {
    setWindowTitle(tr("Новое предложение для перевода"));
    setMinimumWidth(550);

    QFormLayout *layout = new QFormLayout(this);
    layout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
    layout->setLabelAlignment(Qt::AlignLeft);
    layout->setContentsMargins(15, 15, 15, 15);
    layout->setSpacing(10);

    originalEdit = new QLineEdit(this);
    originalEdit->setPlaceholderText(tr("Введите предложение на исходном языке"));
    layout->addRow(new QLabel(tr("Предложение (оригинал):")), originalEdit);

    translationEdit = new QLineEdit(this);
    translationEdit->setPlaceholderText(tr("Введите точный перевод предложения"));
    layout->addRow(new QLabel(tr("Перевод:")), translationEdit);

    levelCombo = new QComboBox(this);
    levelCombo->addItems({tr("Лёгкий"), tr("Средний"), tr("Сложный")});
    levelCombo->setToolTip(tr("Выберите уровень сложности для этого предложения"));
    layout->addRow(new QLabel(tr("Уровень сложности:")), levelCombo);

    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);
    layout->addRow(buttonBox);

    setLayout(layout);
}

QString AddTranslationDialog::getOriginalSentence() const {
    return originalEdit->text().trimmed();
}

QString AddTranslationDialog::getTranslatedSentence() const {
    return translationEdit->text().trimmed();
}

int AddTranslationDialog::getLevel() const {
   return levelCombo->currentIndex() + 1;
}

void AddTranslationDialog::validateAndAccept() {
    if (getOriginalSentence().isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"), tr("Оригинальное предложение не может быть пустым."));
        originalEdit->setFocus();
        return;
    }

    if (getTranslatedSentence().isEmpty()) {
        QMessageBox::warning(this, tr("Ошибка ввода"), tr("Перевод не может быть пустым."));
        translationEdit->setFocus();
        return;
    }
    accept();
}
