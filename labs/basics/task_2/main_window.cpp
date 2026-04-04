#include "main_window.h"

#include "click_label.h"

#include <QAbstractItemView>
#include <QCheckBox>
#include <QComboBox>
#include <QGroupBox>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QProgressBar>
#include <QPushButton>
#include <QSpinBox>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QVBoxLayout>

#include <algorithm>
#include <cmath>

MainWindow::MainWindow(QWidget* parent) : QWidget(parent) {
  setWindowTitle("Pantry Keeper");
  setMinimumSize(980, 680);

  titleLabel_ = new ClickLabel("Pantry Keeper", [this]() { loadSamplePantry(); }, this);
  titleLabel_->setObjectName("TitleLabel");

  summaryLabel_ = new QLabel(this);
  summaryLabel_->setObjectName("SummaryLabel");
  summaryLabel_->setWordWrap(true);

  fillBar_ = new QProgressBar(this);
  fillBar_->setRange(0, 100);
  fillBar_->setTextVisible(true);

  newNameEdit_ = new QLineEdit(this);
  newNameEdit_->setPlaceholderText("New item name");

  newCategoryBox_ = new QComboBox(this);
  newCategoryBox_->addItems({"Dry goods", "Produce", "Dairy", "Canned", "Snacks", "Frozen"});

  newQuantitySpin_ = new QSpinBox(this);
  newQuantitySpin_->setRange(0, 99);
  newQuantitySpin_->setValue(1);

  addButton_ = new QPushButton("Add item", this);

  editNameEdit_ = new QLineEdit(this);
  editNameEdit_->setPlaceholderText("Selected item name");

  editCategoryBox_ = new QComboBox(this);
  editCategoryBox_->addItems({"Dry goods", "Produce", "Dairy", "Canned", "Snacks", "Frozen"});

  editQuantitySpin_ = new QSpinBox(this);
  editQuantitySpin_->setRange(0, 99);

  saveButton_ = new QPushButton("Save selected item", this);

  lowStockOnlyCheck_ = new QCheckBox("Show only low-stock items", this);

  table_ = new QTableWidget(this);
  table_->setColumnCount(4);
  table_->setHorizontalHeaderLabels({"Item", "Category", "Quantity", "State"});
  table_->setSelectionBehavior(QAbstractItemView::SelectRows);
  table_->setSelectionMode(QAbstractItemView::SingleSelection);
  table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
  table_->horizontalHeader()->setStretchLastSection(true);
  table_->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
  table_->verticalHeader()->setVisible(false);

  auto* infoBox = new QGroupBox("Pantry snapshot", this);
  auto* infoLayout = new QVBoxLayout(infoBox);
  infoLayout->addWidget(titleLabel_);
  infoLayout->addWidget(summaryLabel_);
  infoLayout->addWidget(fillBar_);

  auto* addBox = new QGroupBox("Add new item", this);
  auto* addLayout = new QGridLayout(addBox);
  addLayout->setColumnStretch(1, 1);
  addLayout->addWidget(new QLabel("Name:"), 0, 0);
  addLayout->addWidget(newNameEdit_, 0, 1);
  addLayout->addWidget(new QLabel("Category:"), 1, 0);
  addLayout->addWidget(newCategoryBox_, 1, 1);
  addLayout->addWidget(new QLabel("Quantity:"), 2, 0);
  addLayout->addWidget(newQuantitySpin_, 2, 1);
  addLayout->addWidget(addButton_, 3, 0, 1, 2);

  auto* editBox = new QGroupBox("Edit selected item", this);
  auto* editLayout = new QGridLayout(editBox);
  editLayout->setColumnStretch(1, 1);
  editLayout->addWidget(new QLabel("Name:"), 0, 0);
  editLayout->addWidget(editNameEdit_, 0, 1);
  editLayout->addWidget(new QLabel("Category:"), 1, 0);
  editLayout->addWidget(editCategoryBox_, 1, 1);
  editLayout->addWidget(new QLabel("Quantity:"), 2, 0);
  editLayout->addWidget(editQuantitySpin_, 2, 1);
  editLayout->addWidget(lowStockOnlyCheck_, 3, 0, 1, 2);
  editLayout->addWidget(saveButton_, 4, 0, 1, 2);

  auto* rightColumn = new QVBoxLayout();
  rightColumn->addWidget(addBox);
  rightColumn->addWidget(editBox);
  rightColumn->addStretch(1);

  auto* topRow = new QHBoxLayout();
  topRow->addWidget(infoBox, 1);
  topRow->addLayout(rightColumn, 1);

  auto* mainLayout = new QVBoxLayout(this);
  mainLayout->addLayout(topRow);
  mainLayout->addWidget(table_, 1);

  connect(newNameEdit_, &QLineEdit::returnPressed, this, [this]() { addNewItem(); });
  connect(addButton_, &QPushButton::clicked, this, [this]() { addNewItem(); });

  connect(editNameEdit_, &QLineEdit::returnPressed, this, [this]() { saveSelectedItem(); });
  connect(saveButton_, &QPushButton::clicked, this, [this]() { saveSelectedItem(); });

  connect(lowStockOnlyCheck_, &QCheckBox::toggled, this, [this](bool checked) {
    lowStockOnly_ = checked;
    refreshTable();
    refreshSummary("Filter updated");
  });

  connect(editQuantitySpin_, qOverload<int>(&QSpinBox::valueChanged), this, [this](int value) {
    if (updatingUi_ || currentIndex_ < 0 || currentIndex_ >= items_.size()) {
      return;
    }
    items_[currentIndex_].quantity = value;
    refreshTable();
    refreshSummary("Quantity changed");
  });

  connect(editCategoryBox_, &QComboBox::currentTextChanged, this, [this](const QString& text) {
    if (updatingUi_ || currentIndex_ < 0 || currentIndex_ >= items_.size()) {
      return;
    }
    items_[currentIndex_].category = text;
    refreshTable();
    refreshSummary("Category changed");
  });

  connect(table_, &QTableWidget::itemSelectionChanged, this, [this]() { onSelectionChanged(); });
  connect(table_, &QTableWidget::cellDoubleClicked, this, [this](int row, int) { consumeOneFromRow(row); });

  loadSamplePantry();
}

QVector<int> MainWindow::visibleIndices() const {
  QVector<int> result;
  result.reserve(items_.size());
  for (int i = 0; i < items_.size(); ++i) {
    const bool low = items_[i].quantity <= 2;
    if (!lowStockOnly_ || low) {
      result.push_back(i);
    }
  }
  return result;
}

QString MainWindow::stateText(const PantryItem& item) const {
  if (item.quantity == 0) {
    return "Empty";
  }
  if (item.quantity <= 2) {
    return "Low stock";
  }
  return "Fine";
}

QColor MainWindow::stateColor(const PantryItem& item) const {
  if (item.quantity == 0) {
    return QColor("#fde2e2");
  }
  if (item.quantity <= 2) {
    return QColor("#fff2cc");
  }
  return QColor("#e7f5e7");
}

void MainWindow::loadSamplePantry() {
  items_ = {
      {"Rice", "Dry goods", 4},
      {"Milk", "Dairy", 1},
      {"Apples", "Produce", 3},
      {"Beans", "Canned", 5},
      {"Tea", "Dry goods", 2},
  };
  currentIndex_ = items_.isEmpty() ? -1 : 0;
  lowStockOnly_ = false;
  updatingUi_ = true;
  lowStockOnlyCheck_->setChecked(false);
  updatingUi_ = false;
  refreshTable();
  syncEditorFromSelection();
  refreshSummary("Sample pantry loaded");
}

void MainWindow::addNewItem() {
  const QString name = newNameEdit_->text().trimmed();
  if (name.isEmpty()) {
    return;
  }

  items_.push_back({name, newCategoryBox_->currentText(), newQuantitySpin_->value()});
  currentIndex_ = items_.size() - 1;

  updatingUi_ = true;
  newNameEdit_->clear();
  newQuantitySpin_->setValue(1);
  newCategoryBox_->setCurrentIndex(0);
  updatingUi_ = false;

  refreshTable();
  syncEditorFromSelection();
  refreshSummary("New item added");
}

void MainWindow::saveSelectedItem() {
  const QString name = editNameEdit_->text().trimmed();
  if (name.isEmpty()) {
    return;
  }
  if (currentIndex_ < 0 || currentIndex_ >= items_.size()) {
    return;
  }

  items_[currentIndex_].name = name;
  items_[currentIndex_].category = editCategoryBox_->currentText();
  items_[currentIndex_].quantity = editQuantitySpin_->value();

  refreshTable();
  refreshSummary("Item saved");
}

void MainWindow::consumeOneFromRow(int row) {
  const QVector<int> visible = visibleIndices();
  if (row < 0 || row >= visible.size()) {
    return;
  }

  const int index = visible[row];
  currentIndex_ = index;
  if (items_[index].quantity > 0) {
    --items_[index].quantity;
  }
  refreshTable();
  syncEditorFromSelection();
  refreshSummary("One item used");
}

void MainWindow::onSelectionChanged() {
  if (updatingUi_) {
    return;
  }

  const auto* current = table_->currentItem();
  if (!current) {
    currentIndex_ = -1;
    syncEditorFromSelection();
    return;
  }

  const int row = current->row();
  const QVector<int> visible = visibleIndices();
  if (row < 0 || row >= visible.size()) {
    currentIndex_ = -1;
    syncEditorFromSelection();
    return;
  }

  currentIndex_ = visible[row];
  syncEditorFromSelection();
  refreshSummary("Selection changed");
}

void MainWindow::syncEditorFromSelection() {
  updatingUi_ = true;
  if (currentIndex_ < 0 || currentIndex_ >= items_.size()) {
    editNameEdit_->clear();
    editQuantitySpin_->setValue(0);
    editCategoryBox_->setCurrentIndex(0);
    updatingUi_ = false;
    return;
  }

  const PantryItem& item = items_[currentIndex_];
  editNameEdit_->setText(item.name);
  editQuantitySpin_->setValue(item.quantity);
  const int idx = editCategoryBox_->findText(item.category);
  editCategoryBox_->setCurrentIndex(idx >= 0 ? idx : 0);
  updatingUi_ = false;
}

void MainWindow::refreshTable() {
  updatingUi_ = true;
  const QVector<int> visible = visibleIndices();
  table_->setRowCount(visible.size());

  for (int row = 0; row < visible.size(); ++row) {
    const int index = visible[row];
    const PantryItem& item = items_[index];

    auto* nameItem = new QTableWidgetItem(item.name);
    nameItem->setData(Qt::UserRole, index);
    auto* categoryItem = new QTableWidgetItem(item.category);
    auto* qtyItem = new QTableWidgetItem(QString::number(item.quantity));
    auto* stateItem = new QTableWidgetItem(stateText(item));

    const QColor bg = stateColor(item);
    nameItem->setBackground(bg);
    categoryItem->setBackground(bg);
    qtyItem->setBackground(bg);
    stateItem->setBackground(bg);

    table_->setItem(row, 0, nameItem);
    table_->setItem(row, 1, categoryItem);
    table_->setItem(row, 2, qtyItem);
    table_->setItem(row, 3, stateItem);
  }

  table_->clearSelection();
  for (int row = 0; row < visible.size(); ++row) {
    if (visible[row] == currentIndex_) {
      table_->selectRow(row);
      break;
    }
  }
  updatingUi_ = false;
}

void MainWindow::refreshSummary(const QString& reason) {
  int stocked = 0;
  int low = 0;
  int empty = 0;
  for (const auto& item : items_) {
    if (item.quantity == 0) {
      ++empty;
    } else if (item.quantity <= 2) {
      ++low;
    } else {
      ++stocked;
    }
  }

  const int total = std::max(1, items_.size());
  const int filledPercent = static_cast<int>(std::round((100.0 * stocked) / total));
  fillBar_->setValue(filledPercent);
  fillBar_->setFormat(QString("Shelf in good shape: %p%"));

  summaryLabel_->setText(
      QString("%1\nItems: %2 | stocked: %3 | low stock: %4 | empty: %5")
          .arg(reason)
          .arg(items_.size())
          .arg(stocked)
          .arg(low)
          .arg(empty));
}
