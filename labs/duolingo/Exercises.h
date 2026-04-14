#ifndef EXERCISES_H
#define EXERCISES_H

#include <QtCore/QString>
#include <QtCore/QStringList>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QRadioButton>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class TranslationWidget : public QWidget {
    Q_OBJECT
   public:
    explicit TranslationWidget(QWidget* parent = nullptr);
    void setTask(const QString& text);
    QString getAnswer() const;
   signals:
    void submitted();
    void playAudioRequested();

   private:
    QLabel* taskLabel;
    QLineEdit* inputField;
    QPushButton* submitBtn;
    QPushButton* audioBtn;
};

class GrammarWidget : public QWidget {
    Q_OBJECT
   public:
    explicit GrammarWidget(QWidget* parent = nullptr);
    void setTask(const QString& question, const QStringList& options);
    void selectOption(int index);
    QString getSelected() const;
   signals:
    void submitted();

   private:
    QLabel* questionLabel;
    QButtonGroup* group;
    QVBoxLayout* optionsLayout;
    QPushButton* submitBtn;
};

class MathWidget : public QWidget {
    Q_OBJECT
   public:
    explicit MathWidget(QWidget* parent = nullptr);
    void setTask(const QString& expression);
    QString getAnswer() const;
   signals:
    void submitted();

   private:
    QLabel* expressionLabel;
    QLineEdit* answerEdit;
    QPushButton* submitBtn;
};

#endif