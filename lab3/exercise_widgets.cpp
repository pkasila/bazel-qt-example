#include "exercise_widgets.h"

#include <QFont>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QRadioButton>
#include <QVBoxLayout>

BaseExerciseWidget::BaseExerciseWidget(QWidget* parent) : QWidget(parent) {
}

void BaseExerciseWidget::setOnAnswerSubmitted(AnswerCallback cb) {
    m_onAnswer = std::move(cb);
}

void BaseExerciseWidget::setOnEnterPressed(EnterCallback cb) {
    m_onEnter = std::move(cb);
}

TranslationWidget::TranslationWidget(QWidget* parent) : BaseExerciseWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    m_lblPrompt = new QLabel("", this);
    m_lblPrompt->setAlignment(Qt::AlignCenter);
    m_lblPrompt->setFont(QFont("Arial", 14, QFont::Bold));
    m_lblPrompt->setWordWrap(true);
    m_lblPrompt->setTextFormat(Qt::RichText);

    m_input = new QLineEdit(this);
    m_input->setPlaceholderText("Type your answer here...");
    m_input->setFont(QFont("Arial", 12));
    m_input->setMinimumHeight(40);
    m_input->setToolTip("Enter your translation and press Enter or click Submit");

    connect(m_input, &QLineEdit::returnPressed, this, [this]() {
        if (m_onEnter) {
            m_onEnter();
        }
    });

    layout->addWidget(m_lblPrompt);
    layout->addSpacing(10);
    layout->addWidget(m_input);
    layout->addSpacing(10);
}

void TranslationWidget::setQuestion(const Question& q) {
    m_current = q;
    if (q.reverseTranslation) {
        m_lblPrompt->setText(QString("<b>Translate to Russian:</b><br>%1").arg(q.prompt));
        m_input->setPlaceholderText("Enter translation in Russian...");
    } else {
        m_lblPrompt->setText(QString("<b>Translate to English:</b><br>%1").arg(q.prompt));
        m_input->setPlaceholderText("Enter translation in English...");
    }
    clearInput();
    m_input->setFocus();
}

void TranslationWidget::clearInput() {
    m_input->clear();
}

QString TranslationWidget::getHint() const {
    return m_current.hint;
}

bool TranslationWidget::checkAnswer() const {
    return m_input->text().trimmed().toLower() == m_current.correctAnswer.toLower();
}

GrammarWidget::GrammarWidget(QWidget* parent) : BaseExerciseWidget(parent) {
    auto* layout = new QVBoxLayout(this);
    m_lblPrompt = new QLabel("", this);
    m_lblPrompt->setAlignment(Qt::AlignCenter);
    m_lblPrompt->setFont(QFont("Arial", 14, QFont::Bold));
    m_lblPrompt->setWordWrap(true);
    m_lblPrompt->setTextFormat(Qt::RichText);

    m_radioGroup = new QGroupBox("Choose the correct answer:", this);
    m_radioGroup->setToolTip("Select one option and press Enter or click Submit");
    m_radioLayout = new QVBoxLayout(m_radioGroup);

    layout->addWidget(m_lblPrompt);
    layout->addSpacing(10);
    layout->addWidget(m_radioGroup);
    layout->addSpacing(10);
}

void GrammarWidget::setQuestion(const Question& q) {
    m_current = q;
    m_lblPrompt->setText(QString("<b>Complete the sentence:</b><br>%1").arg(q.prompt));
    qDeleteAll(m_radios);
    m_radios.clear();
    QLayoutItem* item;
    while ((item = m_radioLayout->takeAt(0))) {
        delete item->widget();
    }
    for (int i = 0; i < q.options.size(); ++i) {
        auto* rb = new QRadioButton(q.options[i], this);
        rb->setFont(QFont("Arial", 11));
        rb->setToolTip(QString("Option %1: %2").arg(QChar('A' + i)).arg(q.options[i]));
        m_radios << rb;
        m_radioLayout->addWidget(rb);
    }
}

void GrammarWidget::clearInput() {
    for (auto* rb : m_radios) {
        rb->setChecked(false);
    }
}

QString GrammarWidget::getHint() const {
    return m_current.hint;
}

bool GrammarWidget::checkAnswer() const {
    for (int i = 0; i < m_radios.size(); ++i) {
        if (m_radios[i]->isChecked()) {
            return (i == m_current.correctIndex);
        }
    }
    return false;
}