#ifndef SIDEBAR_WIDGET_H
#define SIDEBAR_WIDGET_H

#include <QLabel>
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

class SidebarWidget : public QWidget {
    Q_OBJECT
   public:
    explicit SidebarWidget(QWidget* parent = nullptr);
    void setScore(int score);
    void setHearts(int hearts);
    void setActiveButton(int index);

   signals:
    void translationClicked();
    void grammarClicked();

   private:
    void setupUi();

    QLabel* score_label_;
    QLabel* hearts_label_;
    QList<QPushButton*> nav_buttons_;
};

#endif  // SIDEBAR_WIDGET_H
