#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QListWidgetItem>
#include <QMainWindow>
#include <QObject>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QString>
#include <QVBoxLayout>
#include <QRandomGenerator>
#include <QStack>
#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QInputDialog>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE

namespace Ui {
class MainWindow;
}  // namespace Ui

QT_END_NAMESPACE

class MainWindow : public QMainWindow {
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);

   protected:
    void mousePressEvent(QMouseEvent *event) override;  // Обработка кликов вне списка

   private slots:
    void onSpinBoxValueChanged(int value);
    void onListItemClicked(QListWidgetItem *item);
    void onNameEditEnterPressed();
    void onStatusChanged(int index);
    void onListItemDoubleClicked(QListWidgetItem* item);
    void onNextQuestionClicked();
    void onPreviousQuestionClicked();
    void updateProgressBars();
    void onShowHintClicked();
    void onSaveProgressClicked();
    void onLoadProgressClicked();
    void onItemDoubleClicked(QListWidgetItem *item);
    void clearTicketInfo();
   private:
    QSpinBox* spinBox;
    QListWidget* listWidget;
    QLabel* nameLabel;
    QLabel* numberLabel;
    QLineEdit* nameEdit;
    QComboBox* comboBox;
    QPushButton* nextQuestionButton;
    QPushButton* previousQuestionButton;
    QPushButton *showHintButton;
    QPushButton *saveProgressButton;
    QPushButton *loadProgressButton;
    QProgressBar* totalProgress;
    QProgressBar* greenProgress;
    QListWidgetItem* selectedItem = nullptr;
    QMap<QListWidgetItem*, int> ticketStatuses;
    QMap<QListWidgetItem*, int> ticketNumbers;
    QMap<QListWidgetItem*, QString> ticketHints;
    QStack<QListWidgetItem*> historyStack;
    QListWidgetItem* currentQuestion = nullptr;
    int totalCount = 0;
    int totalLearned = 0;
    int greenLearned = 0;

    void updateItemColor(QListWidgetItem* item);
    void setActiveQuestion(QListWidgetItem* item);
};
#endif  // MAINWINDOW_H
