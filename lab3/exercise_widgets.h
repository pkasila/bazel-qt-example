#pragma once

#include "exercise_data.h"

#include <QString>
#include <QWidget>
#include <functional>

class QLabel;
class QLineEdit;
class QGroupBox;
class QVBoxLayout;
class QRadioButton;

class BaseExerciseWidget : public QWidget {
   public:
    using AnswerCallback = std::function<void(bool)>;
    using EnterCallback = std::function<void()>;
    explicit BaseExerciseWidget(QWidget* parent = nullptr);
    virtual void setQuestion(const Question& q) = 0;
    virtual void clearInput() = 0;
    virtual QString getHint() const = 0;
    virtual bool checkAnswer() const = 0;
    void setOnAnswerSubmitted(AnswerCallback cb);
    void setOnEnterPressed(EnterCallback cb);

   protected:
    AnswerCallback m_onAnswer;
    EnterCallback m_onEnter;
};

class TranslationWidget : public BaseExerciseWidget {
   public:
    explicit TranslationWidget(QWidget* parent = nullptr);
    void setQuestion(const Question& q) override;
    void clearInput() override;
    QString getHint() const override;
    bool checkAnswer() const override;

   private:
    Question m_current;
    QLabel* m_lblPrompt;
    QLineEdit* m_input;
};

class GrammarWidget : public BaseExerciseWidget {
   public:
    explicit GrammarWidget(QWidget* parent = nullptr);
    void setQuestion(const Question& q) override;
    void clearInput() override;
    QString getHint() const override;
    bool checkAnswer() const override;

   private:
    Question m_current;
    QLabel* m_lblPrompt;
    QGroupBox* m_radioGroup;
    QVBoxLayout* m_radioLayout;
    QList<QRadioButton*> m_radios;
};