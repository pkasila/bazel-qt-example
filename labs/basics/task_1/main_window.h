#pragma once

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QtWidgets>
#include <set>
#include <vector>

QT_BEGIN_NAMESPACE
class QAction;
class QActionGroup;
class QLabel;
class QMenu;
QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

    public:
       MainWindow();

    private slots:
       void resetAll();
       void showQuestionView(QListWidgetItem* item);
       void changeName(int index, QString text);
       void changeStatus(int box_index);
       void changeStatusOnDoubleClick(QListWidgetItem* item);
       void toPreviousQuestion();
       void toNextQuestion();

    private:
       void createActions();
       void changeDescription(int index, const QString& text);

       QVBoxLayout* main_layout;
       QSpinBox* count;
       QListWidget* view;
       QComboBox* status;
       QPushButton* next_question;
       QPushButton* previous_question;
       QProgressBar* total_progress;
       QProgressBar* green_progress;
       QVector<QString> descriptions;

       std::vector<QGroupBox*> question_view;
       std::vector<QLabel*> names;
       std::vector<QGroupBox*> question_view_history;
       std::set<int> white_and_yellow;
       int white_amount;
};

#endif
