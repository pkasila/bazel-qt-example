#include "../../labs/duolingo/exercise_widget.h"

#include <QMessageBox>
#include <QSoundEffect>
#include <qtmetamacros.h>

ExerciseWidget::ExerciseWidget(QWidget *parent) : QWidget(parent) {
}

void ExerciseWidget::OnSubmit() {
    const bool ok = ValidateAnswer();
    emit ExerciseAnswered(ok);
}

void ExerciseWidget::ShowMessageBox() {
    if (ValidateAnswer()) {
        QSoundEffect effect;
        effect.setSource(QUrl::fromLocalFile("labs/duolingo/data/correct.wav"));
        effect.play();
        QMessageBox::information(this, "Correct!", "Your answer is correct!");
    } else {
        QSoundEffect effect;
        effect.setSource(QUrl::fromLocalFile("labs/duolingo/data/wrong.wav"));
        effect.play();
        QMessageBox::critical(this, "Wrong!", "Your answer is wrong! The right answer is " + answer_);
    }
}
