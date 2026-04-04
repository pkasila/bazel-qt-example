#pragma once

#include <QColor>
#include <QVector>
#include <QWidget>

#include "pantry_item.h"

class QLabel;
class QLineEdit;
class QComboBox;
class QSpinBox;
class QPushButton;
class QCheckBox;
class QTableWidget;
class QProgressBar;
class ClickLabel;

class MainWindow final : public QWidget {
 public:
  explicit MainWindow(QWidget* parent = nullptr);

 private:
  QVector<int> visibleIndices() const;
  QString stateText(const PantryItem& item) const;
  QColor stateColor(const PantryItem& item) const;

  void loadSamplePantry();
  void addNewItem();
  void saveSelectedItem();
  void consumeOneFromRow(int row);
  void onSelectionChanged();
  void syncEditorFromSelection();
  void refreshTable();
  void refreshSummary(const QString& reason);

  ClickLabel* titleLabel_ = nullptr;
  QLabel* summaryLabel_ = nullptr;
  QProgressBar* fillBar_ = nullptr;

  QLineEdit* newNameEdit_ = nullptr;
  QComboBox* newCategoryBox_ = nullptr;
  QSpinBox* newQuantitySpin_ = nullptr;
  QPushButton* addButton_ = nullptr;

  QLineEdit* editNameEdit_ = nullptr;
  QComboBox* editCategoryBox_ = nullptr;
  QSpinBox* editQuantitySpin_ = nullptr;
  QPushButton* saveButton_ = nullptr;

  QCheckBox* lowStockOnlyCheck_ = nullptr;
  QTableWidget* table_ = nullptr;

  QVector<PantryItem> items_;
  int currentIndex_ = -1;
  bool lowStockOnly_ = false;
  bool updatingUi_ = false;
};
