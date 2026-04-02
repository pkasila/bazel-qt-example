#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStack>
#include <QString>
#include <QVector>

class QListWidget;
class QListWidgetItem;
class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QProgressBar;
class QSpinBox;
class QGroupBox;
class QWidget;

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

private:
    enum TicketStatus {
        DefaultStatus = 0,
        YellowStatus = 1,
        GreenStatus = 2,
    };

    struct Ticket {
        QString name;
        QString hint;
        TicketStatus status = DefaultStatus;
    };

    QVector<Ticket> tickets_;
    int current_index_ = -1;
    QStack<int> history_;

    QSpinBox* count_spinbox_ = nullptr;
    QPushButton* minus_button_ = nullptr;
    QPushButton* plus_button_ = nullptr;

    QListWidget* view_ = nullptr;

    QGroupBox* question_box_ = nullptr;
    QLabel* number_label_ = nullptr;
    QLabel* name_label_ = nullptr;
    QLineEdit* name_edit_ = nullptr;
    QComboBox* status_combo_ = nullptr;
    QPushButton* hint_button_ = nullptr;
    QPushButton* next_button_ = nullptr;
    QPushButton* previous_button_ = nullptr;
    QPushButton* save_button_ = nullptr;
    QPushButton* load_button_ = nullptr;
    QPushButton* reset_button_ = nullptr;
    QProgressBar* total_progress_ = nullptr;
    QProgressBar* green_progress_ = nullptr;

    void SetupUi();
    void RebuildTickets(int count);
    void ResizeTicketsPreserveProgress(int new_count);
    void ResetToInitialState();
    void UpdateView();
    void UpdateSingleItem(int index);
    void UpdateQuestionView();
    void UpdateProgressBars();
    void SelectTicket(int index, bool push_to_history = true);
    int RandomAvailableTicket() const;
    void MarkCurrentAsYellowIfDefault();

    QString DefaultTicketName(int index) const;

    QColor BackgroundColorForStatus(TicketStatus status) const;
    QColor StripeColorForStatus(TicketStatus status) const;

    QWidget* CreateTicketWidget(int index) const;

    QString SaveFilePath() const;
    bool SaveToFile();
    bool LoadFromFile();

private slots:
    void OnCountChanged(int value);
    void OnItemClicked(QListWidgetItem* item);
    void OnItemDoubleClicked(QListWidgetItem* item);
    void OnStatusChanged(int combo_index);
    void OnNameEditReturnPressed();
    void OnHintClicked();
    void OnNextQuestionClicked();
    void OnPreviousQuestionClicked();
    void OnSaveClicked();
    void OnLoadClicked();
    void OnResetClicked();
    void OnPlusClicked();
    void OnMinusClicked();
};

#endif