#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QDialog>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QList>
#include <QListWidget>
#include <QMainWindow>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <cstdlib>
#include <stack>
#include <vector>

// QT_BEGIN_NAMESPACE
// namespace Ui {
// class MainWindow;
// }
// QT_END_NAMESPACE
class ListWidget;
class Ticket;
class ProgressBarGreen;
class ProgressBar;

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();

   private slots:
    void AddElement();

   public:
    // Ui::MainWindow *ui;
    QSpinBox* spinbox;
    ListWidget* listwidget;
    QPushButton* next_question;
    QPushButton* prev_question;
    ProgressBarGreen* green_progress_bar;
    ProgressBar* progress_bar;
};
#endif  // MAINWINDOW_H

class ProgressBarGreen : public QProgressBar {
    Q_OBJECT
   public:
    ProgressBarGreen(MainWindow* parent_);
   public slots:
    void ValueChange();

   private:
    MainWindow* parent;
};

class ProgressBar : public QProgressBar {
    Q_OBJECT
   public:
    ProgressBar(MainWindow* parent_);
   public slots:
    void ValueChange();

   private:
    MainWindow* parent;
};

class QuestionView;

class Ticket {
   public:
    Ticket(QString title, int number);

   public:
    QuestionView* question_view;
};

class QuestionView : public QGroupBox {
    Q_OBJECT
   public:
    QuestionView(QString title, QString number);

   public:
    QWidget* parent;
    QLabel* name;
    QLabel* number;
    QLineEdit* name_edit;
    QComboBox* status;
    bool green = false;
    bool yellow = false;
};

class ListWidget : public QListWidget {
    Q_OBJECT
   public:
    ListWidget(MainWindow* parent_);
    ~ListWidget() = default;
   public slots:
    void AddElement(int val);
    void ElementClicked();
    void CurrentElementChanged();
    void ElementDoubleClicked();
    void StatusChange();
    void NextQuestion();
    void PrevQuestion();
    void NameChanged();

   public:
    MainWindow* parent;
    int i = 1;
    int green = 0;
    int finished;
    int prev;
    int row = 0;
    Ticket* current_ticket;
    std::vector<Ticket*> tickets;
    std::vector<QBoxLayout*> rows;
    QBoxLayout* current_row;
    std::stack<int> history;
};
