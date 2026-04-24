#pragma once

#include <QWidget>

#include "core/models.h"

class QLabel;
class QTextEdit;
class QPushButton;
class QRadioButton;
class QButtonGroup;
class QVBoxLayout;

class StartPage : public QWidget {
    Q_OBJECT

public:
    explicit StartPage(QWidget* parent = nullptr);

    void setDifficultyText(const QString& text);

signals:
    void translationRequested();
    void grammarRequested();

private:
    QLabel* difficultyLabel_ = nullptr;
};

class TranslationPage : public QWidget {
    Q_OBJECT

public:
    explicit TranslationPage(QWidget* parent = nullptr);

    void setTask(const TranslationTask& task);
    QString currentAnswer() const;
    QString helpText() const;
    void clearInput();
    void focusInput();

signals:
    void submitRequested();

private:
    QLabel* promptLabel_ = nullptr;
    QTextEdit* answerEdit_ = nullptr;
    QString helpText_;
};

class GrammarPage : public QWidget {
    Q_OBJECT

public:
    explicit GrammarPage(QWidget* parent = nullptr);

    void setTask(const GrammarTask& task);
    int selectedIndex() const;
    QString helpText() const;
    void clearSelection();

signals:
    void submitRequested();

private:
    QLabel* sentenceLabel_ = nullptr;
    QVBoxLayout* optionsLayout_ = nullptr;
    QButtonGroup* buttonGroup_ = nullptr;
    QString helpText_;
};
