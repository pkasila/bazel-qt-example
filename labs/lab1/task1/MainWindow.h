#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "TicketManager.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QLabel>
#include <QLineEdit>
#include <QListWidget>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QWidget>

class MainWindow
    : public QWidget {  // NOLINT(cppcoreguidelines-special-member-functions,hicpp-special-member-functions)
    Q_OBJECT

   public:
    explicit MainWindow(QWidget* parent = nullptr);
    ~MainWindow() override = default;

   private slots:
    void OnTicketCountChanged(int count);
    void OnItemDoubleClicked(QListWidgetItem* item);
    void OnItemClicked(QListWidgetItem* item);
    void OnNextButtonClicked();
    void OnPreviousButtonClicked();
    void OnNameEditFinished();
    void OnStatusChanged(int index);
    void OnResetNamesClicked();
    void OnResetStatusesClicked();
    void OnResetOrderClicked();
    void OnSaveClicked();
    void OnLoadClicked();
    void OnSearchChanged(const QString& text);
    void OnFilterChanged(int index);
    void OnShuffleClicked();
    void OnTicketReset(int id);

   private:  // NOLINT(readability-redundant-access-specifiers)
    void SetupUi();
    void UpdateView();
    void UpdateProgressBars();
    void SelectTicket(int index);
    void StyleItem(QListWidgetItem* item, const Ticket& ticket);

    TicketManager manager_;
    int current_index_{-1};

    QSpinBox* count_spin_box_{nullptr};
    QPushButton* shuffle_button_{nullptr};
    QPushButton* reset_order_button_{nullptr};
    QPushButton* reset_names_button_{nullptr};
    QPushButton* reset_statuses_button_{nullptr};
    QPushButton* save_button_{nullptr};
    QPushButton* load_button_{nullptr};
    QLineEdit* search_edit_{nullptr};
    QComboBox* filter_combo_{nullptr};
    QListWidget* view_list_{nullptr};

    QGroupBox* question_group_{nullptr};
    QLabel* number_label_{nullptr};
    QLabel* name_label_{nullptr};
    QLineEdit* name_edit_{nullptr};
    QComboBox* status_combo_{nullptr};

    QPushButton* next_button_{nullptr};
    QPushButton* prev_button_{nullptr};

    QProgressBar* total_progress_{nullptr};
    QProgressBar* green_progress_{nullptr};
};

#endif  // MAINWINDOW_H
