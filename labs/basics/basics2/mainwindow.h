#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QLabel>
#include <QLineEdit>
#include <QMainWindow>
#include <QMenu>
#include <QMovie>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>

// NOLINTBEGIN
QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}  // namespace Ui

QT_END_NAMESPACE

class Answer;
class CustomLabel;
class RandomField;

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    MainWindow(QWidget* parent = nullptr);
    ~MainWindow();
    QComboBox* Status() {
        return combo_box2_;
    }
    QLineEdit* Sequence() {
        return line2_;
    }
   public slots:
    void Action();
    void Action2();

   private:
   void clear();
    Ui::MainWindow* ui_ = nullptr;
    QWidget* widget_= nullptr;
    CustomLabel* label_= nullptr;
    QLineEdit* question_= nullptr;
    QPushButton* button_= nullptr;
    Answer* answer_= nullptr;
    QComboBox* combo_box2_= nullptr;
    QLabel* label2_= nullptr;
    QLineEdit* line2_= nullptr;
    RandomField* sequence_= nullptr;
    QPushButton* button2_ = nullptr;
    QMenu* options_= nullptr;
};

class Answer : public QLabel {
    Q_OBJECT
   public slots:
    void GenerateAnswer();
};

class CustomLabel : public QLabel {
    Q_OBJECT
   public:
    CustomLabel() = default;
   public slots:
    void ChangeText1();
    void ChangeText2();
};

class RandomField : public QLabel {
    Q_OBJECT
    public: 
    RandomField(MainWindow* parent) : parent_(parent) {
    }
    public slots:
    void Random();
    private:
    MainWindow* parent_;
};

// NOLINTEND
#endif  // MAINWINDOW_H
