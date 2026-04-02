#include "grammarexercisewidget.h"
#include "exercisestructures.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QRadioButton>
#include <QButtonGroup>
#include <QPushButton>
#include <QSizePolicy>
#include <QDebug>

GrammarExerciseWidget::GrammarExerciseWidget(QWidget *parent) : BaseExerciseWidget(parent)
{
    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(10, 10, 10, 10);
    mainLayout->setSpacing(10);

    instructionLabel = new QLabel(tr("Choose the correct option:"));
    instructionLabel->setStyleSheet("font-weight: bold;");
    mainLayout->addWidget(instructionLabel);

    sentenceLabel = new QLabel(tr("Sentence with a blank will appear here."));
    sentenceLabel->setWordWrap(true);
    sentenceLabel->setStyleSheet("font-size: 16px; border: 1px solid lightgray; padding: 5px;");
    mainLayout->addWidget(sentenceLabel);

    optionsContainer = new QWidget();
    optionsLayout = new QVBoxLayout(optionsContainer);
    optionsLayout->setContentsMargins(0,0,0,0);
    optionsLayout->setSpacing(5);
    mainLayout->addWidget(optionsContainer);

    buttonGroup = new QButtonGroup(this);

    submitButton = new QPushButton(tr("Submit Answer"));
    submitButton->setMinimumHeight(40);
    submitButton->setStyleSheet("padding: 10px 20px; font-size: 14px;");
    mainLayout->addWidget(submitButton, 0, Qt::AlignRight);

    mainLayout->addStretch();
    setLayout(mainLayout);
}

GrammarExerciseWidget::~GrammarExerciseWidget()
{
    qDeleteAll(radioButtons);
    radioButtons.clear();
}

void GrammarExerciseWidget::loadTask(const QVariant& taskData) {
    currentLoadedTaskData = taskData;
    if (!taskData.canConvert<GrammarTask>()) {
        qWarning() << "GrammarExerciseWidget: Failed to convert taskData to GrammarTask";
        sentenceLabel->setText(tr("Error loading task."));
        return;
    }
    GrammarTask task = taskData.value<GrammarTask>();
    sentenceLabel->setText(task.sentence);

    for (QRadioButton* rb : qAsConst(radioButtons)) {
        buttonGroup->removeButton(rb);
        optionsLayout->removeWidget(rb);
        delete rb;
    }
    radioButtons.clear();

    for (int i = 0; i < task.options.size(); ++i) {
        QRadioButton *rb = new QRadioButton(task.options.at(i));
        rb->setStyleSheet("font-size: 14px;");
        radioButtons.append(rb);
        buttonGroup->addButton(rb, i);
        optionsLayout->addWidget(rb);
    }
    clearFields();
    if (!radioButtons.isEmpty()) {
        radioButtons.first()->setFocus();
    }
}

QString GrammarExerciseWidget::getUserAnswer() const {
    QAbstractButton *checked = buttonGroup->checkedButton();
    return checked ? checked->text() : QString();
}

void GrammarExerciseWidget::clearFields() {
    QAbstractButton *checked = buttonGroup->checkedButton();
    if (checked) {
        buttonGroup->setExclusive(false);
        if (checked) checked->setChecked(false);
        buttonGroup->setExclusive(true);
    }
}

QPushButton* GrammarExerciseWidget::getSubmitButton() const {
    return submitButton;
}
