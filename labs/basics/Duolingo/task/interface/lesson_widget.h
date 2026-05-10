#ifndef LESSON_WIDGET_H
#define LESSON_WIDGET_H

#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QProgressBar>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class LessonWidget : public QWidget {
    Q_OBJECT
   public:
    explicit LessonWidget(QWidget* parent = nullptr);

   private slots:
    void onOptionSelected();
    void onCheckClicked();

   private:
    void setupUi();
    void updateProgress();

    QProgressBar* progress_bar_;
    QLabel* question_label_;
    QList<QPushButton*> options_;
    QPushButton* check_button_;
    int current_progress_;
    QPushButton* selected_button_;
};

#endif  // LESSON_WIDGET_H
