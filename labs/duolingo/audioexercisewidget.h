#pragma once

#include <QWidget>
#include "leveldata.h"

class QPushButton;
class QLineEdit;
class QLabel;
class QMediaPlayer;
class QAudioOutput;
class QVBoxLayout;

class AudioExerciseWidget final : public QWidget {
    Q_OBJECT
public:
    explicit AudioExerciseWidget(QWidget* parent = nullptr);
    ~AudioExerciseWidget() override;

    void setQuestion(const Question& q);
    QString getAnswer() const;
    void showCorrectAnswer(const QString& correctAnswer, bool wasUserCorrect) const;

    QPushButton* submitButton_;

    void setAnswerEditReadOnly(bool readOnly) const;
    bool isAnswerEditValid() const;

private slots:
    void playAudio() const;

private:
    QPushButton* playButton_ = nullptr;
    QLineEdit *answerEdit_ = nullptr;
    QMediaPlayer* audioPlayer_ = nullptr;
    QAudioOutput* audioOutput_ = nullptr;
    QString currentAudioPath_;
    QLabel *correctAnswerFeedbackLabel_ = nullptr;
};