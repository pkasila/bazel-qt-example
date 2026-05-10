#ifndef GRAMMAR_WIDGET_H
#define GRAMMAR_WIDGET_H

#include <QButtonGroup>
#include <QLabel>
#include <QPushButton>
#include <QRadioButton>
#include <QVBoxLayout>
#include <QWidget>

class GrammarWidget : public QWidget {
    Q_OBJECT
   public:
    explicit GrammarWidget(QWidget* parent = nullptr);
    void setQuestion(const QString& question, const QStringList& options, int correctIndex);

   signals:
    void answerSubmitted(bool correct);

   private slots:
    void onSubmit();

   private:
    QLabel* question_label_;
    QButtonGroup* button_group_;
    QVBoxLayout* options_layout_;
    QPushButton* submit_btn_;
    int correct_index_;
};

#endif  // GRAMMAR_WIDGET_H
