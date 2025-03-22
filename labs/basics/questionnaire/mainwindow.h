#pragma once

#include <QMainWindow>
#include <QtCore/QVariant>
#include <QtWidgets/QApplication>
#include <QtWidgets/QListWidget>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QMenuBar>
#include <QtWidgets/QProgressBar>
#include <QtWidgets/QPushButton>
#include <QtWidgets/QSpinBox>
#include <QtWidgets/QStatusBar>
#include <QtWidgets/QVBoxLayout>
#include <QtWidgets/QWidget>

class Flag
{
public:
    enum State { Default, Green, Yellow };
    void rotate();
    void rotate_backward();
    [[nodiscard]] QIcon icon() const;
    int state = Default;
};

struct Question
{
    QString name;
    QString desc;
    Flag flag;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
public:
    explicit MainWindow(QWidget *parent = nullptr);

private slots:
    void on_spinBox_valueChanged(int val);
    void on_previousButton_clicked();
    void on_nextButton_clicked();
    void on_shuffleButton_clicked();
    void on_listView_itemClicked();
    void on_listView_itemDoubleClicked();
    void update_progress_bar();
    void add_question(int id, QString desc = "");
    void shuffle_remaining();

public:
    QListWidget *listView;
    QProgressBar *progressBar;
    QLineEdit *lineEdit;
    QSpinBox *spinBox;

    QList<Question> questions;
    QList<int> issued;
    QList<int> remaining;
    int clickItemId;
    std::optional<int> lastShownItemId;
};
