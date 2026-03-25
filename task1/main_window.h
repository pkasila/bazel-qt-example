#pragma once

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QVBoxLayout>
#include <QWidget>
#include <vector>

struct Ticket {
    QString name;
    int status = 0;  // 0 - Default, 1 - Yellow, 2 - Green
};

class MainWindow : public QWidget {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);

   private slots:
    void onCountChanged(int count);
    void onItemClicked(QListWidgetItem* item);
    void onItemDoubleClicked(QListWidgetItem* item);
    void onNameEditReturnPressed();
    void onStatusChanged(int index);
    void onNextClicked();
    void onPrevClicked();

   private:
    void updateProgress();
    void updateItemColor(int index);
    void showQuestion(int index);

    std::vector<Ticket> tickets_;
    int currentIndex_ = -1;

    QSpinBox* countSpinBox_;
    QListWidget* listView_;

    QGroupBox* questionView_;
    QLabel* numberLabel_;
    QLabel* nameLabel_;
    QLineEdit* nameEdit_;
    QComboBox* statusCombo_;

    QPushButton* nextQuestionBtn_;
    QPushButton* prevQuestionBtn_;

    QProgressBar* totalProgress_;
    QProgressBar* greenProgress_;

    std::vector<int> history_;
};
